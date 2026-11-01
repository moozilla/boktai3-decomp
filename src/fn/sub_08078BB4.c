#include "scene_actor_attachment_dispatch.h"

void SceneActorAttachment_ApplySlotCommand(struct SceneActorRegistryResetView *actor, void *attachment, u32 inputMode,
                  u32 inputEvent, u32 inputCategory)
{
    s32 mode = (u8)inputMode;
    u8 event = inputEvent;
    u8 category = inputCategory;
    struct ActorAttachmentFollowView *follow = (void *)(&actor->body.id);
    s32 y;
    s32 z;
    if (event == 127) {
        switch (mode) {
            case 0:
                ActorAttachment_ReleaseResource(attachment, 0, (void *)actor->body.id);
                break;

            case 1:
                ActorAttachment_ReleaseResource(attachment, 1, (void *)actor->body.id);
                break;

            case 3:
                ActorAttachment_ReleasePartsResource(attachment, 3, (void *)actor->body.id);
                break;

            case 2:
                ActorAttachment_ReleaseResource(attachment, 2, (void *)actor->body.id);
                break;
        }
    } else {
        switch (mode) {
            case 0:
                SceneActorAttachment_ReleaseSpriteSlots(actor, &attachment);
                SceneActorAttachment_SelectOffsets(category, 0, &y, &z);
                if ((event == 0) && SceneActorAttachment_StatusBits(actor, 0x8000))
                    event = 3;
                ActorAttachment_CreateSpriteSlot(event, &follow->anchor, (void *)follow->identity,
                                                 y, z, category, actor, follow);
                SceneActorEffect_PlayCategorySoundIfEnabled(mode, event, category,
                                                            *(u8 *)&actor->paletteOffset);
                break;

            case 1:
                if (SceneActorAttachment_MotionBits(actor, 2))
                    break;
                SceneActorAttachment_ReleaseSpriteSlots(actor, &attachment);
                if ((u8)ActorAttachment_IsUnavailable(attachment)) {
                    s16 *value;
                    s32 initial;
                    u8 flag;
                    SceneActorAttachment_SelectOffsets(category, 1, &y, &z);
                    value = (s16 *)(&actor->fieldAA);
                    initial = actor->fieldA8;
                    flag = SceneActorAttachment_StatusBits(actor, 0x200) != 0;
                    ActorAttachment_CreateValueSpriteSlot(event, &actor->body.position,
                                                          (void *)actor->body.id, y, z, category,
                                                          value, initial, flag, actor, follow);
                    SceneActorEffect_PlayCategorySoundIfEnabled(1, event, category,
                                                                *(u8 *)&actor->paletteOffset);
                }
                break;

            case 2:
                SceneActorAttachment_SelectOffsets(category, 2, &y, &z);
                ActorAttachment_CreateSinglePartSlot(1, &actor->body.position,
                                                     (void *)actor->body.id, z, category, follow);
                break;

            case 3:
                SceneActorAttachment_ReleaseSpriteSlots(actor, &attachment);
                attachment =
                    ActorAttachment_ReleasePartsResource(attachment, 3, (void *)actor->body.id);
                SceneActorAttachment_SelectOffsets(category, 3, &y, &z);
                {
                    u32 count = event;
                    s16 *value;
                    s32 initial;
                    if (!count)
                        count = 3;
                    value = (s16 *)(&actor->fieldAA);
                    initial = actor->fieldA8;
                    ActorAttachment_CreatePartsSlotWithCount(
                        7, &actor->body.position, (void *)actor->body.id, z, category, value,
                        initial, count, follow, (u32)actor);
                }
                break;
        }
    }
}
