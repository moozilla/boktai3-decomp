#include "scene_actor_reset.h"
#include "scene_actor_attachment.h"
#include "actor_attachment_manager.h"

s32 ActorAttachment_IsUnavailable(struct ActorAttachmentSlotView *);
void *ActorAttachment_CreatePartsSlotWithCount(u32, void *, u32, s32, u32, s16 *, s32, u32, void *,
                                               void *);
void SceneStimulus_AddTagged(s32, s32, s32, u32, u32, u16);
struct SceneActorAttachmentResponseGroup {
    u8 unknown0[0x4C];
    s16 stimulusHeight;
    u8 unknown4E[0xA0 - 0x4E];
    void *attachment;
};

static inline u32 ReadActorHalfFlags(struct SceneActorRegistryResetView *actor_, u32 mask)
{
    u16 *p = &actor_->flags2D4;
    return (*p) & mask;
}

static inline u32 ReadActorWordFlags(struct SceneActorRegistryResetView *actor_, u32 mask)
{
    u32 *p = &actor_->flags2D0;
    return (*p) & mask;
}

void SceneActorAttachment_ReplaceResponseSlot(struct SceneActorRegistryResetView *actor,
                                              u32 inputkind, u32 inputevent, u32 inputcategory)
{
    s32 kind = (u8)inputkind;
    u8 event = inputevent;
    u8 category = inputcategory;
    struct SceneActorRegistryResetView *owner = actor;
    struct SceneActorAttachmentResponseGroup *group = (void *)&actor->group;
    struct ActorAttachmentFollowView *follow = (void *)(&actor->body.id);
    s32 y;
    s32 z;
    if (event == 127) {
        switch (kind) {
            case 0:
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 0, (void *)actor->body.id);
                break;

            case 1:
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 1, (void *)actor->body.id);
                break;

            case 3:
                actor->attachment =
                    ActorAttachment_ReleasePartsResource(actor->attachment, 3, (void *)actor->body.id);
                break;

            case 2:
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 2, (void *)actor->body.id);
                break;
        }

    } else {
        switch (kind) {
            case 0:
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 0, (void *)actor->body.id);
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 1, (void *)actor->body.id);
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 2, (void *)actor->body.id);
                SceneActorAttachment_SelectOffsets(category, 0, &y, &z);
                if ((event == 0) && ReadActorWordFlags(actor, 0x8000))
                    event = 3;
                group->attachment = ActorAttachment_CreateSpriteSlot(event, &follow->anchor,
                                                                     (void *)follow->identity, y, z,
                                                                     category, actor, follow);
                SceneActorEffect_PlayCategorySoundIfEnabled(kind, event, category,
                                                            *(u8 *)&owner->paletteOffset);
                if ((event == 0) || (event == 3))
                    SceneStimulus_AddTagged(follow->anchor.x, follow->anchor.z,
                                            group->stimulusHeight, 0, 1, (u16)follow->identity);
                break;

            case 1:
                if (ReadActorHalfFlags(actor, 2))
                    break;
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 0, (void *)actor->body.id);
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 1, (void *)actor->body.id);
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 2, (void *)actor->body.id);
                if ((u8)ActorAttachment_IsUnavailable(actor->attachment)) {
                    s16 *value;
                    s32 initial;
                    u8 flag;
                    SceneActorAttachment_SelectOffsets(category, 1, &y, &z);
                    value = (s16 *)(&actor->fieldAA);
                    initial = actor->fieldA8;
                    flag = ReadActorWordFlags(actor, 0x200) != 0;
                    actor->attachment = ActorAttachment_CreateValueSpriteSlot(
                        event, &actor->body.position, (void *)actor->body.id, y, z, category, value, initial,
                        flag, actor, follow);
                    SceneActorEffect_PlayCategorySoundIfEnabled(1, event, category,
                                                                *(u8 *)&actor->paletteOffset);
                }
                break;

            case 2:
                SceneActorAttachment_SelectOffsets(category, 2, &y, &z);
                actor->attachment = ActorAttachment_CreateSinglePartSlot(
                    1, &actor->body.position, (void *)actor->body.id, z, category, follow);
                break;

            case 3:
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 0, (void *)actor->body.id);
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 1, (void *)actor->body.id);
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 2, (void *)actor->body.id);
                actor->attachment =
                    ActorAttachment_ReleasePartsResource(actor->attachment, 3, (void *)actor->body.id);
                SceneActorAttachment_SelectOffsets(category, 3, &y, &z);
                {
                    u32 count = event;
                    s16 *value;
                    s32 initial;
                    if (!count)
                        count = 3;
                    value = (s16 *)(&actor->fieldAA);
                    initial = actor->fieldA8;
                    actor->attachment = ActorAttachment_CreatePartsSlotWithCount(
                        7, &actor->body.position, actor->body.id, z, category, value, initial, count, follow,
                        actor);
                }
                break;
        }
    }
}
