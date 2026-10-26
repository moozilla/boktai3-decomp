#include "global.h"
#include "engine.h"

struct Position {
    s16 x, y, z, unk_06;
};
struct VolumeTransform {
    u8 unk_00[9];
    u8 direction;
    u8 unk_0A[2];
    struct Position position;
};
struct VolumeActor {
    u8 unk_00[0x48];
    struct VolumeTransform transform;
    s16 velocity[4];
    u8 unk_64[0x2C];
    u32 actionData;
    u8 unk_94[8];
    u32 flags;
    u8 unk_A0[2];
    u8 phase;
    u8 unk_A3;
    u32 timer;
    u8 direction4, direction8;
    u8 state, unk_AB, action, subaction, choice, pending, queued, restart, finished, special;
    u8 unk_B4[0x10];
    u32 actionTimer;
    u8 unk_C8[4];
    u8 requestedDirection;
    u8 unk_CD[0x33];
    void *callback;
    u8 unk_104[8];
    u8 *target;
    u8 unk_110[4];
    s8 alternateDirection;
    u8 unk_115[0x173];
    u8 bounds[0x1C];
    s16 destination[4];
    u8 animationState[0x20];
    u8 animation;
    u8 unk_2CD[0x47];
    void *palette;
    u8 unk_318[0x14];
    u8 effect[0x13C];
    s16 height;
    u8 unk_46A[6];
    u8 slowDown, impactDirection, speed, impactCount;
    u8 unk_474[0x6C];
    u8 movementMode;
    u8 unk_4E1;
    u8 reached, pathId, pointId;
    u8 unk_4E5[3];
    u8 transition, previousChoice, decisionMode, decisionIndex;
    u8 unk_4EC[6];
    s16 floorHeight;
    s16 attackFlags;
    u8 unk_4F6[6];
    s16 parameter;
    u8 unk_4FE[6];
    u8 paletteTimer, unk_505, paletteMode, paletteShift;
    u8 unk_508[0x6F8];
    u16 paletteData[16];
    u8 pathPosition[8];
};
void ActorAnimation_UpdateDirectional(void *);
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
void ProximityVolume_TranslateBounds(void *, void *);
void Sound_PlaySong(u32);
s32 ActorPosition_TestPath(void *, void *);
s32 SceneVolume_TestPlayerPosition(void *);
int Math_Atan2(int, int);
static inline void SetDirection(struct VolumeActor *p, int angle)
{
    p->transform.direction = angle;
    {
        s32 d = (p->transform.direction + 0x60) >> 6;
        u32 mask = 3;
        mask &= d;
        p->direction4 = mask;
    }
    {
        s32 d = (p->transform.direction + 0x70) >> 5;
        u32 mask = 7;
        mask &= d;
        p->direction8 = mask;
    }
}
static inline s32 Scale(int x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}

u8 *TerrainModifiers_FindCellOverride(u32, int);
static inline int TerrainHeight(struct Position *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    u32 cell;
    u8 *data;
    u32 shape, flags, height;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    flags = *data;
    shape = flags >> 4;
    {
        u32 mask = 15;
        mask &= flags;
        height = mask << 8;
    }
    switch (shape) {
        case 1:
            height -= *(u8 *)&position->z;
            break;
        case 2:
            height -= *(u8 *)&position->x;
            break;
    }
    return height;
}
static inline void SetVelocity(struct Position *velocity, u32 angle, s32 speed)
{
    s32 x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    s32 zero = 0;
    velocity->x = x;
    velocity->y = zero;
    velocity->z = Scale(gUnk_086149C4[angle & 255] * speed);
}
static inline void SetPalette(struct VolumeActor *p, u32 mode, u32 shift)
{
    p->paletteMode = mode;
    p->paletteShift = shift;
    p->paletteTimer = 0;
    p->palette = p->paletteData;
}
static inline void AddPosition(struct Position *out, struct VolumeTransform *a, struct Position *b)
{
    out->x = b->x + a->position.x;
    out->y = a->position.y + b->y;
    out->z = b->z + a->position.z;
}
static inline void SetHorizontalVelocity(struct VolumeActor *p, u32 angle, s32 speed)
{
    p->velocity[0] = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    p->velocity[2] = Scale(gUnk_086149C4[angle & 255] * speed);
}
static inline void FinishLunge(struct VolumeActor *p, void *callback, u32 one)
{
    p->unk_AB = one;
    p->callback = callback;
    p->actionData = 0;
    p->restart = one;
    p->phase = 0;
    p->timer = 0;
    p->pending = one;
}
static inline int HeightInvalid(u32 difference, u32 limit)
{
    return difference > limit;
}
#ifdef NONMATCHING
// fuzzy: 99.8%
// Tentative: approaches a destination during a short lunge animation.
void BossSlot2_UpdateLungeApproach(struct VolumeActor *p)
{
    int zero = 0;
    struct VolumeActor *q = p;
    int dx, dz, angle;
    int speed;
    if (p->restart) {
        p->restart = zero;
        ProximityVolume_TranslateBounds(p->bounds, &p->transform.position);
        q->reached = 0;
        if (q->attackFlags & 2)
            Sound_PlaySong(0x540);
        q->attackFlags &= ~2;
    }
    if (q->movementMode == 1)
        SceneVolume_TestPlayerPosition(q);
    else if (q->movementMode == 3)
        ActorPosition_TestPath(q, q->pathPosition);
    {
        u32 mask = 0x480004;
        p->flags |= mask;
    }
    if (p->timer == 8)
        Sound_PlaySong(0x344);
    if (++p->timer > 15) {
        u32 mask = ~0x50010;
        p->flags &= mask;
        FinishLunge(p, ActorAnimation_UpdateDirectional, 1);
        return;
    }
    if ((u32)(p->timer - 5) <= 7) {
        u32 mask = 0x10010;
        p->flags |= mask;
    } else {
        u32 mask = 0x50000;
        p->flags |= mask;
    }
    if (p->timer > 7)
        SetPalette(q, 1, 5);
    {
        int radiusSquared = 2500;
        speed = 32;
        dx = q->destination[0] - p->transform.position.x;
        dz = q->destination[2] - p->transform.position.z;
        angle = Math_Atan2(dx, dz) & 0xE0;
        if (dx * dx + dz * dz > radiusSquared) {
            struct VolumeTransform *base = &p->transform;
            struct Position position;
            int invalid;
            u32 heightDifference;
            SetVelocity(&position, angle, speed);
            AddPosition(&position, base, &position);
            heightDifference = TerrainHeight(&position) - q->floorHeight + 511;
            invalid = HeightInvalid(heightDifference, 766);
            if (!invalid)
                goto advance;
        }
        p->velocity[0] = dx;
        p->velocity[2] = dz;
        q->reached = 1;
        SceneActors_StartEightDirectionAnimation(p, &q->animation, q->animationState, 0xF4, 1, 1,
                                                 4);
        return;
    }
advance:
    SetHorizontalVelocity(p, angle, speed);
    SetDirection(p, angle);
    SceneActors_StartEightDirectionAnimation(p, &q->animation, q->animationState, 0xF9, 1, 1, 4);
}
#else
INCLUDE_ASM("asm/nonmatching", BossSlot2_UpdateLungeApproach);
#endif
