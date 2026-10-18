# Silent Hill Quick Weapons

**v1.0.0 — first public release | Windows | Silent Hill 1 Native PC Port**

Quickly change Harry's weapons without opening the inventory. Select a specific weapon with the number keys, or scroll in either direction to cycle through weapons you actually own.

**Unofficial community plugin by [papanicolaus](https://github.com/papanicolaus).** This project is not affiliated with Konami or the maintainers of the PC Port.

## Features

- **1–0**: equip a specific weapon instantly, if present in Harry's inventory.
- **Mouse wheel UP/DOWN**: cycle backward/forward through owned weapons, wrapping at the ends.
- Unowned weapons are ignored automatically.
- Existing game controls stay intact, including the built-in forward-only cycling shortcut.
- Input is safely ignored during menus, cutscenes, and gameplay states where swapping is unsafe.
- Does not patch the original PS1 game image or replace `SilentHillPC.exe`.

## Controls

| Key | Weapon | Key | Weapon |
| --- | --- | --- | --- |
| `1` | Kitchen Knife | `2` | Steel Pipe |
| `3` | Handgun | `4` | Shotgun |
| `5` | Hunting Rifle | `6` | Hammer |
| `7` | Axe | `8` | Chainsaw |
| `9` | Katana | `0` | Rock Drill |
| `-` | Hyper Blaster | `Wheel up/down` | Previous/next owned weapon |

**Known limitation:** Key assignments are currently fixed. Configurable bindings may be explored in a future update.

## Compatibility

**Tested successfully on:**
- Windows version of [Silent Hill 1 Native PC Port](https://github.com/SlickAmogus/silent-hill-decomp), engine build `0b7bfd11d` (October 12, 2026).
- Europe/PAL disc data (`SLES-01514`).
- Gameplay, inventory/menu behavior, cutscenes, new game, loading saves, and weapon switching without ammunition (manual play-testing).

Other PC Port builds, disc regions, operating systems, and input configurations **have not yet been validated**. The plugin is **not** for PS1 emulators or other Silent Hill titles.

## Download

Download the ZIP asset from the repository's **Releases** page once the release is published. The ZIP contains the compiled plugin, instructions, metadata, and GPL license. **Do not download the GitHub “Source code (zip)” expecting a DLL**; it contains source code only.

## Install

1. Close the game. Back up the game folder, including your saves and configuration.
2. Extract `plugins/quick_weapons.dll` from the release ZIP to a `plugins` folder next to `SilentHillPC.exe`. Create that folder if needed.
3. Open `config.cfg` (in the same folder as `SilentHillPC.exe`) with Notepad. **Scroll to the end of the file and paste `enable_plugins = 1` on its own new line**, then save. This setting may not exist in the default configuration. If a line beginning with `enable_plugins` already exists, change its value to `1` instead of adding a duplicate.
4. Start Silent Hill and test in a save with multiple owned weapons.


### Uninstall

Exit the game, delete `plugins/quick_weapons.dll`, and, if you are not using any other plugin, optionally set `enable_plugins = 0` in `config.cfg`. No save conversion is required.

## Technical details

- Written in C as a native DLL plugin using the PC Port's `SH_Plugin_*` lifecycle hooks.
- Uses SDL2 public APIs for keyboard and wheel input.
- Built against the original port's source headers from commit `0b7bfd11d`.
- The engine-side weapon equip routine is adapted from the GPL-licensed source in `pc_port/src/pc_combat.c`; see `src/quick_weapons.c`.
- The binary in v1.0.0 is the same tested DLL from the successful GitHub Actions build #4, without gameplay changes since those tests.

## Feedback and credits

Found a bug or another build that works? Please open an Issue and include the PC Port build hash (at the top of `SilentHill_*.log`), steps to reproduce and which control style you use.

Project and mod: **papanicolaus**. Developed with AI assistance and tested in-game by the author. Credit to the [Silent Hill Native PC Port](https://github.com/SlickAmogus/silent-hill-decomp) contributors for the underlying game port and plugin interface.

## License and disclaimer

Source released under **GPL-3.0-or-later** (see [LICENSE](LICENSE)). Native plugins run executable code: install only files from sources you trust. Distributed **as-is, without warranty**. No original Konami ROMs or proprietary game assets are included.
