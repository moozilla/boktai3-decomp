#include "scene/terrain/terrain_sampling.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

#include "bosses/hresvelgr/boss_action_state.h"
#include "engine/engine.h"
#include "bosses/vanargand/vanargand_parts.h"
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void Actor_SetHeadingVelocity(void *, u32);
void Hresvelgr_SpawnProjectile(void *, u32, u32);
static inline s32 FacingSector4(s16 angle)
{
    return (angle + 96) >> 6;
}
static inline s32 FacingSector8(s16 angle)
{
    return (angle + 112) >> 5;
}
static inline void SetHeading(struct HresvelgrTravelActor *p, s32 heading)
{
    p->facing = heading;
    {
        s32 sector = FacingSector4(p->facing);
        u32 mask = 3;
        mask &= sector;
        p->facing4 = mask;
    }
    {
        s32 sector = FacingSector8(p->facing);
        u32 mask = 7;
        mask &= sector;
        p->facing8 = mask;
    }
}
static inline s32 RandomValue(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)((u32)table + offset);
}
static inline void SetActorPosition(struct SceneVec *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}
static inline u32 *EnableFlags(struct HresvelgrTravelActor *actor, u32 mask)
{
    u32 *p = &actor->flags;
    *p |= mask;
    return p;
}
static inline void SetPaletteBank(struct HresvelgrTravelActor *actor, u32 mask)
{
    actor->animation.paletteBank = mask;
}
// confidence=low: traverses a boundary and spawns falling projectiles over valid terrain.
void Hresvelgr_UpdateBoundaryBarrage(struct HresvelgrTravelActor *actor, u32 tick)
{
    u32 normalPaletteBank = 0x2f0;
    struct BossActionState *action = &actor->action;
    struct SceneVec *target = actor->origin;
    u32 *flags;
    s32 state;
    if (actor->changed) {
        actor->changed = 0;
        ActionState_Reset(action);
        ActionState_SetState(action, 0);
    }
    flags = EnableFlags(actor, 12);
    state = action->state;
    switch (state) {
        case 0:
            *flags |= 16;
            if (action->frame > 128) {
                if (!actor->side)
                    SetActorPosition(&actor->position, 1024, 512, target->z);
                else
                    SetActorPosition(&actor->position, target->x, 512, 1024);
                ActionState_AdvanceState(action);
            } else
                action->frame++;
            break;
        case 1:
            *flags |= 0x1010000;
            if (!action->frame) {
                action->frame = state;
                SetHeading(actor, actor->targetHeading);
                Hresvelgr_SetAnimation(actor, &actor->animation, &actor->animationData, 2, state, 0);
                SetPaletteBank(actor, 0xd000);
                actor->animation.palette = actor->flashPalette[0];
                Sound_PlaySong(0x4d0);
            } else {
                Actor_SetHeadingVelocity(actor, 32);
                state &= tick;
                if (!state) {
                    u32 mask = 16;
                    *flags |= mask;
                }
                if (!(tick & 7)) {
                    struct ContactPosition pos;
                    const u16 *table = gRandomTable;
                    s32 rawX = RandomValue(table);
                    s32 px = actor->position.x;
                    // Not `% 512`: as the whole right-hand side, expand_divmod would
                    // compute the quotient in x itself. Here the division is a
                    // subexpression, so the quotient gets its own register, as in the ROM.
                    s32 x = (rawX >> 3) - ((rawX >> 3) / 512) * 512;
                    s32 rawZ = RandomValue(table);
                    s32 pz = actor->position.z;
                    u32 height;
                    Vanargand_SetVector(&pos, px + x - 256, 0x2000, pz + ((rawZ >> 3) % 512) - 256);
                    height = Terrain_SampleHeight(&pos) - 256;
                    if (height <= 0x6ff)
                        Hresvelgr_SpawnProjectile(&pos, 0, 1);
                }
                {
                    u32 direction =
                        (actor->facing4 >> 1) | (normalPaletteBank & (actor->facing4 >> 8));
                    if (direction == 0 && actor->position.x > 0x1400)
                        ActionState_AdvanceState(action);
                    else if (direction == 1 && actor->position.z > 0x1400)
                        ActionState_AdvanceState(action);
                }
            }
            break;
        case 2:
            actor->animation.paletteBank = normalPaletteBank;
            actor->animation.palette = actor->normalPalette[0];
            {
                u32 mask = 16;
                *flags |= mask;
            }
            actor->finished = 1;
            break;
    }
}
