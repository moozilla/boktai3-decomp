#include "global.h"
/* Tentative family name: owner of the active child-record group. */
#define ChildGroup_UpdateLandingState sub_080F3F48
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
struct ChildGroup;
struct ChildGroup {
    u8 pad0[0x64];
    u16 h64;
    u8 pad66[0x2];
    u16 h68;
    u8 pad6A[0x2];
    u16 h6C;
    u8 pad6E[0x2];
    u16 h70;
    u8 pad72[0x1B];
    u8 flags;
    u8 pad8E[0x14];
    u8 reset;
    u8 padA3[0x5];
    struct CGKid kids[4];
    u8 pad158[0xA10];
    u32 fieldB68;
    u8 padB6C[0x2];
    u8 bB6E;
    u8 padB6F[0x15];
    struct CGPos pos;
    u8 padB8C[0x96];
    u16 hC22;
    u8 padC24[0x52];
    u16 hC76;
    u8 padC78[0x68];
    void (*callbackCE0)(struct ChildGroup *);
    u8 padCE4[0xC];
    void (*callbackCF0)(struct ChildGroup *);
    u8 padCF4[0xC];
    u32 fieldD00;
    u8 padD04[0x4F];
    u8 fieldD53;
};
s32 Terrain_GetHeightAt(void *);
void sub_080F2F24(struct ChildGroup *);
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
void ChildGroup_UpdateLandingState(struct ChildGroup *p)
{
    if (EnterState(p)) {
        int i;
        p->fieldB68 |= 1;
        p->hC22 |= 4;
        p->hC76 |= 4;
        for (i = 0; i < 4; i++) {
            p->kids[i].x = p->pos.x;
            p->kids[i].y = p->pos.y;
            p->kids[i].z = p->pos.z;
            p->kids[i].b6 = p->bB6E;
            p->kids[i].flags &= ~1;
            {
                u32 rec = (u32)p + i * 0x54;
                *(u16 *)(rec + 0x1CE) |= 4;
                *(u16 *)(rec + 0x31E) |= 4;
            }
            p->fieldD00 = (u16)Terrain_GetHeightAt(&p->pos);
        }
        p->h64 = 0;
        p->h68 = 0;
        p->h6C = -7;
        p->h70 = -7;
        p->callbackCE0 = sub_080F2F24;
        p->callbackCF0 = Ivy_InitColorTransition;
        p->fieldD53 = 0;
        Sound_PlaySong(0x16F);
    }
    if (p->callbackCE0)
        Callback_InvokeActor(p, p->callbackCE0);
}
