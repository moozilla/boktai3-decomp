#include "global.h"
#include "engine.h"

struct CGPos080F38E0 {
    u16 x, y, z, w;
};
struct CGKid080F38E0 {
    u32 flags;
    u8 pad4[2];
    u8 b6;
    u8 pad7[0x25];
};
struct CGSrc080F38E0 {
    u8 pad0[0x30];
    u16 h30, h32, h34;
};
struct Group080F38E0 {
    u8 pad0[0x60];
    struct CGSrc080F38E0 *src60;
    u8 pad64[0x29];
    u8 flags;
    u8 pad8E[0x14];
    u8 reset;
    u8 padA3[5];
    struct CGKid080F38E0 kids[4];
    u8 pad158[0x330];
    u16 f488[3];
    u8 pad48E[2];
    u16 f490[3];
    u8 pad496[2];
    u16 f498[3];
    u8 pad49E[0x12];
    u8 b4B0;
    u8 pad4B1[3];
    u16 h4B4;
    u8 pad4B6[0x6B8];
    u8 bB6E;
    u8 padB6F[0x15];
    struct CGPos080F38E0 pos;
    u8 padB8C[0x13C];
    void (*callbackCC8)(struct Group080F38E0 *);
    u8 padCCC[0xC];
    void (*callbackCD8)(struct Group080F38E0 *);
    u8 padCDC[0x14];
    void (*callbackCF0)(struct Group080F38E0 *);
    u8 padCF4[8];
    u32 fieldCFC;
    u8 padD00[0x53];
    u8 fieldD53;
    u8 padD54[0x44];
    struct CGPos080F38E0 saved;
    s32 counterDA0;
};
void Ivy_InitColorTransition(struct Group080F38E0 *);
s32 Callback_InvokeActor(void *, void *);
extern const u16 gUnk_0203B400[];

static inline u8 ChildGroup_TakeEntryFlag(struct Group080F38E0 *p)
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

static inline u32 ChildGroup_Jitter(u32 value, const u16 *base)
{
    u32 mask = 31;
    value &= mask;
    return value + *base - 15;
}

static inline u32 ChildGroup_RandomValue(void)
{
    const u16 *table = gUnk_0203B400;
    u32 *index = &gRandomIndex;
    *index = (*index + 1) & 1023;
    return table[*index];
}

// scope shape 52 (differences once low registers are set aside)
#define ChildGroup_UpdateTransitionJitter sub_080F38E0
void ChildGroup_UpdateTransitionJitter(struct Group080F38E0 *p)
{
    s32 i;

    // confidence=low: per-frame update of the child group: resets kid flags, snapshots
    // position, cycles the random table into the position, then runs the callback.
    if (ChildGroup_TakeEntryFlag(p)) {
        p->f498[0] = p->pos.x;
        p->f498[1] = p->pos.y;
        p->f498[2] = p->pos.z;
        p->f488[0] = p->src60->h30;
        p->f488[1] = 0x15E + p->src60->h32;
        p->f488[2] = p->src60->h34;
        p->f490[0] = p->f488[0] - p->pos.x;
        p->f490[1] = p->f488[1] - p->pos.y;
        p->f490[2] = p->f488[2] - p->pos.z;
        for (i = 0; i < 4; i++) {
            p->kids[i].b6 = 0x80;
            p->kids[i].flags |= 1;
        }
        p->b4B0 = 0;
        p->h4B4 = 0;
        p->saved = p->pos;
        p->callbackCF0 = Ivy_InitColorTransition;
        p->fieldD53 = 0;
    }
    if (p->counterDA0 > 0) {
        p->pos.x = ChildGroup_Jitter(ChildGroup_RandomValue(), &p->saved.x);
        p->pos.y = ChildGroup_Jitter(ChildGroup_RandomValue(), &p->saved.y);
        p->pos.z = ChildGroup_Jitter(ChildGroup_RandomValue(), &p->saved.z);
        p->counterDA0--;
    }
    p->fieldCFC = 0;
    if (p->callbackCF0 != 0) {
        Callback_InvokeActor(p, p->callbackCF0);
    }
}
