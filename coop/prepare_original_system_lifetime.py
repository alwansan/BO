"""Stage exact system allocation lifetime evidence in a disposable pinned SDK.
Does not infer original game heap suballocation ownership or enable gameplay.
"""
from pathlib import Path

CPP = "runtime/kernel/memory.cpp"
HDR = "runtime/kernel/memory.h"

def once(s, a, b):
    if s.count(a) != 1:
        raise ValueError("Pinned SDK memory anchor changed")
    return s.replace(a, b, 1)

def patch(cpp, hdr):
    hdr = once(hdr, "void FreeSystem(uint32_t va);",
        "void FreeSystem(uint32_t va);\n"
        "// Exact live system allocation only; 0 means unavailable.\n"
        "uint64_t CoopSystemAllocationGeneration(uint32_t va, uint32_t size) noexcept;")
    cpp = once(cpp, "static std::recursive_mutex g_lock;", """static std::recursive_mutex g_lock;

// BO-COOP: exact system allocator lifetime evidence under existing g_lock.
// Never identify an arbitrary game heap suballocation from a backing region.
struct CoopSystemLife { uint32_t size; uint64_t generation; };
static std::map<uint32_t, CoopSystemLife> g_coop_system_life;
static uint64_t g_coop_next_generation = 0;
static bool g_coop_lifetime_disabled = false;
static void CoopRecordSystemAllocation(uint32_t at, uint32_t size) noexcept {
    if (g_coop_lifetime_disabled) return;
    if (g_coop_next_generation == UINT64_MAX || g_coop_system_life.count(at)) {
        g_coop_lifetime_disabled = true;
        g_coop_system_life.clear();
        return;
    }
    try {
        g_coop_system_life.emplace(at, CoopSystemLife{size, ++g_coop_next_generation});
    } catch (...) {
        // Diagnostics must not change allocator success or throw into the game.
        g_coop_lifetime_disabled = true;
        g_coop_system_life.clear();
    }
}
static void CoopInvalidateSystemPages(uint64_t first, uint64_t end) noexcept {
    if (end <= kSystemBase || first >= uint64_t(kSystemBase)+kSystemSize) return;
    for (auto it = g_coop_system_life.begin(); it != g_coop_system_life.end();) {
        if (uint64_t(it->first) < end && uint64_t(it->first)+it->second.size > first)
            it = g_coop_system_life.erase(it);
        else ++it;
    }
}""")
    cpp = once(cpp, "    for (uint64_t p = first; p < end; p += kPage) g_committed[size_t(p >> 12)] = 0;",
        "    CoopInvalidateSystemPages(first, end);\n"
        "    for (uint64_t p = first; p < end; p += kPage) g_committed[size_t(p >> 12)] = 0;")
    cpp = once(cpp, """    Commit(a, size);
    std::memset(g_base + a, 0, size);
    return a;
}

void FreeSystem(uint32_t va) {
    std::lock_guard<std::recursive_mutex> lock(g_lock);
    g_system.Free(va);
}""", """    Commit(a, size);
    std::memset(g_base + a, 0, size);
    CoopRecordSystemAllocation(a, size);
    return a;
}

void FreeSystem(uint32_t va) {
    std::lock_guard<std::recursive_mutex> lock(g_lock);
    if (g_system.Free(va)) g_coop_system_life.erase(va);
}

uint64_t CoopSystemAllocationGeneration(uint32_t va, uint32_t size) noexcept {
    std::lock_guard<std::recursive_mutex> lock(g_lock);
    if (!size || g_coop_lifetime_disabled) return 0;
    auto it = g_coop_system_life.find(va);
    if (it == g_coop_system_life.end() || it->second.size != size ||
        g_system.SizeOf(va) != size) return 0;
    const uint64_t end = uint64_t(va) + size;
    for (uint64_t p = va & ~uint64_t(kPage-1); p < end; p += kPage)
        if (!IsCommitted(static_cast<uint32_t>(p))) return 0;
    return it->second.generation;
}""")
    hdr = once(hdr, "void FreeContiguous(uint32_t va);",
        "void FreeContiguous(uint32_t va);\n"
        "// Exact live physical allocator block, canonical VA only.\\n"
        "uint64_t CoopContiguousAllocationGeneration(uint32_t va, uint32_t size) noexcept;")
    cpp = once(cpp, """    Commit(a, size);
    std::memset(g_base + a, 0, size);
    return a;
}

void FreeContiguous(uint32_t va) {
    std::lock_guard<std::recursive_mutex> lock(g_lock);
    uint32_t size;
    if (g_contig.Free(va, &size)) Decommit(va, size);
}""", """    Commit(a, size);
    std::memset(g_base + a, 0, size);
    // Same generation namespace as system allocations, disjoint canonical
    // address ranges. Never grant provenance to the write-combine alias.
    CoopRecordSystemAllocation(a, size);
    return a;
}

void FreeContiguous(uint32_t va) {
    std::lock_guard<std::recursive_mutex> lock(g_lock);
    uint32_t size;
    if (g_contig.Free(va, &size)) {
        g_coop_system_life.erase(va);
        Decommit(va, size);
    }
}

uint64_t CoopContiguousAllocationGeneration(uint32_t va, uint32_t size) noexcept {
    std::lock_guard<std::recursive_mutex> lock(g_lock);
    if (!size || va < kContiguousBase + kKernelDataSize ||
        uint64_t(va)+size > uint64_t(kContiguousBase)+kPhysicalSize ||
        g_coop_lifetime_disabled) return 0;
    auto it = g_coop_system_life.find(va);
    if (it == g_coop_system_life.end() || it->second.size != size ||
        g_contig.SizeOf(va) != size) return 0;
    return it->second.generation;
}""")
    return cpp, hdr

def prepare(sdk):
    root = Path(sdk)
    cpp, hdr = root / CPP, root / HDR
    a, b = patch(cpp.read_text(encoding="utf-8"), hdr.read_text(encoding="utf-8"))
    cpp.write_text(a, encoding="utf-8")
    hdr.write_text(b, encoding="utf-8")
    return [CPP, HDR]

if __name__ == "__main__":
    import argparse
    p = argparse.ArgumentParser()
    p.add_argument("--sdk", required=True)
    print("BO_COOP_SYSTEM_LIFETIME_SOURCE_STAGED", prepare(p.parse_args().sdk))
