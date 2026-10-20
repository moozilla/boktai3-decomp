#include "scene_actor_behavior.h"
#include "actor_attachment_manager.h"
#include "scene_actor_attachment.h"
struct ActorAttachmentSlotView *
ActorAttachment_CreatePartsSlotWithCount(u8, struct ContactPosition *, void *, s32, u32, s16 *, s32,
                                         u32, struct ActorAttachmentFollowView *, u32);
s32 ActorAttachment_IsUnavailable(struct ActorAttachmentSlotView *);
#ifdef NONMATCHING
// fuzzy: 75.0%
// Offset defaults keep unsupported categories defined; the ROM omits these stores.
// Remaining differences include entry ordering, argument setup and scratch registers.
#define SceneActorAttachment_ApplyResourceCommand sub_08078BB4
static inline u32 Flags32(struct SceneActorBehaviorView *actor, u32 mask)
{
    return actor->flags2D0 & mask;
}
static inline u32 Flags16(struct SceneActorBehaviorView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
void SceneActorAttachment_ApplyResourceCommand(struct SceneActorBehaviorView *actor,
                                               struct ActorAttachmentSlotView *inputSlot,
                                               u8 inputKind, u8 inputFrame, u8 inputCategory)
{
    struct ActorAttachmentSlotView *slot = inputSlot;
    s32 kind = inputKind;
    u32 frame = inputFrame;
    u32 category = inputCategory;
    struct ActorAttachmentFollowView *follow = (void *)((u8 *)actor + 0x50);
    s32 y = 0, z = 0;
    if (frame == 127) {
        switch (kind) {
            case 0:
                ActorAttachment_ReleaseResource(slot, 0, *(void **)((u8 *)actor + 0x50));
                break;
            case 1:
                ActorAttachment_ReleaseResource(slot, 1, *(void **)((u8 *)actor + 0x50));
                break;
            case 3:
                ActorAttachment_ReleasePartsResource(slot, 3, *(void **)((u8 *)actor + 0x50));
                break;
            case 2:
                ActorAttachment_ReleaseResource(slot, 2, *(void **)((u8 *)actor + 0x50));
                break;
        }
    } else {
        switch (kind) {
            case 0:
                slot = ActorAttachment_ReleaseResource(slot, 0, *(void **)((u8 *)actor + 0x50));
                slot = ActorAttachment_ReleaseResource(slot, 1, *(void **)((u8 *)actor + 0x50));
                ActorAttachment_ReleaseResource(slot, 2, *(void **)((u8 *)actor + 0x50));
                SceneActorAttachment_SelectOffsets(category, 0, &y, &z);
                if (!frame && Flags32(actor, 0x8000))
                    frame = 3;
                ActorAttachment_CreateSpriteSlot(frame, &follow->anchor,
                                                 *(void **)((u8 *)actor + 0x50), y, z, category,
                                                 actor, follow);
                SceneActorEffect_PlayCategorySoundIfEnabled(kind, frame, category,
                                                            *((u8 *)actor + 0x1E6));
                break;
            case 1:
                if (Flags16(actor, 2))
                    break;
                slot = ActorAttachment_ReleaseResource(slot, 0, *(void **)((u8 *)actor + 0x50));
                slot = ActorAttachment_ReleaseResource(slot, 1, *(void **)((u8 *)actor + 0x50));
                slot = ActorAttachment_ReleaseResource(slot, 2, *(void **)((u8 *)actor + 0x50));
                if (!(u8)ActorAttachment_IsUnavailable(slot))
                    break;
                SceneActorAttachment_SelectOffsets(category, 1, &y, &z);
                {
                    s16 *value = (s16 *)((u8 *)actor + 0xAA);
                    s32 limit = *(s16 *)((u8 *)actor + 0xA8);
                    u32 flag = -Flags32(actor, 0x200) >> 31;
                    ActorAttachment_CreateValueSpriteSlot(
                        frame, &actor->position, *(void **)((u8 *)actor + 0x50), y, z, category,
                        value, limit, flag, actor, follow);
                }
                SceneActorEffect_PlayCategorySoundIfEnabled(1, frame, category,
                                                            *((u8 *)actor + 0x1E6));
                break;
            case 2:
                SceneActorAttachment_SelectOffsets(category, 2, &y, &z);
                ActorAttachment_CreateSinglePartSlot(
                    1, &actor->position, *(void **)((u8 *)actor + 0x50), z, category, follow);
                break;
            case 3: {
                u32 count;
                slot = ActorAttachment_ReleaseResource(slot, 0, *(void **)((u8 *)actor + 0x50));
                slot = ActorAttachment_ReleaseResource(slot, 1, *(void **)((u8 *)actor + 0x50));
                slot = ActorAttachment_ReleaseResource(slot, 2, *(void **)((u8 *)actor + 0x50));
                ActorAttachment_ReleasePartsResource(slot, 3, *(void **)((u8 *)actor + 0x50));
                SceneActorAttachment_SelectOffsets(category, 3, &y, &z);
                count = frame;
                if (!count)
                    count = 3;
                {
                    s16 *value = (s16 *)((u8 *)actor + 0xAA);
                    s32 limit = *(s16 *)((u8 *)actor + 0xA8);
                    ActorAttachment_CreatePartsSlotWithCount(
                        7, &actor->position, *(void **)((u8 *)actor + 0x50), z, category, value,
                        limit, count, follow, (u32)actor);
                }
                break;
            }
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08078BB4);
#endif
