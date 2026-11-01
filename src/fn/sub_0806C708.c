// MATCHING_CONCESSION: register-pinning
#include "scene_steering_actor.h"
s32 BurstEmitter_Create(const struct ContactPosition *, u32, u32, s32, s32, s32, s32);
void ScaledShadow_Destroy(void *);
static inline s32 Dimension(u16 *field)
{
    u32 value = *field;
    s32 result = 1;
    if (value)
        result = value;
    return result;
}
void SceneSteeringActor_DestroySlot(struct SceneSteeringActorPool *p,
                                    struct SceneSteeringActor *actor)
{
    struct SceneSteeringActor *early = actor;
    if (actor->enter) {
        register u16 *field asm("r1") = (void *)((u8 *)early + 0xe4);
        s32 width;
        register s32 height asm("r2");
        asm("" : "+r"(field));
        width = Dimension(field);
        field = (void *)((u8 *)actor + 0xe8);
        asm("" : "+r"(field));
        height = Dimension(field);
        asm("" : "+r"(height));
        BurstEmitter_Create(&actor->position, 2, 0, width, height, 40, 16);
        {
            u16 *frame = &actor->stateFrame;
            register u32 zero asm("r8") = 0;
            asm("" : "+r"(frame), "+r"(zero)::"r0");
            *frame = 0;
            {
                u32 offset = 0x115;
                u8 *status;
                status = (void *)((u8 *)actor + offset);
                if (!*status) {
                    ScaledShadow_Destroy(actor->shadow);
                    *status = 255;
                }
            }
            Sprite_Remove(actor);
            p->used &= ~(1U << actor->index);
            actor->index = -1;
            {
                u32 offset = 0x122;
                u8 *state;
                register u32 lowZero asm("r1");
                state = (void *)((u8 *)actor + offset);
                lowZero = zero;
                *state = lowZero;
                actor->enter = lowZero;
            }
        }
    }
}
