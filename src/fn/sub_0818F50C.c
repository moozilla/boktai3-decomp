#include "global.h"
#define EffectPool_CreateTimedEffect sub_0818F50C
#define EffectPool_AllocateSlot sub_0818F218
struct EffectPosition {
    u32 a, b;
};
struct EffectPoolSlot {
    void (*f0)(void);
    void (*f1)(void);
    u8 g[0x38];
    struct EffectPosition v;
    u8 unknown48[0x20];
    u8 lifetime;
};
struct EffectPoolSlot *sub_0818F218(void *);
void AnimData_Load(void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void Sprite_Init(void *, void *, s32);
void Anim_RestartSequence(void *, s32, s32, s32, s32);
void sub_0818F0FC(void);
void Nidhoggr_DestroySpriteEffect(void);
void sub_0818F50C(u8 *p, struct EffectPosition *q)
{
    struct EffectPoolSlot *o = sub_0818F218(p);
    if (o != 0) {
        u8 *r = (u8 *)o + 8;
        void (*a)(void);
        void (*b)(void);
        AnimData_Load(r, 0xC7ED);
        AnimData_SelectPaletteBank(r, 0x1AA);
        Sprite_Init((u8 *)o + 0x24, r, 0);
        Anim_RestartSequence((u8 *)o + 0x50, *(s32 *)(p + 0x53AC), 6, 0, 0);
        o->v = *q;
        a = sub_0818F0FC;
        b = Nidhoggr_DestroySpriteEffect;
        o->f0 = a;
        o->f1 = b;
        o->lifetime = 100;
    }
}
