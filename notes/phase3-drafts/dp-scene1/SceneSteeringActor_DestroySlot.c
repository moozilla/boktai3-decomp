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
        u16 *field = &early->collider.unknown3C;
        s32 width;
        s32 height;

        width = Dimension(field);
        field = &actor->collider.responseParameter;

        height = Dimension(field);

        BurstEmitter_Create(&actor->position, 2, 0, width, height, 40, 16);
        {
            u16 *frame = &actor->stateFrame;
            u32 zero = 0;

            *frame = 0;
            {
                u32 offset = 0x115;
                u8 *status;
                status = &actor->shadowStatus;
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
                u32 lowZero;
                state = &actor->state;
                lowZero = zero;
                *state = lowZero;
                actor->enter = lowZero;
            }
        }
    }
}
