#include "npc/manager/scene_actor_lifecycle.h"
#include "engine/math/math_shift.h"

struct KuroJumpTag {
    u16 tag, pad2, phase, entered;
    u32 ticks;
    s16 velocity;
    u16 duration;
    u16 startX, startY, startZ;
};
void Kuro_PlayTaggedAnimation(struct SceneActor *, s32, s32, s32, s32);
s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);
u16 Sqrt(u32);
s32 Mod(s32, s32);
s32 Div(s32, s32);

void Kuro_UpdateJumpToTarget(void *manager, struct SceneActor *actor)
{
    struct KuroJumpTag *tag = (struct KuroJumpTag *)actor->unknown20;
    struct ContactPosition *position = &actor->body.position;
    if (actor->animationEntered) {
        u32 root;
        actor->animationEntered = 0;
        actor->finished = 0;
        actor->direction = 64;
        actor->body.direction = 64;
        actor->facing8 = 5;
        actor->facing4 = 2;
        Kuro_PlayTaggedAnimation(actor, 7, 2, 0, 4);
        tag->velocity = 0;
        tag->startX = position->x;
        tag->startY = position->y;
        tag->startZ = position->z;
        root = Sqrt(-200 * (actor->target.y - tag->startY));
        if (Mod(root, 10) > 4)
            tag->duration = Div(root, 10) + 1;
        else
            tag->duration = Div(root, 10);
    }
    SceneActor_SetFlags(actor, 0x400);
    switch (tag->phase) {
        case 0: {
            struct KuroJumpTag *state = (struct KuroJumpTag *)actor->unknown20;
            state->entered = 0;
            if (DisplayAnimation_TestFinished(&actor->drawable.common)) {
                state->phase = 1;
                state->entered = 1;
                state->ticks = 0;
            }
            break;
        }
        case 1: {
            struct KuroJumpTag *state = (struct KuroJumpTag *)actor->unknown20;
            u32 entered = state->entered;
            struct KuroJumpTag *landing;
            s32 dx, dz;
            state->entered = 0;
            landing = state;
            if (entered)
                Kuro_PlayTaggedAnimation(actor, 8, 2, 0, 4);
            dx = actor->target.x - tag->startX;
            dz = actor->target.z - tag->startZ;
            position->x = tag->startX + Div(tag->ticks * dx, tag->duration);
            position->z = tag->startZ + Div(tag->ticks * dz, tag->duration);
            position->y = tag->velocity * tag->ticks + tag->startY +
                          Math_ShiftTowardZero(-(s32)tag->ticks * tag->ticks, 1);
            if (tag->ticks >= tag->duration) {
                position->y = actor->target.y;
                landing->phase = 2;
                landing->entered = 1;
                landing->ticks = 0;
            }
            break;
        }
        case 2: {
            struct KuroJumpTag *state = (struct KuroJumpTag *)actor->unknown20;
            u32 entered = state->entered;
            state->entered = 0;
            if (entered)
                Kuro_PlayTaggedAnimation(actor, 9, 2, 0, 4);
            if (DisplayAnimation_TestFinished(&actor->drawable.common))
                actor->finished = 1;
            break;
        }
    }
}
