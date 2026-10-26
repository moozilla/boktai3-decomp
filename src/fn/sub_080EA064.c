#include "global.h"
#include "scene_actor_reaction.h"
#include "engine.h"
#include "scene_vec.h"
#include "gba/macro.h"
#include "gba/syscall.h"

struct Player;
s32 Div(s32, s32);
s32 Mod(s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Player_ApplyPositionRequest(struct Player *, struct SceneVec *, s32, u32, s32, s32, s32, u32);
s32 Player_BeginPositionRequest(struct Player *, struct SceneVec *, struct SceneVec *, s32, s32,
                                s32, s32, s32, u32);
void sub_0807D2E0(void *, u32, struct ContactPosition *, u32, u32, u32);
void SceneHud_SetTimedState(u32, s32, u32, u32, u32, u32);
void DamageNumberDisplay_Show(void *, u32, s32, void *, void *, u32);
u32 Octopus_GetAnimationStatus(struct SceneActorRegistryResetView *);
void Octopus_UpdateActiveState(void);
extern u8 *gUnk_02000580;
extern s16 *gUnk_020004B4;

struct OctopusContext {
    u8 unknown0[0x628];
    u32 flags;
    u8 unknown630[0xC];
    u8 startTick, holdTicks, interval;
};

struct OctopusTarget {
    u8 unknown0[8];
    u8 kind;
    u8 unknown9[0x33];
    void *player;
};

struct OctopusDrawable {
    u8 unknown0[7];
    u8 mode : 8;
};

struct OctopusOffset {
    u16 x, y, z, pad;
};
extern const struct OctopusOffset gUnk_086062B0[2][5];

static inline u8 TestFlags(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

static inline void CellCenter(struct SceneVec *out, u32 cell)
{
    s32 x = Mod(cell, *(s16 *)(gTerrainMap->cells + 4));
    s32 z = Div(cell, *(s16 *)(gTerrainMap->cells + 4));
    u8 *data;
    out->x = (x << 8) + 0x80;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + (cell * 4 + 12);
    out->y = (*data & 15) << 8;
    out->z = (z << 8) + 0x80;
}

static inline void SetCtl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}

static inline void ResetState(struct SceneActorRegistryResetView *actor, u32 s)
{
    actor->state2A8 = s;
    actor->phase2A9 = 0;
    actor->behavior2AA = 0;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void SetState(struct SceneActorRegistryResetView *actor, u32 s, u32 p, u32 b)
{
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void Facing(struct SceneActorRegistryResetView *actor, u8 *quadrant, u8 *flip)
{
    s32 side = ((actor->direction + 0x60) >> 6) & 3;
    if (side > 1) {
        *flip = 1;
        *quadrant = 3 - side;
    } else {
        *flip = 0;
        *quadrant = side;
    }
}

static inline s32 WrapCounter(s32 n)
{
    return n - 256;
}

static inline void FacingAngle(struct SceneActorRegistryResetView *actor, u8 *angle)
{
    s32 turn = (actor->direction + 0x20) & 0xFF;
    s32 quadrant = turn >= 0 ? turn >> 6 : -(-turn >> 6);
    *angle = quadrant << 6;
}

static inline void ClearHeld(struct OctopusContext *context)
{
    u32 *flags = &context->flags;
    u32 mask = ~2;
    *flags &= mask;
}

static inline u8 IsIdle(struct Player *player)
{
    if (*(u16 *)((u8 *)player + 0x424) == 0)
        return 1;
    return 0;
}

static inline s32 HudMode(void)
{
    if (!gUnk_020004B4)
        return -1;
    return gUnk_020004B4[0xC];
}

static inline u8 MayCount(struct SceneActorRegistryResetView *victim)
{
    if (TestFlags(gEngineFlags, 0x4800)) {
        u32 value = victim->field2E0;
        if (!(value & 0x20) && value) {
            switch (HudMode()) {
                case 0:
                    if (victim->field2E0 != 1)
                        return 0;
                    break;
                case 1:
                    if (victim->field2E0 != 2)
                        return 0;
                    break;
                case 2:
                    if (victim->field2E0 != 4)
                        return 0;
                    break;
                case 3:
                    if (victim->field2E0 != 8)
                        return 0;
                    break;
            }
        }
    }
    return 1;
}

static inline void ClearVector(void *dest)
{
    CpuFill32(0, dest, 8);
}

static inline void ReportDrain(struct SceneActorRegistryResetView *victim)
{
    struct ContactPackedVector range;
    if (MayCount(victim)) {
        s32 health = victim->health;
        u32 max = *(u16 *)((u8 *)victim + 0xB0);
        s32 kind = *(s16 *)((u8 *)victim + 0xB2);
        s32 parameter = *(s16 *)((u8 *)victim + 0x9A);
        SceneHud_SetTimedState(victim->id, health, max, kind, 0, parameter);
        ClearVector(&range);
        if (victim->reactionSoundVariant || !(victim->field2D8 & 0x20000))
            DamageNumberDisplay_Show(&victim->colliderA, victim->reactionSoundVariant, 0,
                                     &victim->position, &range, 1);
    }
}

void Octopus_UpdateHold(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct OctopusContext *context = actor->context;
    void *group = (u8 *)actor + 0x3D4;
    struct DisplaySequenceState *sequence = (void *)((u8 *)actor->drawable + 0x48);
    struct OctopusTarget *target;
    u8 quadrant, flip;
    struct ContactPosition position;
    s32 index;
    if (SceneActorReaction_TakePrepared(actor)) {
        context->startTick = gUnk_02000580[0x4F4];
        {
            u16 *destination = &actor->colliderA.flags;
            u32 flags = *destination;
            u32 mask = 4;
            mask |= flags;
            mask &= 0xFFF7;
            *destination = mask;
        }
        {
            u32 mask = 0x800;
            mask |= actor->flags2C0;
            actor->flags2C0 = mask;
        }
    }
    target = (void *)actor->fieldDC;
    Facing(actor, &quadrant, &flip);
    if (target->kind <= 3) {
        if (sequence->elapsed + 1 >= sequence->duration) {
            index = sequence->index + 1;
            if (index >= sequence->frameCount)
                index = 0;
        } else
            index = sequence->index;
    } else
        index = sequence->index;
    position.x = actor->position.x + gUnk_086062B0[flip][index].x;
    position.y = actor->position.y + gUnk_086062B0[flip][index].y;
    position.z = actor->position.z + gUnk_086062B0[flip][index].z;
    if (target->kind == 4) {
        struct SceneActorRegistryResetView *victim;
        ((struct OctopusDrawable *)actor->drawable)->mode = 2;
        victim = target->player;
        sub_0807D2E0(victim, 0, &position, 0, flip, 0);
        goto animate;
    }
    if (target->kind > 3)
        goto animate;
    {
        struct Player *player;
        ((struct OctopusDrawable *)actor->drawable)->mode = 1;
        player = target->player;
        if (Mod(frame, context->interval) == 0) {
            s32 start, cur, elapsed;
            if (IsIdle(player))
                goto release;
            start = context->startTick;
            cur = gUnk_02000580[0x4F4];
            if (cur < start) {
                s32 wrapped = WrapCounter(start);
                elapsed = cur - wrapped;
            } else
                elapsed = cur - start;
            if (elapsed > context->holdTicks) {
            release:
                FacingAngle(actor, &quadrant);
                CellCenter((struct SceneVec *)&position, actor->terrainCell);
                Player_ApplyPositionRequest(player, (struct SceneVec *)&position, 0x40, quadrant,
                                            0x3C, 0, 0, 0);
                SetState(actor, 0, 0, 0);
                SetCtl(actor, (void *)SceneActorControl_OnIdle, 0x10);
                return;
            }
            {
                u32 status = Octopus_GetAnimationStatus(actor);
                u32 request = actor->fieldB6;
                u32 option = *(u32 *)((u8 *)actor + 0xD0);
                Player_BeginPositionRequest(player, (struct SceneVec *)&position, 0, status, flip,
                                            0, 0, request, option);
            }
        } else {
            Player_BeginPositionRequest(player, (struct SceneVec *)&position, 0,
                                        Octopus_GetAnimationStatus(actor), flip, 0, 0, 0, 0);
        }
    }
animate:
    SceneActorAnimation_SelectDirectionalFlags((void *)actor, 0x12);
    if (sequence->index == 0 && sequence->elapsed == 0)
        Sound_PlaySong(0x1DE);
    if (frame > 0xB4) {
        actor->field2B0 = 1;
        ((struct OctopusDrawable *)actor->drawable)->mode = 1;
    }
}
