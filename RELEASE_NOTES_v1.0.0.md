# Silent Hill Quick Weapons v1.0.0 — First public release

An unofficial gameplay plugin for **Silent Hill 1 Native PC Port** that lets you change weapons without opening the inventory.

### Features

- Select a specific owned weapon with number keys **1–0** (or **-** for the Hyper Blaster).
- Use the **mouse wheel in either direction** to cycle through owned weapons.
- Skips unavailable weapons and preserves existing controls.
- Prevents weapon changes while menus, cutscenes or unsafe gameplay states are active.

### Tested configuration

- Windows, PC Port build **`0b7bfd11d`** (2026-10-12)
- PAL Europe game data **SLES-01514**
- Manual play-tests: general gameplay, menus, cutscenes, new game, save loading and guns without ammunition

**Other versions and regions are not yet verified.** Please report results or bugs in the Issues tab with the build hash from your game log.

### Installation

1. Download **`Silent-Hill-Quick-Weapons-v1.0.0.zip`** from this release's Assets.
2. Close the game and back up the game folder.
3. Copy `plugins/quick_weapons.dll` into the `plugins` directory alongside `SilentHillPC.exe`.
4. Open `config.cfg` next to `SilentHillPC.exe` in Notepad. Scroll to the **end of the file**, paste `enable_plugins = 1` on a **new line**, and save. This entry may be missing from the default configuration. If it already exists, change the existing value to `1` instead of adding a duplicate.
5. Launch the game and enjoy.

The Native PC Port Mod Manager also supports `plugins/*.dll` packages, although this specific ZIP was validated manually rather than through Mod Manager.

### Credits and license

By **papanicolaus**, developed with AI assistance and tested in game. Thanks to the [Silent Hill Native PC Port](https://github.com/SlickAmogus/silent-hill-decomp) contributors.

GPL-3.0-or-later. No commercial game data included. Unofficial fan modification, not affiliated with Konami.
