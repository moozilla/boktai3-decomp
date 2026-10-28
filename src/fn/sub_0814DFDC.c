#include "global.h"
#include "player.h"
#include "ram.h"
// confidence=low: descriptor meanings remain tentative; creates the selected attack display projectile.
extern u8 *gGameState;
s32 Player_GetSelectedSlotActionScale(void);
s32 ActorResponse_Apply(struct Player *, s32);
u8 *Player_GetSpecialAttackDescriptor(s32);
s32 Player_IsStatBBelow(struct Player *, s32);
void Player_ConfigureActionSelectionDisplay(struct Player *);
void PlayerContact_GetOffsetPosition(s32, struct SceneVec *);
s32 Task4E01_GetKind7Value(void);
u32 Player_GetAttackAttributeFlags(u32);
void Projectile_SetDamageParams(void *, s32, u32, s32, s32, s32);
s32 Projectile_CreateForOwner(void *, struct SceneVec *, s32);
s32 Projectile_Spawn(void *, struct SceneVec *);
void Player_SubtractStatB(struct Player *, s32);
void Sound_PlaySong(u32);
s32 Div(s32, s32);
static inline u32 testFlags(u32 *f, u32 mask)
{
    return *f & mask;
}
static inline s32 truncate4(s32 n)
{
    s32 result;
    if (n >= 0)
        result = n >> 4;
    else
        result = -((-n) >> 4);
    return result;
}
s32 Player_SpawnSelectedAttackProjectile(struct Player *p)
{
    s32 cost;
    s32 special;
    s32 power;
    s32 size;
    u8 *descriptor;
    struct SceneVec position;
    if (p->actionMode == 6) {
        p->statModifierContext.statBOverrideMode = 8;
        p->statModifierContext.actionKind = 2;
    }
    if (p->statModifierContext.statBOverrideMode == 6 &&
        p->statModifierContext.statBOverrideEnabled)
        return 0;
    if (gEngineFlags & 0x1000)
        cost = Player_GetSelectedSlotActionScale();
    else {
        if (!p->statModifierContext.statBOverrideEnabled)
            cost = 3;
        else if (p->statModifierContext.statBOverrideMode == 1)
            cost = 0;
        else
            cost = Player_GetSpecialAttackDescriptor(p->statModifierContext.actionKind)[2];
        cost = ActorResponse_Apply(p, cost);
    }
    if (Player_IsStatBBelow(p, cost) && !(gEngineFlags & 0x1000)) {
        if (p->statModifierContext.statBOverrideEnabled && p->actionMode != 6)
            Sound_PlaySong(0xD5);
        return 0;
    }
    Player_ConfigureActionSelectionDisplay(p);
    PlayerContact_GetOffsetPosition(0, &position);
    {
        s32 stat = p->currentStatB;
        special = 0;
        if (gEngineFlags & 0x1000) {
            if ((u8)(p->thresholdMode - 6) <= 5 && stat >= cost)
                special = 1;
        } else {
            if ((!p->statModifierContext.statBOverrideEnabled && p &&
                 testFlags(&p->statBonusFlags, 0x10000000)) ||
                Player_GetSpecialAttackDescriptor(p->statModifierContext.actionKind)[0] == 8) {
                if (p->statModifierContext.attachedEntry < 0)
                    cost = 0;
                else {
                    s32 count = Task4E01_GetKind7Value();
                    if (count > 0) {
                        s32 prior = cost - 1;
                        cost = prior + count;
                    }
                }
            }
        }
    }
    {
        s32 kind;
        if (!p->statModifierContext.statBOverrideEnabled)
            kind = 8;
        else
            kind = p->statModifierContext.actionKind;
        descriptor = Player_GetSpecialAttackDescriptor(kind);
    }
    size = 16;
    switch ((s32)descriptor[3]) {
        case 0:
        default:
            break;
        case 1:
            size = 48;
            break;
        case 2:
            size = 80;
            break;
        case 3:
            size = 144;
            break;
    }
    if (gEngineFlags & 0x1000) {
        switch (p->statModifierContext.actionKind) {
            case 1:
                power = 30;
                goto powerReady;
            case 5:
            case 7:
                power = 25;
                goto powerReady;
        }
    }
    power = *(s16 *)(gGameState + 0x40) + descriptor[1];
powerReady:
    if (special && (gEngineFlags & 0x1000)) {
        u32 flags =
            Player_GetAttackAttributeFlags(p->statModifierContext.statBOverrideMode) | 0x800000;
        Projectile_SetDamageParams(p->statModifierContext.actionSelectionDisplay, size, flags,
                                   power, 50, 40);
    } else if (!p->statModifierContext.statBOverrideEnabled) {
        s32 base = power + Div(power, 10);
        s32 half = Div(power, 2);
        s32 ratio;
        s32 maximum = p->maximumStatA;
        ratio = Div((maximum - p->currentStatA) << 4, maximum);
        base -= truncate4(ratio * half);
        Projectile_SetDamageParams(p->statModifierContext.actionSelectionDisplay, size, 1, base, 50,
                                   20);
    } else if (p->statModifierContext.actionKind == 2) {
        u32 flags = Player_GetAttackAttributeFlags(p->statModifierContext.statBOverrideMode);
        Projectile_SetDamageParams(p->statModifierContext.actionSelectionDisplay, size, flags,
                                   power, 50, 6);
    } else {
        u32 flags = Player_GetAttackAttributeFlags(p->statModifierContext.statBOverrideMode);
        Projectile_SetDamageParams(p->statModifierContext.actionSelectionDisplay, size, flags,
                                   power, 50, 20);
    }
    if (((!p->statModifierContext.statBOverrideEnabled && p &&
          testFlags(&p->statBonusFlags, 0x10000000)) ||
         p->statModifierContext.actionKind == 8) &&
        cost > 0)
        p->statModifierContext.attachedEntry = -1;
    else {
        if (p->actionMode == 6)
            p->statModifierContext.attachedEntry = Projectile_CreateForOwner(
                p->statModifierContext.actionSelectionDisplay, &position, 1);
        else
            p->statModifierContext.attachedEntry =
                Projectile_Spawn(p->statModifierContext.actionSelectionDisplay, &position);
        if (p->statModifierContext.attachedEntry < 0)
            return 0;
    }
    Player_SubtractStatB(p, cost);
    return 1;
}
