#define ActionGroup_RunSelectedCallbacks sub_08121A78
#define gActionGroupCallbacks gUnk_086106C8
#include "global.h"
struct ActionGroupEntry {
    u8 unknown0[0x9a];
    u8 kind;
    u8 unknown9B[9];
};
struct ActionGroup {
    u8 unknown0[0x38];
    u32 selected;
    struct ActionGroupEntry entries[4];
};
extern void *const gUnk_086106C8[];
s32 sub_08249240(void *, void *, void *);
static inline u8 ActionGroup_IsSelected(struct ActionGroup *group, s32 index)
{
    if (group->selected & (1 << index))
        return TRUE;
    return FALSE;
}
s32 sub_08121A78(struct ActionGroup *group)
{
    struct ActionGroupEntry *entries = group->entries;
    s32 i;
    for (i = 0; i < 4; i++) {
        if (ActionGroup_IsSelected(group, i)) {
            struct ActionGroupEntry *entry = (struct ActionGroupEntry *)(i * sizeof(*entries) + (u32)entries);
            sub_08249240(group, entry, gUnk_086106C8[entry->kind]);
        }
    }
    return 0;
}
