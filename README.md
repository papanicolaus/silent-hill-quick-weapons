# Silent Hill Quick Weapons — v0.1 source prototype

**STATUS: SOURCE CODE PROTOTYPE. NOT YET COMPILED OR TESTED ON WINDOWS.**
Do **not** drop this source ZIP into the Mod Manager: there is no compiled DLL inside it.

Target: Silent Hill Native PC Port (SlickAmogus/silent-hill-decomp),
Windows build `0b7bfd11d` of 2026-10-12 00:13:15 (user's actual log).
Tested game disc in log: Europe/PAL (SLES-01514). This is a PC-port plugin,
not a patch to the original PS1 disc image.

## Requested behavior

- Number hotkeys equip a particular weapon immediately, if Harry owns it.
- Mouse wheel UP equips the previous owned weapon; DOWN equips the next.
- If a weapon is absent, the direct hotkey does nothing. The wheel skips it.
- Existing `Left Ctrl` native one-way cycle stays untouched.
- Safe game-state guard: menus, cutscenes, attacks, aiming and reloading do not
  activate weapon swaps; no delayed swaps are queued when control returns.
- Multiple wheel notches landing in one frame resolve to one final weapon swap.
- No save file or original game content is overwritten.

### Default key layout (v0.1)

| Key | Weapon |
|---|---|
| 1 | Kitchen Knife |
| 2 | Steel Pipe |
| 3 | Handgun |
| 4 | Shotgun |
| 5 | Hunting Rifle |
| 6 | Hammer |
| 7 | Axe |
| 8 | Chainsaw |
| 9 | Katana |
| 0 | Rock Drill |
| - | Hyper Blaster |

**Key remapping is planned but is NOT yet implemented.**

## Building against the port

Build this plugin from the original port source, with the same compiler setup
used to build the game. This prototype links against the PC executable's
exported symbols; prebuilt DLL compatibility with other engine commits is not
established.

1. Check out engine commit `0b7bfd11d` from the official project.
2. Put this folder anywhere outside the game's installed binaries.
3. In the engine's `pc_port/CMakeLists.txt`, after the `SilentHillPC` target is
   declared, append `add_subdirectory("ABSOLUTE_PATH/TO/quick_weapons_v0_1" "${CMAKE_BINARY_DIR}/quick_weapons_mod")`.
4. Configure/build the port with the same MinGW/CMake/SDL2/PsyCross dependencies
   the project itself documents, then build the `quick_weapons` target.
5. The expected DLL output is `quick_weapons_release/plugins/quick_weapons.dll`
   in the CMake build directory. (Not yet validated with a Windows build.)

After the binary is successfully compiled and tested, distribute a ZIP with
only `plugins/quick_weapons.dll`, `mod.json`, `README.md` and the source link.
The port Mod Manager can identify `plugins/*.dll` under a gameplay mod ZIP.
The game requires `enable_plugins = 1` in `config.cfg` to load gameplay plugins;
make a backup before editing. Plugin DLLs execute native code: install only
from trusted, reviewable sources. The port performs a static import/export
screen, but that is not a security sandbox.

## Validation checklist BEFORE a public release

1. Compile DLL for 64-bit Windows against build `0b7bfd11d`.
2. Confirm DLL passes the loader's plugin security audit and the Mod Manager
   deploys precisely one file under `plugins/`.
3. Verify keyboard actions in classic camera AND Over-the-Shoulder; verify Q
   and Left Ctrl remain untouched.
4. Verify 1-0 and '-' for owned/unowned weapons and no remapped game controls.
5. Test scroll in each direction, fast-wheel bursts, wrap-around, with 1/2/11
   weapons, and scroll navigation in menus/puzzles.
6. Check equipping and unequipping chainsaw/rock drill, firearm ammo values,
   animation loading, and absence of visual / audio glitches.
7. Test cutscenes, map, inventory, pause, console, reload, aiming, game-over,
   quick load, and changing control style; verify no delayed swaps.
8. Test uninstall/reinstall using the Mod Manager; check game config and saves.
9. Check licensing and update compatibility before public publishing.

## Where the gameplay change is implemented

- `src/quick_weapons.c` adapts the engine's `Pc_EquipWeapon` behavior because
  `Pc_EquipWeapon` in `pc_port/src/pc_combat.c` is declared `static` and cannot
  be called directly from a plugin.
- `g_PsyX_WheelTotal` is used for both wheel directions, even when the game
  captures the mouse in OTS/FPS mode.
- Keyboard input uses the engine's SDL keyboard state rather than an external
  hotkey process.

Original project: https://github.com/SlickAmogus/silent-hill-decomp

License: GPL-3.0-or-later; see `LICENSE`.
