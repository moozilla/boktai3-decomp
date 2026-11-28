#include "npc/manager/scene_actor_lifecycle.h"
#include "engine/math/math_shift.h"

struct KuroArcTag {
    u16 unknown0, unknown2, phase, entered;
    u32 elapsed;
    u16 fieldC, duration;
    u16 startX, startY, startZ;
};
extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
void Kuro_PlayTaggedAnimation(struct SceneActor *, s32, s32, s32, s32);

void Kuro_UpdateArcMove(void *ctx, struct SceneActor *actor)
{
    struct KuroArcTag *tag = (struct KuroArcTag *)actor->unknown20;
    struct ContactPosition *position = &actor->body.position;
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        actor->direction = 0;
        actor->body.direction = 0;
        actor->facing8 = 3;
        actor->facing4 = 1;
        Kuro_PlayTaggedAnimation(actor, 20, 1, 0, 4);
        tag->fieldC = 20;
        position->x = tag->startX;
        position->y = tag->startY;
        position->z = tag->startZ;
    }
    SceneActor_SetFlags(actor, 0x400);
    switch (tag->phase) {
        case 0: {
            s32 dx, dz, height;
            struct KuroArcTag *landing;
            tag->entered = 0;
            dx = actor->target.x - tag->startX;
            dz = actor->target.z - tag->startZ;
            position->x = tag->startX + Div(tag->elapsed * dx, tag->duration);
            position->z = tag->startZ + Div(tag->elapsed * dz, tag->duration);
            height = 200 * gUnk_086149C4[Div(tag->elapsed << 7, tag->duration) & 255];
            landing = tag;
            position->y = tag->startY + Math_ShiftTowardZero(height, 12);
            if (tag->elapsed >= tag->duration) {
                position->y = actor->target.y;
                {
                    u32 next = 1;
                    landing->phase = next;
                    landing->entered = next;
                    landing->elapsed = 0;
                }
                actor->finished = 1;
            }
            break;
        }
        case 1:
            tag->entered = 0;
            break;
    }
}
