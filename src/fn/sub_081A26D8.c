#include "global.h"
#include "engine.h"
#include "ram.h"
// confidence=low: updates render shake and cached bearings/distances to tracked positions.
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u8 pad0[12];
    struct Vec position;
};
struct Hresvelgr {
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
    u8 animationState[0x20];
    u8 animation[0x20];
    struct Vec renderPosition;
    u8 pad1BC[0x154];
    u16 shake;
    u8 pad312[0x6a];
    u16 targetCount;
};
extern const s16 gUnk_086149C4[];
void Hresvelgr_DispatchSceneMessages(void *);
void Obj_StepPosition(void *);
void SceneActors_UpdateAnimation(void *, void *, void *);
void SceneActors_UpdateTargetMetrics(void *);
s32 Math_Atan2(s32, s32);
static inline u32 TestFlags(struct Hresvelgr *p, u32 mask)
{
    return p->flags & mask;
}

void Hresvelgr_UpdateAnimationAndTargets(struct Hresvelgr *p)
{
    struct Hresvelgr *actor = p;
    struct Hresvelgr *owner = p;
    struct Sprite *sprite = &p->sprite;
    s32 i;
    Hresvelgr_DispatchSceneMessages(actor);
    Obj_StepPosition(sprite);
    if (!TestFlags(actor, 0x800000))
        SceneActors_UpdateAnimation(actor, actor->animation, actor->animationState);
    actor->renderPosition.x = sprite->position.x;
    actor->renderPosition.y = sprite->position.y;
    actor->renderPosition.z = sprite->position.z;
    if (actor->shake) {
        u32 angle;
        s32 product, x, z;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        angle = *(u16 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1));
        product = gUnk_086149C4[(angle + 64) & 255] * (actor->shake << 2);
        if (product >= 0)
            x = product >> 12;
        else
            x = -(-product >> 12);
        actor->renderPosition.x += x;
        product = gUnk_086149C4[angle & 255] * (actor->shake << 2);
        if (product >= 0)
            z = product >> 12;
        else
            z = -(-product >> 12);
        actor->renderPosition.z += z;
        actor->shake--;
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
