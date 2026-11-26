#include "global.h"

struct SceneNpcAttachmentDescriptor {
    void *resource;
    u8 unk_04[0x264 - 4];
};

struct SceneNpc {
    u8 unk_00[0x5C];
    struct SceneNpcAttachmentDescriptor descriptor;
    void *attachment;
};

void *ActorAttachment_UpdateFromDescriptor(void *, void *, u32, u32, void *, s32, s32, s32);
void *ActorAttachment_ReleaseResource(void *, u32, void *);
void *ActorAttachment_ReleasePartsResource(void *, u32, void *);
void Sound_PlaySong(u32);

// Tentative role: plays or releases an NPC attachment effect.
void SceneNpc_PlayAttachmentEffect(struct SceneNpc *npc, s32 effect)
{
    switch (effect) {
        case 2:
            npc->attachment = ActorAttachment_UpdateFromDescriptor(
                npc->attachment, &npc->descriptor, 0, 0, 0, 0, 0, 0);
            Sound_PlaySong(0x1EC);
            break;
        case 1:
            npc->attachment = ActorAttachment_UpdateFromDescriptor(
                npc->attachment, &npc->descriptor, 0, 3, 0, 0, 0, 0);
            Sound_PlaySong(0x1EC);
            break;
        case 0:
            npc->attachment = ActorAttachment_UpdateFromDescriptor(
                npc->attachment, &npc->descriptor, 0, 1, 0, 0, 0, 0);
            Sound_PlaySong(0x1B1);
            break;
        case 3:
            npc->attachment = ActorAttachment_UpdateFromDescriptor(
                npc->attachment, &npc->descriptor, 0, 2, 0, 0, 0, 0);
            Sound_PlaySong(0x1ED);
            break;
        case 8:
            npc->attachment =
                ActorAttachment_ReleaseResource(npc->attachment, 0, npc->descriptor.resource);
            npc->attachment =
                ActorAttachment_ReleaseResource(npc->attachment, 1, npc->descriptor.resource);
            npc->attachment =
                ActorAttachment_ReleaseResource(npc->attachment, 2, npc->descriptor.resource);
            npc->attachment =
                ActorAttachment_ReleaseResource(npc->attachment, 4, npc->descriptor.resource);
            npc->attachment =
                ActorAttachment_ReleaseResource(npc->attachment, 5, npc->descriptor.resource);
            npc->attachment =
                ActorAttachment_ReleasePartsResource(npc->attachment, 3, npc->descriptor.resource);
            break;
    }
}
