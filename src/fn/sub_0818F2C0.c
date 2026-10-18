#include "global.h"
struct V {
    u32 a, b;
};
struct O {
    void (*f0)(void);
    void (*f1)(void);
    u8 g[0x38];
    struct V v;
};
struct O *EffectPool_AllocateSlot(void *);
void AnimData_Load(void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void Sprite_Init(void *, void *, s32);
void Anim_RestartSequence(void *, s32, s32, s32, s32);
void sub_0818EE38(void);
void Nidhoggr_DestroySpriteEffect(void);
void Nidhoggr_CreatePalette424Effect(u8 *p, struct V *q)
{
    struct O *o = EffectPool_AllocateSlot(p);
    if (o != 0) {
        u8 *r = (u8 *)o + 8;
        void (*a)(void);
        void (*b)(void);
        AnimData_Load(r, 0xC7ED);
        AnimData_SelectPaletteBank(r, 0x1A8);
        Sprite_Init((u8 *)o + 0x24, r, 0);
        Anim_RestartSequence((u8 *)o + 0x50, *(s32 *)(p + 0x53AC), 1, 0, 0);
        o->v = *q;
        a = sub_0818EE38;
        b = Nidhoggr_DestroySpriteEffect;
        o->f0 = a;
        o->f1 = b;
    }
}
