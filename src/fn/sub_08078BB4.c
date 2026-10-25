#include "global.h"

// confidence=low: applies attachment command (mode 0-3) for an actor; kind 0x7F only releases slots

// confidence=low: applies attachment command (mode 0-3) for an actor; kind 0x7F only releases slots
// confidence=low: applies attachment command (mode 0-3) for an actor; kind 0x7F only releases slots

// confidence=low: applies attachment command (mode 0-3) for an actor; kind 0x7F only releases slots
struct Sub08078BB4Pos {
    s16 x, y, z, pad;
};

struct Sub08078BB4Actor {
    u8 unknown0[0x50];
    u32 id;
    u8 unknown54[0x5C - 0x54];
    struct Sub08078BB4Pos position;
    u8 unknown64[0xA8 - 0x64];
    s16 fieldA8, fieldAA;
    u8 unknownAC[0x1E6 - 0xAC];
    u8 flag1E6;
    u8 unknown1E7[0x2D0 - 0x1E7];
    u32 word2D0;
    u16 flags2D4;
};

void *ActorAttachment_ReleaseResource(void *, u32, u32);
void *ActorAttachment_ReleasePartsResource(void *, u32, u32);
s32 ActorAttachment_IsUnavailable(void *);
void SceneActorAttachment_SelectOffsets(u8, u8, s32 *, s32 *);
void *ActorAttachment_CreateSpriteSlot(u32, void *, u32, s32, s32, u32, void *, void *);
void *ActorAttachment_CreateValueSpriteSlot(u32, void *, u32, s32, s32, u32, void *, s32, u32,
                                            void *, void *);
void *ActorAttachment_CreateSinglePartSlot(u32, void *, u32, s32, u32, void *);
void *ActorAttachment_CreatePartsSlotWithCount(u32, void *, u32, s32, u32, void *, s32, u32, void *,
                                               void *);
void SceneActorEffect_PlayCategorySoundIfEnabled(u32, u32, u32, u32);

#ifdef NONMATCHING
// fuzzy: 41.1%
// scope shape 98 (differences once low registers are set aside)
// confidence=low: applies attachment command (mode 0-3) for an actor; kind 0x7F only releases slots
void sub_08078BB4(void *arg0, void *arg1, u32 arg2, u32 arg3, u32 arg4)
{
    struct Sub08078BB4Actor *actor = arg0;
    void *attachment = arg1;
    u8 mode = arg2;
    u8 kind = arg3;
    u8 category = arg4;
    s32 y, z;

    if (kind == 0x7F) {
        switch (mode) {
            case 0:
                ActorAttachment_ReleaseResource(attachment, 0, actor->id);
                break;
            case 1:
                ActorAttachment_ReleaseResource(attachment, 1, actor->id);
                break;
            case 2:
                ActorAttachment_ReleaseResource(attachment, 2, actor->id);
                break;
            case 3:
                ActorAttachment_ReleasePartsResource(attachment, 3, actor->id);
                break;
        }
        return;
    }

    switch (mode) {
        case 0:
            attachment = ActorAttachment_ReleaseResource(attachment, 0, actor->id);
            attachment = ActorAttachment_ReleaseResource(attachment, 1, actor->id);
            attachment = ActorAttachment_ReleaseResource(attachment, 2, actor->id);
            SceneActorAttachment_SelectOffsets(category, 0, &y, &z);
            if (kind == 0 && (actor->word2D0 & 0x8000))
                kind = 3;
            ActorAttachment_CreateSpriteSlot(kind, &actor->position, actor->id, y, z, category,
                                             actor, &actor->id);
            SceneActorEffect_PlayCategorySoundIfEnabled(mode, kind, category, actor->flag1E6);
            break;
        case 1:
            if (actor->flags2D4 & 2)
                break;
            attachment = ActorAttachment_ReleaseResource(attachment, 0, actor->id);
            attachment = ActorAttachment_ReleaseResource(attachment, 1, actor->id);
            attachment = ActorAttachment_ReleaseResource(attachment, 2, actor->id);
            if ((u8)ActorAttachment_IsUnavailable(attachment)) {
                SceneActorAttachment_SelectOffsets(category, 1, &y, &z);
                ActorAttachment_CreateValueSpriteSlot(
                    kind, &actor->position, actor->id, y, z, category, &actor->fieldAA,
                    actor->fieldA8, (actor->word2D0 & 0x200) != 0, actor, &actor->id);
                SceneActorEffect_PlayCategorySoundIfEnabled(1, kind, category, actor->flag1E6);
            }
            break;
        case 2:
            SceneActorAttachment_SelectOffsets(category, 2, &y, &z);
            ActorAttachment_CreateSinglePartSlot(1, &actor->position, actor->id, z, category,
                                                 &actor->id);
            break;
        case 3:
            attachment = ActorAttachment_ReleaseResource(attachment, 0, actor->id);
            attachment = ActorAttachment_ReleaseResource(attachment, 1, actor->id);
            attachment = ActorAttachment_ReleaseResource(attachment, 2, actor->id);
            attachment = ActorAttachment_ReleasePartsResource(attachment, 3, actor->id);
            SceneActorAttachment_SelectOffsets(category, 3, &y, &z);
            if (kind == 0)
                kind = 3;
            ActorAttachment_CreatePartsSlotWithCount(7, &actor->position, actor->id, z, category,
                                                     &actor->fieldAA, actor->fieldA8, kind,
                                                     &actor->id, actor);
            break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08078BB4);
#endif
