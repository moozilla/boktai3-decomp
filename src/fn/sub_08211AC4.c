#include "global.h"

struct FbFlags {
    u32 flags;
    u8 rest[0x5c];
};
struct FbBook {
    u8 pad0[0x388];
    u8 base[0x20];
    u8 animBase[8];
    struct FbFlags slot[18];
    u8 pad2[0xb60 - 0x3a8 - 8 - 18 * 0x60];
    s16 category;
    u8 pad3[0xb84 - 0xb62];
    u8 entry[1];
};
s32 ForgingBook_GetCategoryEntryOffset(s32);
s32 ForgingBook_GetCategoryEntryCount(s32);
s32 ForgingBook_IsWeaponDiscovered(u32);
s32 ForgingBook_IsItemDiscovered(u32);
u32 ForgingBook_GetEntryAnimation(u32);
void Anim_LoadKeyframe(void *, void *, u32, u32);
#define ForgingBook_RefreshEntrySprites sub_08211AC4
u32 ForgingBook_RefreshEntrySprites(struct FbBook *p)
{
    s32 offset = ForgingBook_GetCategoryEntryOffset(p->category);
    s32 count = ForgingBook_GetCategoryEntryCount(p->category);
    s32 i;
    u32 hidden = 1;

    struct FbFlags *fl = p->slot;
    for (i = 17; i >= 0; i--) {
        fl->flags |= hidden;
        fl++;
    }
    if (p->category <= 5) {
        for (i = 0; i < count; i++) {
            u8 *e;
            u8 *entries;
            s32 idx = offset + i;
            entries = p->entry;
            e = entries + idx;
            if (ForgingBook_IsWeaponDiscovered(*e))
                p->slot[i].flags &= ~1;
            Anim_LoadKeyframe(p->animBase + i * 0x60, p->base,
                              (u16)ForgingBook_GetEntryAnimation(*e), 0);
        }
        for (i = count; i <= 17; i++) {
            p->slot[i].flags &= ~1;
            Anim_LoadKeyframe(p->animBase + i * 0x60, p->base, 0x189, 0);
        }
    } else {
        s32 n;
        i = 0;
        n = count - 1;
        for (; i < n; i++) {
            u8 *e;
            u8 *entries;
            s32 idx = offset + i;
            entries = p->entry;
            e = entries + idx;
            if (ForgingBook_IsItemDiscovered(*e))
                p->slot[i].flags &= ~1;
            Anim_LoadKeyframe(p->animBase + i * 0x60, p->base,
                              (u16)ForgingBook_GetEntryAnimation(*e + 0x31), 0);
        }
        for (i = n; i <= 17; i++) {
            p->slot[i].flags &= ~1;
            Anim_LoadKeyframe(p->animBase + i * 0x60, p->base, 0x189, 0);
        }
    }
    return 0;
}
