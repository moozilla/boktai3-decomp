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
struct ChildGroup;
struct ChildGroup {
    u8 pad0[0x8D];
    u8 flags;
    u8 pad8E[0x14];
    u8 reset;
    u8 padA3[0x5];
    struct CGKid kids[4];
    u8 pad158[0x348];
    s32 angle[4];
    s8 speed;
    u8 b4B1;
    u8 pad4B2[0x6];
    s32 radius;
    u8 pad4BC[0x694];
    u16 count;
    u8 padB52[0x1C];
    u8 bB6E;
    u8 padB6F[0x15];
    struct CGPos pos;
    u8 padB8C[0x150];
    void (*callbackCDC)(struct ChildGroup *);
    u8 padCE0[0x10];
    void (*callbackCF0)(struct ChildGroup *);
    u8 padCF4[0x5F];
    u8 fieldD53;
    u8 padD54[0x30];
    u16 timer;
};
extern s16 gUnk_086149C4[];
void Ivy_WaitForSecondaryActionCompletion(struct ChildGroup *);
void Ivy_InitColorTransition(struct ChildGroup *);
void Ivy_UpdateBobbing(struct ChildGroup *);
void Sound_PlaySong(u32);
s32 Div(s32, s32);
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
static inline s32 truncateShift(s32 value, s32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -(-value >> shift);
}
void ChildGroup_UpdateOrbitingMembers(struct ChildGroup *p)
{
    int i;
    if (EnterState(p)) {
        for (i = 0; i < p->count; i++) {
            p->kids[i].x = p->pos.x;
            p->kids[i].y = p->pos.y;
            p->kids[i].z = p->pos.z;
            p->kids[i].b6 = p->bB6E;
            p->kids[i].flags &= ~1;
            p->angle[i] = Div(0x100, p->count) * i;
        }
        p->radius = 0;
        p->b4B1 = 0;
        p->speed = 5;
        p->timer = 0;
        for (i = 0; i < p->count; i++) {
            u32 rec = (u32)p + i * 0x54;
            *(u16 *)(rec + 0x1CE) &= ~4;
            *(u16 *)(rec + 0x31E) &= ~4;
        }
        p->callbackCDC = Ivy_WaitForSecondaryActionCompletion;
        p->callbackCF0 = Ivy_InitColorTransition;
        p->fieldD53 = 0;
        Sound_PlaySong(0x16F);
    }
    Ivy_UpdateBobbing(p);
    i = 0;
    if (i < p->count) {
        s32 *radius = &p->radius;
        const s16 *cosT = gUnk_086149C4;
        const s16 *sinT = cosT;
        s32 *a = p->angle;
        do {
            p->kids[i].x = p->pos.x + truncateShift(cosT[(*a + 64) & 255] * *radius, 10);
            p->kids[i].y = p->pos.y;
            p->kids[i].z = p->pos.z + truncateShift(sinT[*a & 255] * *radius, 10);
            *a += p->speed;
            if (*a > 255) {
                if (p->timer > 0x46)
                    Sound_PlaySong(0x169);
                *a -= 255;
            }
            a++;
            i++;
        } while (i < p->count);
    }
    if (p->callbackCDC)
        Callback_InvokeActor(p, p->callbackCDC);
    p->timer++;
}
