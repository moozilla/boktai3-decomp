#define SceneActorReaction_SetProfileEntries sub_080B7580
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_profile.h"
static inline void SetEntry(u32 value, struct SceneActorProfileEntry *p)
{
    p->value = value;
    p->flags = value >> 8;
}
void sub_080B7580(struct SceneActorRegistryResetView *actor)
{
    s32 category = actor->category;
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 0);
    u32 mask = 0x280;
    u16 flags = actor->flags2D4;
    u32 special = mask & flags;
    if (special) {
        switch (category) {
            case 5:
                SetEntry(0x63, entry);
                SetEntry(0x27, entry + 1);
                break;
            case 6:
                SetEntry(0x65, entry);
                SetEntry(0x29, entry + 1);
                SetEntry(0x1E, entry + 2);
                break;
            case 7:
                SetEntry(0x67, entry);
                SetEntry(0x2B, entry + 1);
                SetEntry(0x24, entry + 2);
                break;
        }
    } else {
        switch (category) {
            case 5:
                SetEntry(0, entry);
                SetEntry(6, entry + 1);
                break;
            case 6:
                if (flags & 0x200) {
                    SetEntry(0x65, entry);
                    SetEntry(0x29, entry + 1);
                    SetEntry(0x1E, entry + 2);
                } else {
                    SetEntry(2, entry);
                    SetEntry(8, entry + 1);
                    SetEntry(0x1B, entry + 2);
                }
                break;
            case 7:
                if ((u16)(flags & 0x200)) {
                    SetEntry(0x67, entry);
                    SetEntry(0x2B, entry + 1);
                    SetEntry(0x24, entry + 2);
                } else {
                    SetEntry(4, entry);
                    SetEntry(0xA, entry + 1);
                    SetEntry(0x21, entry + 2);
                }
                break;
        }
    }
}
