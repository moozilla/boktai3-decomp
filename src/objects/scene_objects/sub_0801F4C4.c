#include "scene/collision/contact_actor.h"
#include "objects/scene_objects/scene_object_controller.h"

void SceneObject_UpdateLowered(struct SceneObjSlot *);
s32 SceneParticleBurst_CreateWhilePaused(const struct ContactPosition *position,
                                         const struct ContactPosition *range,
                                         const struct ContactPosition *jitter, s32 count,
                                         u32 priority, u32 flip);

void SceneObject_UpdateLowering(struct SceneObjSlot *slot)
{
    slot->lift -= 15;
    if ((s16)slot->lift < 0) {
        struct ContactPackedVector jitter;
        slot->lift = 0;
        Sound_StopSong(0x152);
        Sound_PlaySong(0x153);
        slot->timer = 30;
        slot->variant = SceneObjSlot_LiftVariant(SCENE_OBJ_LIFT_LOWERED);
        SceneObjSlot_ResetPose(slot, *(u8 *)&slot->loweredPose);
        slot->callback = SceneObject_UpdateLowered;

        jitter.x = jitter.y = jitter.z = 0;
        if (slot->dir & 1) {
            jitter.x = 128;
            SceneParticleBurst_CreateWhilePaused((const struct ContactPosition *)&slot->q0, NULL,
                                                 (const struct ContactPosition *)&jitter, 5, 2, 0);
        } else {
            jitter.z = 128;
            SceneParticleBurst_CreateWhilePaused((const struct ContactPosition *)&slot->q0, NULL,
                                                 (const struct ContactPosition *)&jitter, 5, 2, 0);
        }
    }
    slot->q0.position.y = slot->lift + slot->r0.position.y;
}
