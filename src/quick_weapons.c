/* SPDX-License-Identifier: GPL-3.0-or-later
 * Silent Hill Quick Weapons - community prototype v0.1
 * Target: Silent Hill Native PC Port, build 0b7bfd11d (2026-10-12).
 *
 * Not a standalone DLL: compile as a plugin with the port's source headers.
 * The engine-facing equip routine below is adapted from the port's
 * pc_port/src/pc_combat.c (GPL-3.0-or-later).
 */
#include <SDL2/SDL.h>
#include "game.h"
#include "bodyprog/bodyprog.h"
#include "bodyprog/player.h"
#include "bodyprog/chara/chara.h"
#include "bodyprog/item_screens.h"
#include "bodyprog/sound/sound_system.h"

#ifdef _WIN32
#define QW_EXPORT __declspec(dllexport)
#else
#define QW_EXPORT __attribute__((visibility("default")))
#endif

/* Use SDL's public input API instead of PsyCross private C++ symbols.
 * Private C++ globals are not necessarily exported under their C names. */
extern int Pc_ConsoleIsOpen(void);
extern void GameFs_WeaponInfoUpdate(void);

/* Numeric hotkeys map to ONE specific item. `-` = secret HyperBlaster.
 * 1 knife, 2 pipe, 3 handgun, 4 shotgun, 5 rifle,
 * 6 hammer, 7 axe, 8 chainsaw, 9 katana, 0 drill, - hyperblaster. */
typedef struct QwBind { SDL_Scancode scancode; u8 item; } QwBind;
static const QwBind kBinds[] = {
    { SDL_SCANCODE_1, InvItemId_KitchenKnife },
    { SDL_SCANCODE_2, InvItemId_SteelPipe },
    { SDL_SCANCODE_3, InvItemId_Handgun },
    { SDL_SCANCODE_4, InvItemId_Shotgun },
    { SDL_SCANCODE_5, InvItemId_HuntingRifle },
    { SDL_SCANCODE_6, InvItemId_Hammer },
    { SDL_SCANCODE_7, InvItemId_Axe },
    { SDL_SCANCODE_8, InvItemId_Chainsaw },
    { SDL_SCANCODE_9, InvItemId_Katana },
    { SDL_SCANCODE_0, InvItemId_RockDrill },
    { SDL_SCANCODE_MINUS, InvItemId_HyperBlaster },
};
#define QW_NBINDS ((int)(sizeof(kBinds) / sizeof(kBinds[0])))

/* The scroll order intentionally matches numeric hotkeys, not the game's
 * inventory enumeration. It includes only possessed items at runtime. */
static const u8 kWeaponOrder[] = {
    InvItemId_KitchenKnife,
    InvItemId_SteelPipe,
    InvItemId_Handgun,
    InvItemId_Shotgun,
    InvItemId_HuntingRifle,
    InvItemId_Hammer,
    InvItemId_Axe,
    InvItemId_Chainsaw,
    InvItemId_Katana,
    InvItemId_RockDrill,
    InvItemId_HyperBlaster,
};
#define QW_NWEAPONS ((int)(sizeof(kWeaponOrder) / sizeof(kWeaponOrder[0])))

static unsigned char s_prevKeys[QW_NBINDS];
static SDL_atomic_t s_wheelAccumulator = {0};
static int s_prevWheelTotal;

/* SDL invokes event watchers when input events enter its queue.
 * Accumulate wheel movement even when the port grabs/hides its cursor. */
static int SDLCALL Qw_WheelEvent(void *userdata, SDL_Event *event)
{
    int delta;
    (void)userdata;
    if (event->type != SDL_MOUSEWHEEL) return 1;
    delta = event->wheel.y;
    if (event->wheel.direction == SDL_MOUSEWHEEL_FLIPPED) delta = -delta;
    if (delta != 0) SDL_AtomicAdd(&s_wheelAccumulator, delta);
    return 1;
}

static s32 Qw_FindOwnedSlot(u8 id)
{
    s32 i;
    if (!g_SavegamePtr) return NO_VALUE;
    /* Never inspect dead inventory slots: they may contain stale items. */
    for (i = 0;
         i < g_SavegamePtr->inventorySlotCount && i < INV_ITEM_COUNT_MAX;
         ++i)
        if (g_SavegamePtr->items[i].id_0 == id) return i;
    return NO_VALUE;
}

/* Same safety gate as Pc_ActionSafe() in the port's pc_combat.c.
 * Do NOT load weapon graphics while aiming, reloading or swinging. */
static int Qw_SafeToEquip(void)
{
    return g_GameWork.gameState == GameState_InGame &&
           g_SysWork.sysState == SysState_Gameplay &&
           !g_Player_DisableControl &&
           g_SysWork.playerWork.extra.upperBodyState <=
               PlayerUpperBodyState_SidestepRightStumble &&
           !Pc_ConsoleIsOpen();
}

/* Mirror the engine's existing direct-equip implementation. DO NOT merely
 * change the savegame's equippedWeapon: model, attack and ammo state must
 * all be updated in sync. */
static void Qw_EquipOwnedWeapon(u8 invItemId, s32 slot)
{
    s32 groupId = INV_ITEM_GROUP(invItemId);
    if (g_SavegamePtr->equippedWeapon == invItemId) return;

    /* Stop sound of previous weapon, e.g. a running chainsaw. */
    func_8004C564(0, NO_VALUE);

    g_Inventory_EquippedItem = invItemId;
    g_SavegamePtr->equippedWeapon = invItemId;
    g_SysWork.playerCombat.weaponAttack = invItemId + InvItemId_KitchenKnife;
    g_SysWork.playerCombat.weaponInventoryIdx = slot;
    g_SysWork.playerCombat.currentWeaponAmmo = g_SavegamePtr->items[slot].count_1;

    if (groupId == InvItemGroup_GunWeapons) {
        s32 ammoSlot = Qw_FindOwnedSlot(invItemId + InvItemId_HealthDrink);
        g_SysWork.playerCombat.totalWeaponAmmo =
            (ammoSlot == NO_VALUE) ? 0 : (s8)g_SavegamePtr->items[ammoSlot].count_1;
    } else {
        g_SysWork.playerCombat.totalWeaponAmmo = 0;
    }

    GameFs_WeaponInfoUpdate();
    Gfx_PlayerHeldItemAttach(g_SysWork.playerCombat.weaponAttack);
    WorldGfx_PlayerPrevHeldItem(&g_SysWork.playerCombat);
    func_8003D01C();

    /* Match the inventory-exit cleanup for petrol-powered tools. */
    {
        u8 previousAttackId = WEAPON_ATTACK_ID_GET(g_Player_WeaponAttack);
        if ((previousAttackId == EquippedWeaponId_Chainsaw &&
             g_SysWork.playerCombat.weaponAttack != previousAttackId) ||
            (previousAttackId == EquippedWeaponId_RockDrill &&
             g_SysWork.playerCombat.weaponAttack !=
                 WEAPON_ATTACK(previousAttackId, AttackInputType_Tap))) {
            g_SysWork.playerWork.player.field_44.field_0 = 0;
            g_SysWork.playerWork.player.properties.player.gasWeaponPowerTimer = Q12(0.0f);
        }
    }
}

/* Return the next/previous owned weapon (or current when no alternative). */
static u8 Qw_AdjacentOwned(int direction, u8 current)
{
    int i, j, start = -1;
    for (i = 0; i < QW_NWEAPONS; ++i) {
        if (kWeaponOrder[i] == current) { start = i; break; }
    }
    /* Start outside the range when no weapon is currently equipped. */
    if (start < 0) start = direction > 0 ? -1 : 0;
    for (j = 1; j <= QW_NWEAPONS; ++j) {
        int index = (start + direction * j + QW_NWEAPONS * 2) % QW_NWEAPONS;
        if (Qw_FindOwnedSlot(kWeaponOrder[index]) != NO_VALUE)
            return kWeaponOrder[index];
    }
    return current;
}

QW_EXPORT const char *SH_Plugin_GetName(void) { return "Quick Weapons"; }
QW_EXPORT s32 SH_Plugin_GetApiVersion(void) { return 1; }

QW_EXPORT void SH_Plugin_Init(void)
{
    int i;
    for (i = 0; i < QW_NBINDS; ++i) s_prevKeys[i] = 0;
    SDL_AtomicSet(&s_wheelAccumulator, 0);
    s_prevWheelTotal = 0;
    SDL_AddEventWatch(Qw_WheelEvent, NULL);
}
QW_EXPORT void SH_Plugin_Shutdown(void)
{
    SDL_DelEventWatch(Qw_WheelEvent, NULL);
}

/* The plugin loader invokes this once per game update. No keyboard commands
 * are queued while in inventory, title, cutscenes or unsafe animations. */
QW_EXPORT void SH_Plugin_OnUpdate(void)
{
    int i;
    int wheelDelta = 0;
    int target = -1;
    const unsigned char *keyboard = SDL_GetKeyboardState(NULL);

    /* Consume input transitions even while blocked: never execute them later. */
    for (i = 0; i < QW_NBINDS; ++i) {
        int down = keyboard && keyboard[kBinds[i].scancode];
        if (down && !s_prevKeys[i]) target = kBinds[i].item;
        s_prevKeys[i] = down ? 1 : 0;
    }

    {
        int newWheelTotal = SDL_AtomicGet(&s_wheelAccumulator);
        wheelDelta = newWheelTotal - s_prevWheelTotal;
        s_prevWheelTotal = newWheelTotal;
    }

    if (!Qw_SafeToEquip()) return;
    if (!g_SavegamePtr) return;

    /* The numeric key takes priority over wheel movement in the same frame. */
    if (target < 0 && wheelDelta != 0) {
        int steps = wheelDelta > 0 ? wheelDelta : -wheelDelta;
        int direction = wheelDelta > 0 ? -1 : 1;
        u8 current = g_SavegamePtr->equippedWeapon;
        /* Limit sudden high-resolution mouse bursts to one full rotation. */
        if (steps > QW_NWEAPONS) steps = QW_NWEAPONS;
        for (i = 0; i < steps; ++i)
            current = Qw_AdjacentOwned(direction, current);
        target = current;
    }

    if (target >= 0) {
        s32 slot = Qw_FindOwnedSlot((u8)target);
        if (slot != NO_VALUE)
            Qw_EquipOwnedWeapon((u8)target, slot);
    }
}
