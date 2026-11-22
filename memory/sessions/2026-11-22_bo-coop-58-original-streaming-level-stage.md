# Session58 — original BLACK streaming level/stage source candidate

Date 2026-11-22. Sole writable branch `alwansan/BO:experiment/black-coop`. Owner priority: one real host-authoritative original campaign for 2–4 real human players, any existing bot appearance temporarily, never borrowed NPC AI, original first-person camera/weapon per phone, NO phone micro-tests.

## New direct public source lead (not yet proven on owner's c5 XBE)
Pinned MIT `portingpete/Black-2006-Recomp@b2512fafd3e181e4febfb298f297245b845ef628` `scripts/generate.py` specifically patches `sub_001EDC90`: it reads low bytes at offsets `+0x2020C` and `+0x2020E` into the x86 EDX/EAX low lanes and calls `black_level_override(&lv_,&sg_)`. Pinned `src/recomp_manual.c` calls these frontend-chosen level and stage respectively; its debug-only env override is NOT to be copied as automatic co-op launch. Pinned `src/black_checkpoint.inc` separately describes `sub_001EDC90:0x1EE310` as a point before initial streaming, after level/gun resources are ready. Together these are stronger targeted source evidence for ORIGINAL LEVEL and STAGE bytes than menu WORD candidate A/B/C, but do NOT prove our original Xbox XBE byte semantics/loader ABI, selected mission or stage COMPLETION/READY. Never assume level folder is mission index.

## Code
Added isolated `coop/analysis/original_streaming_stage_xbe.py` and synthetic regression and private workflow. It validates exact owner XBE SHA1, disassembles one bounded 0x900-byte window around original `0x001EDC90` in disposable runner, counts only direct BYTE PTR register loads from candidate `+0x2020C` / `+0x2020E`, and exports numeric counts plus relative positions, never game code or resource bytes. Not a CFG, loaded scene or runtime proof. No game memory/gameplay/net changes.

## Follow-up
Check new [Actions workflow](https://github.com/alwansan/BO/actions/workflows/black-coop-original-streaming-source.yml) synthetic and original private analysis. A positive owner XBE scan would justify a read-only native hook at a verified streaming ENTRY for host-selected level and stage and correlation with original file opens and player state, still not READY. Next prove safe exact guest engine stage launch and real stage loaded callback, independent bot-looking human bodies and one host AI/quest/combat world. No phone tests for this probe.
