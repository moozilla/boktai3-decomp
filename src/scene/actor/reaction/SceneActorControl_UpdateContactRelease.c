#include "global.h"
#include "scene/actor/scene_actor_control_phase.h"
#include "engine/math/movement_curve.h"
#include "scene/collision/scene_actor_contacts.h"
// confidence=low: animate contact release, then resume the saved actor callback.
s32 Math_Atan2(s32, s32);
void SceneActorEffect_PlayCategorySound(u32, u32, u32);
void TetherActor_StartInteractionEffect(void);
void SceneActorContacts_ReleaseNode(void *);
static inline s32 Angle(s32 angle)
{
    if (angle > 128)
        angle -= 256;
    else if (angle < -128)
        angle += 256;
    return angle;
}
struct ContactTargetView {
    u8 unknown0[0xC];
    struct ContactPosition position;
};
static inline void SetActorFlags(struct ChaseActor *input, u32 mask)
{
    u32 value = mask;
    value |= input->base.reactionFlags;
    input->base.reactionFlags = value;
}

static inline s32 RelativeAngle(s32 x, s32 z, s32 facing)
{
    return Angle(Math_Atan2(x, z) - facing);
}

void SceneActorControl_UpdateContactRelease(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct ContactPosition delta;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2))
        SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    MovementCurve_SubtractPositions(
        &delta, &actor->base.body.position,
        &((struct ContactTargetView *)((struct SceneActorContactNode *)group->contactNode)->object)
             ->position);
    RelativeAngle(delta.x, delta.z, actor->base.body.direction);
    if (frame > 59) {
        if (frame <= 119) {
            SceneActorAnimation_SelectDirectionalEntry(actor, 3);
            if (frame == 60)
                SceneActorEffect_PlayCategorySound(0, actor->base.category,
                                                   (u8)actor->base.paletteOffset);
            if (frame == 100) {
                TetherActor_StartInteractionEffect();
                SceneActorContacts_ReleaseNode(group->contactNode);
            }
            if ((frame >> 2) & 1)
                SetActorFlags(actor, 0x20);
            if (!actor->base.drawableKind) {
                struct Sequence {
                    u8 pad[14];
                    u16 elapsed;
                };
                struct Sequence *s = (void *)(((u8 *)actor->base.drawable) + 0x48);
                s->elapsed = 0;
            } else
                *((u16 *)(((u8 *)actor->base.drawable) + 0x2C)) = 0;
        }
        if (actor->base.restart) {
            Callback_InvokeActor(actor, group->interactionCallback);
            group->contactNode = NULL;
        }
    }
}
