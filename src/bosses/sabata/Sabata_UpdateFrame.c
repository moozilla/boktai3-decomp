#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "bosses/state_actor_action_entry.h"
#include "ui/hud/scene_indicator.h"
// confidence=low: BossSlot2 per-frame update: animation, shake, cached target bearings.
struct Sprite {
    u8 pad0[12];
    struct Vec position;
};
struct SabataTick {
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

void Sabata_UpdateFrame(struct SabataTick *p)
{
    struct SabataTick *owner = p;
    struct Sprite *sprite = &p->sprite;
    s32 i;
    sub_0817B2DC(p);
    if (SceneActorState_TestFlags((struct StateActor *)p, 0x80000))
        p->cell = 0;
    else if (!p->cell) {
        CellObject_Init(p->cellObject, &p->sprite.position);
        p->cell = p->cellObject;
    }
    Obj_StepPosition(sprite);
    NavMesh_UpdateProbe(p->probe, &sprite->position);
    SceneActors_UpdateAnimation(owner, p->animation, p->animationState);
    p->renderPosition.x = sprite->position.x;
    p->renderPosition.y = sprite->position.y;
    p->renderPosition.z = sprite->position.z;
    if (SceneActorState_TestFlags((struct StateActor *)owner, 0x20000)) {
        s32 product, x, z;
        product = gUnk_086149C4[(p->shakeAngle + 64) & 255] * (p->shakeAmp << 2);
        if (product >= 0)
            x = product >> 12;
        else
            x = -(-product >> 12);
        p->renderPosition.x += x;
        product = gUnk_086149C4[p->shakeAngle] * (p->shakeAmp << 2);
        if (product >= 0)
            z = product >> 12;
        else
            z = -(-product >> 12);
        p->renderPosition.z += z;
    }
    if (p->shake2) {
        u32 angle;
        s32 product, x, z;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        angle = *(u16 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1));
        product = gUnk_086149C4[(angle + 64) & 255] * (p->shake2 << 2);
        if (product >= 0)
            x = product >> 12;
        else
            x = -(-product >> 12);
        p->renderPosition.x += x;
        product = gUnk_086149C4[angle & 255] * (p->shake2 << 2);
        if (product >= 0)
            z = product >> 12;
        else
            z = -(-product >> 12);
        p->renderPosition.z += z;
        p->shake2--;
    }
    if (p->b475 <= 0x59)
        p->b475++;
    else
        p->b474 = 0;
    if (p->w4E6 == 0)
        p->b4E5 = 0;
    else {
        p->w4E6--;
        p->b4E5 = 1;
    }
    SceneActors_UpdateTargetMetrics(owner);
    i = 0;
    if (i < p->targetCount) {
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
        } while (i < p->targetCount);
    }
}
