#include "global.h"
#include "engine.h"
#include "ram.h"
// confidence=low: BossSlot2 per-frame update: animation, shake, cached target bearings.
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u8 pad0[12];
    struct Vec position;
};
struct BossSlot2Tick {
    u8 pad0[0x48];
    struct Sprite sprite;
    u8 pad5C[0x8];
    void *cell;
    u8 pad68[0x34];
    u32 flags;
    u8 padA0[0x28];
    s32 distances[1];
    u8 bearings[1];
    u8 padCD[0x43];
    struct Vec *targets[1];
    u8 pad114[0x60];
    u8 targetCount;
    u8 pad175[0x103];
    u8 cellObject[0x10];
    u8 probe[0x24];
    u8 animationState[0x20];
    u8 animation[0x20];
    struct Vec renderPosition;
    u8 pad2f4[0x180];
    u8 b474;
    u8 b475;
    u8 pad476[0x6f];
    u8 b4E5;
    u16 w4E6;
    u8 pad4E8[0x28];
    u8 shakeAngle;
    u8 shakeAmp;
    u8 pad512;
    u8 shake2;
};
extern const s16 gUnk_086149C4[];
void sub_0817B2DC(void *);
void CellObject_Init(void *, void *);
void Obj_StepPosition(void *);
void NavMesh_UpdateProbe(void *, void *);
void SceneActors_UpdateAnimation(void *, void *, void *);
void SceneActors_UpdateTargetMetrics(void *);
s32 Math_Atan2(s32, s32);

static inline u32 TestFlags(struct BossSlot2Tick *p, u32 mask)
{
    return p->flags & mask;
}

void BossSlot2_UpdateFrame(struct BossSlot2Tick *p)
{
    struct BossSlot2Tick *actor = p;
    struct BossSlot2Tick *owner = p;
    struct Sprite *sprite = &p->sprite;
    s32 i;
    sub_0817B2DC(actor);
    if (TestFlags(actor, 0x80000))
        actor->cell = 0;
    else if (!actor->cell) {
        CellObject_Init(actor->cellObject, (u8 *)actor + 0x54);
        actor->cell = actor->cellObject;
    }
    Obj_StepPosition(sprite);
    NavMesh_UpdateProbe(actor->probe, (u8 *)sprite + 0xc);
    SceneActors_UpdateAnimation(owner, actor->animation, actor->animationState);
    actor->renderPosition.x = sprite->position.x;
    actor->renderPosition.y = sprite->position.y;
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
    if (actor->b475 <= 0x59)
        actor->b475++;
    else
        actor->b474 = 0;
    if (actor->w4E6 == 0)
        actor->b4E5 = 0;
    else {
        actor->w4E6--;
        actor->b4E5 = 1;
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
