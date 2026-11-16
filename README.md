# Godot 4 for Nintendo Switch (overlay)

This repository contains a *Godot 4* platform overlay targeting the Nintendo Switch architecture via [devkitPro](https://devkitpro.org/) and [mesa-switch](https://github.com/danfromtico/mesa-switch.git). See the original [Godot Engine docs](https://docs.godotengine.org/en/stable/) for info about Godot.

> [!NOTE]
> **EXPERIMENTAL BUILD**
> Built and tested on real hardware (**Nintendo Switch Lite**) with real game.
> See [What's Working](#whats-working) below for supported features and things that still need testing.

## What's Working

Only core features are implemented and tested so far:

- ✅ **Vulkan Renderer (`Mobile` & `Forward+`)** — Powered by Mesa NVK (`mesa-switch`) with working persistent `shader_cache` on microSD.
- ✅ **OpenGL ES 3 Renderer (`Compatibility`)** — Works out of the box.
- ✅ **Audio (`audren`)** — Background music and sound effects play properly.
- ✅ **Joypad / Controller (`hid`)** — Joy-Con and Pro Controller inputs work natively.
- ✅ **Touchscreen** — Handheld touch input is supported.
- 🧪 **Virtual Keyboard (`swkbd`)** — Implemented in codebase, but **untested** yet.
- 🧪 **Built-in Third-Party Modules (`mbedtls`, networking, etc.)** — Included in the build, but not all tested yet.

## Known Quirks & Limitations

- **Slow First Boot (~20s black screen):** The first launch takes around 20 seconds on a black screen while shaders compile and assets load from the microSD card.
- **No GDExtension Support:** Dynamic libraries (`dlopen`) are stubbed out on Switch, so only pure GDScript projects are supported for now.
- **No C# / .NET:** Built without Mono/C# support — GDScript only.
- **Full RAM Mode Required:** Launching from the Album menu (Applet Mode) is blocked due to low memory.
- **Manual Export:** No one-click editor exporter addon yet—you need to bundle your `.pck` with the `.nro` template manually.

## Credits & Acknowledgements

This project wouldn't be possible without the amazing work from the Nintendo Switch homebrew and open-source graphics communities:

- **[Godot Engine](https://godotengine.org)** — For the awesome open-source game engine.
- **[devkitPro](https://devkitpro.org)** — For `devkitA64`, `libnx`, and the Switch homebrew toolchain.
- **[Stary2001](https://github.com/Stary2001/godot.git)** — For the foundational `libnx` platform implementation for Godot 3.5 port.
- **[danfromtico](https://github.com/danfromtico/mesa-switch.git)** — For porting Mesa NVK (mesa-switch), this making all Godot rendering type possible.
- **[DemoxDev](https://github.com/DemoxDev/godot-switch-template.git)** — For helpful documentation and references regarding POSIX shims and adapting Switch platform code.
