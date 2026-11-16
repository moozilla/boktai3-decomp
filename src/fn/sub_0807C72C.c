#include "scene_actor_attachment_dispatch.h"

void SceneStimulus_AddTagged(s32, s32, s32, u32, u32, u16);
struct SceneActorAttachmentResponseGroup {
    u8 unknown0[0x4C];
    s16 stimulusHeight;
    u8 unknown4E[0xA0 - 0x4E];
    void *attachment;
};

void SceneActorAttachment_ReplaceResponseSlot(struct ChaseActor *actor, u32 inputkind,
                                              u32 inputevent, u32 inputcategory)
{
    s32 kind = (u8)inputkind;
    u8 event = inputevent;
    u8 category = inputcategory;
    struct ChaseActor *owner = actor;
    struct SceneActorAttachmentResponseGroup *group = (void *)&actor->group.common;
    struct ActorAttachmentFollowView *follow = (void *)(&actor->base.body.id);
    s32 y;
    s32 z;
    if (event == 127) {
        switch (kind) {
            case 0:
                actor->group.attachment = ActorAttachment_ReleaseResource(
                    actor->group.attachment, 0, (void *)actor->base.body.id);
                break;

            case 1:
                actor->group.attachment = ActorAttachment_ReleaseResource(
                    actor->group.attachment, 1, (void *)actor->base.body.id);
                break;

            case 3:
                actor->group.attachment = ActorAttachment_ReleasePartsResource(
                    actor->group.attachment, 3, (void *)actor->base.body.id);
                break;

            case 2:
                actor->group.attachment = ActorAttachment_ReleaseResource(
                    actor->group.attachment, 2, (void *)actor->base.body.id);
                break;
        }

    } else {
        switch (kind) {
            case 0:
                SceneActorAttachment_ReleaseSpriteSlots(actor, &actor->group.attachment);
                SceneActorAttachment_SelectOffsets(category, 0, &y, &z);
                if ((event == 0) && SceneActorAttachment_StatusBits(actor, 0x8000))
                    event = 3;
                group->attachment = ActorAttachment_CreateSpriteSlot(event, &follow->anchor,
                                                                     (void *)follow->identity, y, z,
                                                                     category, actor, follow);
                SceneActorEffect_PlayCategorySoundIfEnabled(kind, event, category,
                                                            *(u8 *)&owner->base.paletteOffset);
                if ((event == 0) || (event == 3))
                    SceneStimulus_AddTagged(follow->anchor.x, follow->anchor.z,
                                            group->stimulusHeight, 0, 1, (u16)follow->identity);
                break;

            case 1:
                if (SceneActorAttachment_MotionBits(actor, 2))
                    break;
                SceneActorAttachment_ReleaseSpriteSlots(actor, &actor->group.attachment);
                if ((u8)ActorAttachment_IsUnavailable(actor->group.attachment)) {
                    s16 *value;
                    s32 initial;
                    u8 flag;
                    SceneActorAttachment_SelectOffsets(category, 1, &y, &z);
                    value = (s16 *)(&actor->base.fieldAA);
                    initial = actor->base.fieldA8;
                    flag = SceneActorAttachment_StatusBits(actor, 0x200) != 0;
                    actor->group.attachment = ActorAttachment_CreateValueSpriteSlot(
                        event, &actor->base.body.position, (void *)actor->base.body.id, y, z,
                        category, value, initial, flag, actor, follow);
                    SceneActorEffect_PlayCategorySoundIfEnabled(1, event, category,
                                                                *(u8 *)&actor->base.paletteOffset);
                }
                break;

            case 2:
                SceneActorAttachment_SelectOffsets(category, 2, &y, &z);
                actor->group.attachment = ActorAttachment_CreateSinglePartSlot(
                    1, &actor->base.body.position, (void *)actor->base.body.id, z, category,
                    follow);
                break;

            case 3:
                SceneActorAttachment_ReleaseSpriteSlots(actor, &actor->group.attachment);
                actor->group.attachment = ActorAttachment_ReleasePartsResource(
                    actor->group.attachment, 3, (void *)actor->base.body.id);
                SceneActorAttachment_SelectOffsets(category, 3, &y, &z);
                {
                    u32 count = event;
                    s16 *value;
                    s32 initial;
                    if (!count)
                        count = 3;
                    value = (s16 *)(&actor->base.fieldAA);
                    initial = actor->base.fieldA8;
                    actor->group.attachment = ActorAttachment_CreatePartsSlotWithCount(
                        7, &actor->base.body.position, (void *)actor->base.body.id, z, category,
                        value, initial, count, follow, (u32)actor);
                }
                break;
        }
    }
}
