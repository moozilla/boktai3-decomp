#include "global.h"
#ifdef NONMATCHING
// fuzzy: 89.4%
// Remaining differences: register allocation and instruction/block ordering.
#include "global.h"
#include "engine.h"
#include "player.h"
#define PlayerContact_ApplyTerrainDamage sub_0815E8A4
// confidence=low: consumes pending damage and selects terrain/death reactions.
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
s32 Player_BeginTerrainRecovery(struct Player *);
s32 sub_08140748(struct Player *);
s32 Player_BeginTerrainRecovery_081408AC(struct Player *);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
s32 Inventory_HasItem(s32);
void Inventory_RemoveItem(s32);
void HealthRestoreEffect_Create(struct Player *);
#define H(off) (*(u16 *)((u8 *)p + (off)))
static inline u8 sceneFlag(void)
{
    u32 mask = 0x4000;
    if (gUnk_02000710->flags868 & mask)
        return 1;
    return 0;
}
static inline u32 specialState(struct Player *p)
{
    u32 result = 0;
    if (p->active && p->sceneMode == 3)
        result = 1;
    return result;
}
s32 PlayerContact_ApplyTerrainDamage(struct Player *p)
{
    s32 cell, skip;
    u8 *properties;
    {
        s32 x = p->position.x >> 8, z = p->position.z >> 8;
        if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
            cell = 0;
        else
            cell = gUnk_030052F4->rows[z] + x;
    }
    properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties)
        properties += 4;
    else
        properties = gUnk_030052F4->cells + 12 + cell * 4;
    if ((*(u16 *)(properties + 2) & 0x40) && !p->tetherStatusFlags) {
        if (sceneFlag())
            ActorSequence_Begin(p, 0x22, 0);
        else
            ActorSequence_Begin(p, 0x21, 0);
        return 1;
    }
    if (gUnk_02000710->playerParticleRate > 4999) {
        ActorSequence_Begin(p, 0x23, 0);
        return 1;
    }
    skip = 0;
    if (p->terrainClampApplied) {
        p->particleEmissionCount += Player_BeginTerrainRecovery(p);
        if (p && (p->actionFlags & 2))
            skip = 1;
    } else if (p->actionPhase488)
        p->particleEmissionCount += sub_08140748(p);
    else if (p->terrainRecoveryPhase)
        p->particleEmissionCount += Player_BeginTerrainRecovery_081408AC(p);
    if (!p->positionResetValue) {
        if (p->movementModifierKind == 1 && p->movementModifierActive) {
            p->statusFlags |= 0x200;
            if (++H(0x4C6) >= Div(60, p->movementModifierActive)) {
                p->particleEmissionCount += 10;
                H(0x4C6) = 0;
            }
        }
        if (p && (p->statBonusFlags & 0x08000000)) {
            if (++H(0x4CE) > 14) {
                p->particleEmissionCount++;
                H(0x4CE) = 0;
            }
        }
    }
    if (!skip && p->particleEmissionCount && !(p && specialState(p))) {
        DamageNumberDisplay_Show(&p->collider, p->particleEmissionCount, 0, &p->position,
                                 &p->movementRequestTarget, 0);
        if (p->currentStatA < p->particleEmissionCount)
            p->currentStatA = 0;
        else
            p->currentStatA -= p->particleEmissionCount;
    }
    p->particleEmissionCount = 0;
    p->movementRequestTarget.x = 0;
    p->movementRequestTarget.y = 0;
    p->movementRequestTarget.z = 0;
    if (p->tetherMode != 0x18 && !p->currentStatA) {
        if (gUnk_030053F4 & 0x1000)
            return 1;
        if (!Inventory_HasItem(0x26))
            return 1;
        p->contactActionTag = 0;
        if (!H(0x270)) {
            p->facing |= 1;
            H(0x270) = 1;
        }
        Inventory_RemoveItem(0x26);
        HealthRestoreEffect_Create(p);
    }
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0815E8A4);
#endif
