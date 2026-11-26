# Asset-free actual SDK rasterization lab

This Windows Actions integration test compiles the actual SDK D3D12 renderer,
shader translator, vertex/texture code, threaded queue and kernel allocator from
`99078b119b5d5e0d1d6afb1390e29437c0c984c0`, with ImGui v1.91.0 at
`8199457a7d9e453f8d3d9cadc14683fb54a858b5`. It requires Visual Studio clang-cl
because the pinned SDK uses GNU attributes and builtins. Compilation is bounded
to two workers and the job to 20 minutes.

The sole SDK deviation is adapter selection in a disposable translation-unit
copy: require `EnumWarpAdapter`, verify the software-adapter flag and prohibit
fallback. Preparation checks the exact SDK commit, original source SHA256 and
replacement anchor before writing. Original SDK files and production code are
unchanged. This is real D3D12 software rasterization through the full backend,
not a hardware performance test, toy rasterizer or shader-only compilation.

The harness supplies a hidden HWND, an ImGui context and minimal host interfaces.
It initializes actual guest memory, creates its own color/depth surfaces using
the pinned texture-header decoder, then exercises `PrepareModPlaceholderDraw`
and `SubmitModPlaceholder` with the actual `ModPlaceholderUploadKernel` and
`CreateThreadedBackend(CreateBackend())`. The real queue fence must complete
before upload storage is freed. Instrumentation checks resource busy state,
seven released header/data pairs and unchanged allocation totals/input State.

The full-ticket presentation decoder and batch submitter also receive a
synthetic four-human facade frame and render three separated peer markers.
Three center colors and untouched exterior are checked; rejected scope must
allocate nothing. This tests the real rendering path, not the live JNI or
BLACK camera.

`Backend::Sync` reads back owned color pixels. Assertions check clear color,
box color, untouched exterior, occlusion by an owned cleared depth plane and
visibility after depth clear. Drawing a farther box over a nearer box verifies
that placeholders do not write depth. The SDK's Sync API does not expose depth
readback; this lab reports depth behavior through rasterized color, not a depth
pixel dump. Any Init, compilation or readback failure fails the job.

Artifacts contain only synthetic test diagnostics, pin/deviation metadata and
licenses. No game image, generated guest source, APK, binary, owner log, save or
secret is fetched or uploaded. Passing this lab establishes neither BLACK's live
camera nor a visible remote body, Android rendering, shared gameplay or co-op.
