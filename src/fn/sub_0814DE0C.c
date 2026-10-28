// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"
// foundry draft (worker best 97.4%); what differs is mostly register allocation and instruction order
u8 *Player_GetSpecialAttackDescriptor(s32 idx);
s32 Player_GetAttackElementType(u32 x);
void Projectile_InitializeParams(u8 *);
void Projectile_SetFlightParams(u8 *, u16, u16, u8, u32);
void Projectile_SetCollisionSize(u8 *, u16, u16, u16);
void Projectile_SetTypeAndElement(u8 *, u32, u32);
void Projectile_SetVariant(u8 *, u16);

static inline u32 DisplayFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
void Player_ConfigureActionSelectionDisplay(struct Player *p)
{
    u32 selection;
    Player_GetSpecialAttackDescriptor(p->statModifierContext.actionKind);
    Projectile_InitializeParams(p->statModifierContext.actionSelectionDisplay);
    if (!p->statModifierContext.statBOverrideEnabled) {
        if (p->actionMode == 6)
            goto mode6;
        if (p) {
            u32 flags;
            {
                u32 off;
                u32 *fp;
                asm("" ::: "r1");
                off = 0x444;
                fp = (u32 *)((u8 *)p + off);
                flags = DisplayFlags(fp, 0x10000000);
            }
            selection = 7;
            if (flags)
                goto selected;
        }
        selection = 1;
    selected:
        Projectile_SetVariant(p->statModifierContext.actionSelectionDisplay, 1);
    } else {
        if (p->actionMode == 6) {
        mode6:
            Projectile_SetVariant(p->statModifierContext.actionSelectionDisplay, 1);
            selection = 1;
        } else {
            selection = p->statModifierContext.actionKind - 1;
            Projectile_SetVariant(p->statModifierContext.actionSelectionDisplay, 0);
        }
    }
    if (!p->statModifierContext.statBOverrideEnabled)
        Projectile_SetTypeAndElement(p->statModifierContext.actionSelectionDisplay, selection, 0);
    else
        Projectile_SetTypeAndElement(
            p->statModifierContext.actionSelectionDisplay, selection,
            Player_GetAttackElementType(p->statModifierContext.statBOverrideMode));
    {
        u8 value = ((u8 *)gPlayers[0])[0x2D];
        Projectile_SetFlightParams(p->statModifierContext.actionSelectionDisplay, 0, 60, value, 24);
    }
    Projectile_SetCollisionSize(p->statModifierContext.actionSelectionDisplay, 16, 100, 16);
}
