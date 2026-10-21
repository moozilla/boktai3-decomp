#include "global.h"
/* Tentative family name: owner of the active child-record group. */
struct CGPos {
    u16 x, y, z, w;
};
struct CGKid {
    u32 flags;
    u8 pad4[2];
    u8 b6;
    u8 pad7[0x15];
    u16 x, y, z;
    u8 pad[0xA];
};
struct ChildGroup {
    u8 pad0[0x8D];
    u8 flags;
    u8 pad8E[0x14];
    u8 reset;
    u8 padA3[5];
    struct CGKid kids[4];
    u8 pad158[0x358];
    u8 b4B0;
    u8 pad4B1[3];
    u16 h4B4;
    u8 pad4B6[0x6B8];
    u8 bB6E;
    u8 padB6F[0x15];
    struct CGPos pos;
    u8 padB8C[0x13C];
    void (*callbackCC8)(struct ChildGroup *);
    u8 padCCC[0xC];
    void (*callbackCD8)(struct ChildGroup *);
    u8 padCDC[0x14];
    void (*callbackCF0)(struct ChildGroup *);
    u8 padCF4[0x5C];
    u32 fieldD50;
    u8 padD54[6];
    u16 hD5A;
    u8 padD5C[0x24];
    u32 fieldD80;
};
void Ivy_WaitForPrimaryActionCompletion(struct ChildGroup *);
void Ivy_InitColorTransition(struct ChildGroup *);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
static inline u8 EnterState(struct ChildGroup *p)
{
    u32 mask = 1;
    u32 flags = p->flags;
    if (flags & mask) {
        u32 cleared = ~1;
        cleared &= flags;
        {
            u32 zero = 0;
            p->flags = cleared;
            p->reset = zero;
        }
        return 1;
    }
    return 0;
}
#ifdef NONMATCHING
// fuzzy: 99.4%
// Differs only in where `movs r2, #3` (the loop counter) is placed: the ROM sets it before the kid-0 pointers.
void ChildGroup_UpdateTrailingMembers(struct ChildGroup *p)
{
    if (EnterState(p)) {
        int i;
        for (i = 0; i < 4; i++) {
            p->kids[i].x = p->pos.x;
            p->kids[i].y = p->pos.y;
            p->kids[i].z = p->pos.z;
            p->kids[i].b6 = p->bB6E;
        }
        for (i = 0; i < 4; i++)
            p->kids[i].flags &= ~1;
        p->callbackCD8 = Ivy_WaitForPrimaryActionCompletion;
        p->h4B4 = 0;
        p->b4B0 = 0;
        p->hD5A = 0;
        p->callbackCF0 = Ivy_InitColorTransition;
        *((u8 *)p + 0xD53) = 0;
        Sound_PlaySong(0x16F);
    }
    if (p->callbackCD8)
        Callback_InvokeActor(p, p->callbackCD8);
    if (p->fieldD80 & 1) {
        int i;
        u16 *x0 = &p->kids[0].x;
        u16 *y0 = &p->kids[0].y;
        u16 *z0 = &p->kids[0].z;
        u8 *b0 = &p->kids[0].b6;
        for (i = 3; i > 0; i--) {
            p->kids[i].x = p->kids[i - 1].x;
            p->kids[i].y = p->kids[i - 1].y;
            p->kids[i].z = p->kids[i - 1].z;
            p->kids[i].b6 = p->kids[i - 1].b6;
        }
        *x0 = p->pos.x;
        *y0 = p->pos.y;
        *z0 = p->pos.z;
        *b0 = p->bB6E;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", ChildGroup_UpdateTrailingMembers);
#endif
