#include "global.h"
#include "engine.h"
// confidence=low: Vampire per-frame update: animation, scale wobble, shake, cached target bearings.
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u8 pad0[12];
    struct Vec position;
};
struct VampireTick {
    u8 pad0[0x48];
    struct Sprite sprite;
    u8 pad5C[0x40];
    u32 flags;
    u8 padA0[0x28];
    s32 distances[1];
    u8 bearings[1];
    u8 padCD[0x43];
    struct Vec *targets[1];
    u8 pad114[0x60];
    u8 targetCount;
    u8 pad175[0xab];
    u8 animationState[0x20];
    u8 animation[0x20];
    struct Vec renderPosition;
    u8 pad268[0xdc];
    s16 scale;
    u8 pad346[0xc];
    u16 yOffset;
    u8 pad354[0x44];
    u8 shakeAngle;
    u8 shakeAmp;
    u8 pad39a;
    u8 shake2;
};
extern const s16 gUnk_086149C4[];
void SceneActors_DispatchCommands(void *);
void Obj_StepPosition(void *);
void SceneActors_UpdateAnimation(void *, void *, void *);
void SceneActorGraphics_OscillateScale(void *);
void SceneActors_UpdateTargetMetrics(void *);
s32 Math_Atan2(s32, s32);

static inline u32 TestFlags(struct VampireTick *p, u32 mask)
{
    return p->flags & mask;
}

void Vampire_UpdateFrame(struct VampireTick *p)
{
    struct VampireTick *actor = p;
    struct VampireTick *owner = p;
    struct Sprite *sprite = &p->sprite;
    s32 i;
    SceneActors_DispatchCommands(actor);
    Obj_StepPosition(sprite);
    SceneActors_UpdateAnimation(owner, actor->animation, actor->animationState);
    SceneActorGraphics_OscillateScale(owner);
    {
        u16 *yo = &actor->yOffset;
        sprite->position.y = (actor->scale >> 4) + *yo;
    }
    actor->renderPosition.y = sprite->position.y;
    actor->renderPosition.x = sprite->position.x;
    actor->renderPosition.z = sprite->position.z;
    if (TestFlags(owner, 0x20000)) {
        s32 product, x, z;
        product = gUnk_086149C4[(actor->shakeAngle + 64) & 255] * (actor->shakeAmp << 2);
        if (product >= 0)
            x = product >> 12;
        else
            x = -(-product >> 12);
        actor->renderPosition.x += x;
        product = gUnk_086149C4[actor->shakeAngle] * (actor->shakeAmp << 2);
        if (product >= 0)
            z = product >> 12;
        else
            z = -(-product >> 12);
        actor->renderPosition.z += z;
    }
    if (actor->shake2) {
        u32 angle;
        s32 product, x, z;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        angle = *(u16 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1));
        product = gUnk_086149C4[(angle + 64) & 255] * (actor->shake2 << 2);
        if (product >= 0)
            x = product >> 12;
        else
            x = -(-product >> 12);
        actor->renderPosition.x += x;
        product = gUnk_086149C4[angle & 255] * (actor->shake2 << 2);
        if (product >= 0)
            z = product >> 12;
        else
            z = -(-product >> 12);
        actor->renderPosition.z += z;
        actor->shake2--;
    }
    SceneActors_UpdateTargetMetrics(owner);
    i = 0;
    if (i < actor->targetCount) {
        struct Sprite *base = &owner->sprite;
        do {
            struct Vec *target = owner->targets[i];
            if (target) {
                s32 x = target->x - base->position.x;
                s32 z = target->z - base->position.z;
                owner->bearings[i] = Math_Atan2(x, z);
                owner->distances[i] = x * x + z * z;
            }
            i++;
        } while (i < actor->targetCount);
    }
}
