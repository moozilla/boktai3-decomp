#include "global.h"
#ifdef NONMATCHING
// fuzzy: 94.0%
// fix.py best version (reprinted C)
// Remaining: saved frame and node registers, facing-load order, and flag-mask scheduling.
// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_phase.h"
// confidence=low: animate contact release, then resume the saved actor callback.
#define SceneActorControl_UpdateContactRelease sub_0807FCA8
s32 Math_Atan2(s32, s32);
void SceneActorEffect_PlayCategorySound(u32, u32, u32);
void TetherActor_StartInteractionEffect(void);
void SceneActorContacts_ReleaseNode(void *);
static inline void Delta(struct ContactPosition *v, struct ContactPosition *b,
                         struct ContactPosition *a)
{
    v->x = a->x - b->x;
    v->y = a->y - b->y;
    v->z = a->z - b->z;
}
static inline void Or(u16 *p, u32 mask)
{
    typeof(mask | (*p)) t1;
    t1 = mask | (*p);
    *p = t1;
}

static inline s32 Angle(s32 angle)
{
    if (angle > 128)
        angle -= 256;
    else if (angle < -128)
        angle += 256;
    return angle;
}
static inline void Or_1(u16 *p)
{
    typeof(0x20 | (*p)) t1;
    t1 = 0x20 | (*p);
    *p = t1;
}

void SceneActorControl_UpdateContactRelease(struct SceneActorRegistryResetView *input, s32 frame)
{
    u8 *group = ((u8 *)input) + 0x3D4;
    void **node;
    struct ContactPosition delta;
    s32 facing;
    if (SceneActorControl_TakeFlag(&input->field2B2))
        SceneActorAnimation_SelectDirectionalEntry(input, 0);
    node = (void **)(group + 0x88);
    Delta(&delta, &input->position, (void *)((*((u8 **)(*((void **)(group + 0x88))))) + 12));
    facing = input->direction;
    Angle(Math_Atan2(delta.x, delta.z) - facing);
    if (frame > 59) {
        if (frame <= 119) {
            SceneActorAnimation_SelectDirectionalEntry(input, 3);
            if (frame == 60)
                SceneActorEffect_PlayCategorySound(0, input->category, ((u8 *)input)[0x1E6]);
            if (frame == 100) {
                TetherActor_StartInteractionEffect();
                SceneActorContacts_ReleaseNode(*((void **)(group + 0x88)));
            }
            if ((frame >> 2) & 1)
                Or_1(&input->flags2D6);
            if (!input->drawableKind) {
                struct Sequence {
                    u8 pad[14];
                    u16 elapsed;
                };
                struct Sequence *s = (void *)(((u8 *)input->drawable) + 0x48);
                s->elapsed = 0;
            } else
                *((u16 *)(((u8 *)input->drawable) + 0x2C)) = 0;
        }
        if (input->field2B3) {
            Callback_InvokeActor(input, *((void **)(group + 0x244)));
            *((void **)(group + 0x88)) = 0;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0807FCA8);
#endif
