#include "global.h"
#include "engine_globals.h"
#include "ram.h"
#include "contact_position.h"

struct FormationActor {
    u8 pad0[0x54];
    struct ContactPosition position;
    u8 pad5C[0xB4];
    struct ContactPosition *target;
    u8 pad114[0x198];
    u8 animationState[0x20];
    u8 animation;
    u8 pad2CD[0x233];
    s16 parameter;
};

void SceneActors_StartUnmirroredAnimation(void *, u8 *, void *, s32, s32, s32);
void SceneSteeringActor_CreateBobbingActor(const struct ContactPosition *position,
                                           const struct ContactPosition *target, s32 velocityY,
                                           s32 direction, s32 steeringTimer, s32 turnStep,
                                           s32 steeringDelay, s32 speed, s32 speedLimit, s32 a,
                                           s32 b, s32 c, s32 d);
extern const s16 gUnk_086149C4[];

static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}

static inline void GetOffset(struct ContactPosition *position, u32 angle, s32 distance, u32 mask)
{
    position->x = Scale(gUnk_086149C4[(angle + 64) & mask] * distance);
    position->y = 0;
    position->z = Scale(gUnk_086149C4[angle & mask] * distance);
}

static inline s32 FormationRandomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}

void BossSlot2_SpawnBobbingFormation(struct FormationActor *p)
{
    struct FormationActor *q = p;
    u32 base;
    u32 mask;
    s32 remaining;
    s32 offset;
    s32 direction;
    struct ContactPosition position;
    const struct ContactPosition *origin;
    struct ContactPosition *vector;

    SceneActors_StartUnmirroredAnimation(p, &p->animation, p->animationState, 0x12D, 2, 4);
    base = ((FormationRandomValue() >> 8) & 1) << 5;
    mask = 255;
    vector = &position;
    direction = base + 16;
    offset = 0;
    origin = &p->position;
    remaining = 3;
    do {
        u32 angle = (offset + base) & mask;
        GetOffset(vector, angle, 256, mask);
        vector->x += origin->x;
        vector->y += origin->y;
        vector->z += origin->z;
        vector->y += 320;
        SceneSteeringActor_CreateBobbingActor(&position, q->target, 0, direction & mask, 180, 1, 0,
                                              1, 16, p->parameter, 128, 4, 30);
        direction += 64;
        offset += 64;
    } while (--remaining >= 0);
}
