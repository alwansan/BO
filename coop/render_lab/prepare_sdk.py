"""Prepare one disposable, asset-free WARP test translation unit; never edit SDK."""
import argparse
import hashlib
import json
import pathlib
import subprocess

SDK_COMMIT = "99078b119b5d5e0d1d6afb1390e29437c0c984c0"
SOURCE_SHA256 = "2838352eada34bc031ef7abfb926360e1b8369d53c89bbd66fd5faae0ee3d8c1"
OLD = '''    // dedicated GPU first on hybrid laptops
    ComPtr<IDXGIFactory6> f6;
    ComPtr<IDXGIAdapter1> adapter;
    if (SUCCEEDED(factory_.As(&f6))) {
        f6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
    }
    if (FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&dev_))) &&
        FAILED(D3D12CreateDevice((adapter = nullptr).Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&dev_)))) {
        LOGE("d3d12: no device");
        return false;
    }
'''
NEW = '''    // TEST ONLY: the CI lab explicitly requires Microsoft's software adapter.
    ComPtr<IDXGIAdapter1> adapter;
    if (FAILED(factory_->EnumWarpAdapter(IID_PPV_ARGS(&adapter)))) return false;
    DXGI_ADAPTER_DESC1 labAdapter{};
    if (FAILED(adapter->GetDesc1(&labAdapter)) ||
        !(labAdapter.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)) return false;
    if (FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&dev_)))) {
        LOGE("render lab: required WARP device unavailable");
        return false;
    }
'''


def prepare(sdk, output):
    sdk, output = sdk.resolve(), output.resolve()
    if output == sdk or sdk in output.parents:
        raise ValueError("Test output must be outside the pinned SDK")
    if output.exists() and any(output.iterdir()):
        raise ValueError("Refuse to overwrite existing test outputs")
    head = subprocess.check_output(["git", "-C", str(sdk), "rev-parse", "HEAD"], text=True).strip()
    if head != SDK_COMMIT:
        raise ValueError("SDK commit mismatch")
    source = sdk / "runtime/gpu/d3d12.cpp"
    raw = source.read_bytes()
    # Git-for-Windows line endings are not a source modification.
    normalized = raw.replace(b"\r\n", b"\n")
    if hashlib.sha256(normalized).hexdigest() != SOURCE_SHA256:
        raise ValueError("SDK D3D12 source hash mismatch")
    text = normalized.decode("utf-8")
    if text.count(OLD) != 1:
        raise ValueError("Unknown adapter selection anchor")
    output.mkdir(parents=True, exist_ok=True)
    generated = text.replace(OLD, NEW)
    (output / "d3d12_warp.cpp").write_text(generated, encoding="utf-8", newline="\n")
    report = {"sdk_commit": head, "original_d3d12_sha256": SOURCE_SHA256,
              "test_d3d12_sha256": hashlib.sha256(generated.encode()).hexdigest(),
              "deviation": "Adapter selection only: require EnumWarpAdapter; no hardware/default fallback",
              "production_sdk_modified": False, "assets_used": False,
              "rasterization_verified": False}
    (output / "source-manifest.json").write_text(json.dumps(report, indent=2), encoding="utf-8")
    return report


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sdk", required=True, type=pathlib.Path)
    parser.add_argument("--output", required=True, type=pathlib.Path)
    args = parser.parse_args()
    print(json.dumps(prepare(args.sdk.resolve(), args.output.resolve()), indent=2))
