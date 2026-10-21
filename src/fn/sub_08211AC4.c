#include "global.h"

#ifdef NONMATCHING
// fuzzy: 94.2%
// MATCHING_CONCESSION: register-pinning
// Remaining: p+0xb84 is folded into the index sum (original adds p+0xb84 after offset+i).
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
u32 ForgingBook_RefreshEntrySprites(struct FbBook *p)
{
    register s32 offset asm("sl") = ForgingBook_GetCategoryEntryOffset(p->category);
    register s32 count asm("sb") = ForgingBook_GetCategoryEntryCount(p->category);
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
            s32 idx = offset + i;
            asm("" : "+r"(idx));
            e = p->entry + idx;
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
        register s32 n asm("r8");
        i = 0;
        n = count - 1;
        for (; i < n; i++) {
            u8 *e;
            s32 idx = offset + i;
            asm("" : "+r"(idx));
            e = p->entry + idx;
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
#else
INCLUDE_ASM("asm/nonmatching", ForgingBook_RefreshEntrySprites);
#endif
