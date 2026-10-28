// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
// confidence=low: emits four bobbing actors in a circular formation around this boss
struct FormationActor {
    u8 pad0[0x54];
    s16 position[4];
    u8 pad5C[0xB4];
    void *target;
    u8 pad114[0x198];
    u8 animationState[0x20];
    u8 animation;
    u8 pad2CD[0x233];
    s16 parameter;
};
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void SceneSteeringActor_CreateBobbingActor(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32,
                                           s32, s32, s32);
extern u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
static inline void GetOffset(s16 *position, u32 angle, s32 distance, u32 mask)
{
    position[0] = Scale(gUnk_086149C4[(angle + 64) & mask] * distance);
    {
        u8 k1 = 0;
        position[1] = k1;
    }
    position[2] = Scale(gUnk_086149C4[angle & mask] * distance);
}

inline static s32 RandomValue_SpawnBobbingFormation(void)
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
    s32 count;
    s32 offset;
    s32 direction;
    s16 position[4];
    s16 *origin;
    register s16 *vector asm("r4");
    SceneActors_StartUnmirroredAnimation(p, &p->animation, p->animationState, 0x12D, 2, 4);
    base = ((RandomValue_SpawnBobbingFormation() >> 8) & 1) << 5;
    mask = 255;
    vector = position;
    asm("" : : "r"(vector));
    direction = base + 16;
    offset = 0;
    origin = p->position;
    count = 3;
    do {
        u32 angle = (offset + base) & mask;
        GetOffset(position, angle, 256, mask);
        position[0] = position[0] + origin[0];
        position[1] = position[1] + origin[1];
        position[2] = position[2] + origin[2];
        position[1] += 320;
        SceneSteeringActor_CreateBobbingActor(position, q->target, 0, direction & mask, 180, 1, 0,
                                              1, 16, p->parameter, 128, 4, 30);
        direction += 64;
        offset += 64;
    } while ((--count) >= 0);
}
