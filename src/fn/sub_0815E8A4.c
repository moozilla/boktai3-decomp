#include "global.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: consumes pending damage and selects terrain/death reactions.
#include "engine.h"
#include "player.h"
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
s32 Player_BeginTerrainRecovery(struct Player *);
s32 Player_BeginTerrainFacingAction(struct Player *);
s32 Player_BeginTerrainRecovery_081408AC(struct Player *);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
s32 Inventory_HasItem(s32);
void Inventory_RemoveItem(s32);
void HealthRestoreEffect_Create(struct Player *);
#define H(off) (*(u16 *)((u8 *)p + (off)))
static inline u8 sceneFlag(void)
{
    u32 mask = 0x4000;
    if (gGameState->flags868 & mask)
        return 1;
    return 0;
}
static inline u32 specialState(struct Player *p)
{
    u32 active = p->active;
    u32 result = 0;
    if (active && p->sceneMode == 3)
        result = 1;
    return result;
}
static inline u32 playerFlag(struct Player *p, u32 offset, u32 mask)
{
    if (!p)
        return 0;
    {
        u32 *field = (u32 *)((u8 *)p + offset);
        u32 bits = mask;
        return *field & bits;
    }
}
s32 PlayerContact_ApplyTerrainDamage(struct Player *p)
{
    s32 cell, skip;
    s32 sequence;
    register struct Player *hazardPlayer asm("r0");
    u8 *properties;
    {
        s32 x = p->position.x >> 8, z = p->position.z >> 8;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            cell = 0;
        else
            cell = gTerrainMap->rows[z] + x;
    }
    properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties)
        properties += 4;
    else
        properties = gTerrainMap->cells + 12 + cell * 4;
    if ((*(u32 *)properties >> 16 & 0x40) && !p->tetherStatusFlags) {
        if (sceneFlag()) {
            hazardPlayer = p;
            sequence = 0x22;
        } else {
            hazardPlayer = p;
            sequence = 0x21;
        }
        goto hazard;
    }
    if (gGameState->playerParticleRate > 4999) {
        hazardPlayer = p;
        sequence = 0x23;
    hazard:
        ActorSequence_Begin(hazardPlayer, sequence, 0);
        return 1;
    }
    skip = 0;
    if (p->terrainClampApplied) {
        p->particleEmissionCount += Player_BeginTerrainRecovery(p);
        if (playerFlag(p, 0x448, 2))
            skip = 1;
    } else if (p->actionPhase488)
        p->particleEmissionCount += Player_BeginTerrainFacingAction(p);
    else if (p->terrainRecoveryPhase)
        p->particleEmissionCount += Player_BeginTerrainRecovery_081408AC(p);
    if (!p->positionResetValue) {
        if (p->movementModifierKind == 1 && p->movementModifierActive) {
            {
                u32 bits = 0x200;
                p->statusFlags |= bits;
            }
            if (++H(0x4C6) >= Div(60, p->movementModifierActive)) {
                p->particleEmissionCount += 10;
                H(0x4C6) = 0;
            }
        }
        if (playerFlag(p, 0x444, 0x08000000)) {
            u16 *timer = (u16 *)((u8 *)p + 0x4CE);
            if (++*timer > 14) {
                u16 *pending = &p->particleEmissionCount;
                u32 zero;
                ++*pending;
                zero = 0;
                *timer = zero;
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
        if (gEngineFlags & 0x1000)
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
