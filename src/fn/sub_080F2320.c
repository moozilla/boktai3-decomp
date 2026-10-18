#define ColorPulse_ApplyContact   sub_080F2320
#define ColorPulse_UpdateHitFlash sub_080F6414
#include "global.h"
struct ColorPulseContact {
    u8 pad0[0xa];
    u16 kind;
    u8 padC[0x28];
    u32 source, flags;
    u8 pad3C[2];
    u16 damage;
    u8 pad40[3];
    u8 effect;
    u16 value;
};
struct ColorPulse {
    u8 pad0[0x64];
    u16 velocityX;
    u8 pad66[2];
    u16 velocityZ;
    u8 pad6A[0xa];
    u16 impulseX;
    u8 pad76[2];
    u16 impulseZ;
    u8 pad7A[0xa];
    s16 hitValue;
    u8 pad86[0xd];
    u8 mode;
    u8 pad94[8];
    u16 colliderState;
    u8 pad9E[0xb74 - 0x9e];
    void *animation;
    u8 padB78[0xcb4 - 0xb78];
    u16 disabled;
    u8 padCB6[0x12];
    void (*callback)(void);
    u8 padCCC[0x20];
    void (*hitCallback)(void);
    u8 padCF0[0x68];
    u16 palette;
    u8 padD5A[6];
    u32 hitSource, hitFlags, hitKind;
    u8 padD6C[0xc];
    u16 damage, effect;
};
void sub_080F421C(void);
void sub_080F6414(void);
void sub_0821FF8C(void *, void *);
void sub_08159860(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, u32);
void Sound_PlaySong(u32);
void sub_080F2320(struct ColorPulseContact *a, struct ColorPulseContact *b, struct ColorPulse *p)
{
    if (p->callback == sub_080F421C)
        return;
    if (p->disabled) {
        if (p->effect) {
            b->damage = 0;
            b->effect = 0;
        }
    } else {
        sub_0821FF8C(a, b);
        b->value = a->value;
        if (!p->hitCallback) {
            u32 damage = b->damage;
            u32 flags = a->flags;
            if (damage || (flags & 0x40)) {
                u32 source = a->source;
                p->hitSource = source;
                p->hitFlags = flags;
                p->hitKind = a->kind;
                p->colliderState = 8;
                p->hitCallback = sub_080F6414;
                AnimData_SelectPaletteBank(p->animation, p->palette);
                sub_08159860(a, b, p->hitValue);
            }
        }
        if (b->damage)
            Sound_PlaySong(318);
        else
            Sound_PlaySong(400);
        {
            u16 *velocity = &p->velocityX;
            if (p->mode == 1) {
                p->impulseX = -*velocity * 4;
                p->impulseZ = -p->velocityZ * 4;
            }
        }
        p->damage = b->damage;
        p->effect = b->effect;
    }
}
