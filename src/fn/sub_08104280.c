#include "global.h"
struct Member {
    u8 pad0[0x140];
    void *owner;
    u8 pad144[0x184 - 0x144];
};
struct MemberManager {
    u8 pad0[0x1C];
    struct Member members[4];
    u8 pad[0x62C - 0x1C - 4 * 0x184];
    u32 activeMask;
};
extern struct MemberManager *gUnk_020001B0;
static inline u8 IsActive(u32 mask, s32 i)
{
    if ((1 << i) & mask)
        return 1;
    return 0;
}
struct Member *sub_08104280(void *owner)
{
    struct MemberManager *manager = gUnk_020001B0;
    s32 i;
    if (!manager)
        return NULL;
    for (i = 0; i <= 3; i++) {
        if (IsActive(manager->activeMask, i)) {
            struct Member *m = &manager->members[i];
            if (m->owner == owner)
                return m;
        }
    }
    return NULL;
}
