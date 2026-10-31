#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
#include "engine.h"
#include "ram.h"
s32 Div(s32, s32);
s32 Mod(s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
u8 sub_080E7FF4(void *);
u8 Mimic_TryPursuitRoute(void *);
s32 Math_Atan2(s32, s32);
void ActorShadow_Enable(void *);
extern const s16 gUnk_086149C4[];
static inline void CellCenter(struct ContactPosition *out, u32 cell)
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

static inline s32 Scale12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}
static inline s32 Half(s32 n)
{
    if (n >= 0)
        return n >> 1;
    return -((-n) >> 1);
}
static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Scale12(gUnk_086149C4[(angle + 0x40) & 0xFF] * speed);
    out->y = 0;
    out->z = Scale12(gUnk_086149C4[angle & 0xFF] * speed);
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

static inline void RouteDelta(struct ContactPosition *out, struct ContactPosition *from, u8 *group)
{
    Delta(out, from, (struct ContactPosition *)(*(u8 **)(group + 0x1FC) + 0x1C));
}
static inline u8 HasMode(u8 *p)
{
    u32 mask = 1;
    if (*(u16 *)(p + 0x65C) & mask)
        return 1;
    return 0;
}
static inline s32 SpeedBase(u8 *context)
{
    register u8 *base asm("r4") = context;
    u32 offset;
    register u8 *sample asm("r0");
    asm("" : "+r"(base));
    offset = 0x65e;
    sample = base + offset;
    return *sample - 2;
}
// confidence=low: generic scene actor leap; species ownership is not established.
void SceneActor_LeapTowardTarget(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    u8 *group = (u8 *)&actor->group.common;
    struct ContactPosition point, delta;
    if (TakeFlag(&actor->field2B2)) {
        s32 correcting;
        s32 speed;
        if (!HasMode(context)) {
            if (!sub_080E7FF4(actor->context)) {
                SceneActor_ResetCollisionReaction(actor);
                return;
            }
        } else if (!Mimic_TryPursuitRoute(actor->context))
            return;
        CellCenter(&point, actor->terrainCell);
        Delta(&point, &actor->body.position, &point);
        {
            s32 quadrant = ((actor->body.direction + 0x20) & 0xFF) >> 6;
            correcting = 0;
            switch (quadrant) {
                case 0:
                case 2:
                    if ((point.z < 0 ? -point.z : point.z) > 0x40) {
                        CellCenter(&point, actor->terrainCell);
                        point.x = actor->body.position.x;
                        correcting = 1;
                    }
                    break;
                case 1:
                case 3:
                    if ((point.x < 0 ? -point.x : point.x) > 0x40) {
                        CellCenter(&point, actor->terrainCell);
                        point.z = actor->body.position.z;
                        correcting = 1;
                    }
                    break;
            }
        }
        if (correcting) {
            Delta(&delta, &actor->body.position, &point);
            speed = 4;
        } else {
            RouteDelta(&delta, &actor->body.position, group);
            gRandomIndex = (gRandomIndex + 1) & 1023;
            {
                u32 offset = gRandomIndex * 2;
                u32 random = *(u16 *)(offset + (u32)gRandomTable);
                s32 base = SpeedBase(context);
                speed = base + (random & 3);
            }
        }
        {
            s32 angle = Math_Atan2(delta.x, delta.z);
            u8 *heading = &actor->body.direction;
            *heading = angle;
            {
                u32 offset = 0x2ec;
                Polar((void *)((u8 *)actor + offset), *heading, speed);
            }
        }
        *(u16 *)((u8 *)actor + 0x2EE) = 24;
        SceneActorAnimation_SelectEntryFlags((void *)actor, 14,
                                             SceneActor_GetDirectionFlags(actor->body.direction));
        Sound_PlaySong(0x1B2);
        ActorShadow_Enable((u8 *)actor + 0x320);
    } else {
        u32 mask = 16;
        if (actor->flags2D0 & mask)
            actor->field2B0 = 1;
    }
}
