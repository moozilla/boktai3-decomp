#include "global.h"
// MATCHING_CONCESSION: register-pinning
// Provisional: SolarBank rate display setup, including rate digits,
// widget rows and two palette-selected sprites.
struct Res {
    u32 words[8];
};
struct Wgt {
    u8 b[0x60];
};
struct BankSprite {
    u32 flags;
    u8 pad4[3];
    u8 b7;
    u8 pad8[0x14];
    u16 x, y, z;
    u8 pad22[0xA];
};
struct BankAnim {
    u8 b[0x1C];
};
struct SolarBankDisplay {
    u8 pad0[0x18];
    struct Res resA;
    struct Res resB;
    struct Wgt w58;
    struct Wgt wB8;
    struct Wgt w118;
    struct Wgt w178;
    struct Wgt w1D8;
    struct Wgt w238[4];
    struct Wgt w3B8[4];
    struct Wgt w538;
    struct Wgt w598;
    struct Wgt w5F8;
    struct BankSprite sprites[2];
    struct BankAnim anims[2];
};
struct Pos {
    s16 x, y, z, w;
};
extern u8 *gUnk_02000710;
static inline void Pos_Set(struct Pos *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}
void *ScriptProc_Start(u16, u16);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, u32);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 SunClock_GetSecond(void);
s32 Mod(s32, s32);
s32 Div(s32, s32);

void sub_081BBE88(struct SolarBankDisplay *p)
{
    struct Res *res;
    struct Res *resB;
    struct Res *resA;
    struct Pos pos;
    s32 v;
    s32 tens;
    s32 i;
    // Keep the widget coordinate zero separate from the packed position reset.
    register s32 zero asm("r1");
    res = ScriptProc_Start(0xCB05, 0x32BA);
    p->resA = *res;
    ResourceHeader_Relocate(&p->resA, res);
    res = ScriptProc_Start(0xCB05, 0x530D);
    p->resB = *res;
    ResourceHeader_Relocate(&p->resB, res);
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    zero = 0;
    Widget_InitLinked(&p->w58, &p->resA, 0, 0x30, zero, zero, 0x3C, &pos);
    if (Script_SeekToKeyword(0x52)) {
        s32 rate = Script_GetValue();
        if (rate < 0)
            rate = 0;
        if (rate > 13)
            rate = 13;
        *((s16 *)(gUnk_02000710 + 0x598)) = rate + 0x41;
    } else if ((*((s16 *)(gUnk_02000710 + 0x598))) == 0) {
        s32 sec = SunClock_GetSecond();
        *((s16 *)(gUnk_02000710 + 0x598)) =
            Mod((*((u32 *)(gUnk_02000710 + 0x614))) ^ sec, 13) + 0x41;
    }
    v = ((100 * (*((s16 *)(gUnk_02000710 + 0x598)))) >> 6) - 100;
    tens = Div(v, 10);
    v -= tens * 10;
    Widget_InitLinked(&p->w118, &p->resA, (u16)(tens + 8), 0x30, 0, 0, 0x3C, &pos);
    pos.x += 8;
    Widget_InitLinked(&p->w178, &p->resA, (u16)(v + 8), 0x30, 0, 0, 0x3C, &pos);
    pos.x = 0;
    Widget_InitLinked(&p->w1D8, &p->resA, 0x12, 0x30, 0, 0, 0x3C, &pos);
    pos.x = -8;
    pos.y = -8;
    Widget_InitLinked(&p->wB8, &p->resB, 0x4B, 0x10, 0, 0, 0x3C, &pos);
    *((void **)(((u8 *)p) + 0x100)) = ((u8 *)p) + 0x710;
    pos.x = 0x28;
    pos.y = 0x28;
    resA = &p->resA;
    resB = &p->resB;
    for (i = 0; i < 4; i++) {
        struct Pos *position = &pos;
        struct Wgt *widget = &p->w238[i];
        Widget_InitLinked(widget, resB, 0, 0x30, 0, 0, 0x3C, position);
        pos.x += 8;
    }

    pos.x = 0x88;
    pos.y = 0x28;
    for (i = 0; i < 4; i++) {
        struct Pos *position = &pos;
        struct Wgt *widget = &p->w3B8[i];
        Widget_InitLinked(widget, resB, 0, 0x30, 0, 0, 0x3C, position);
        pos.x += 8;
    }

    for (i = 0; i < 2; i++) {
        AnimData_Load(&p->anims[i], 0xA47C);
        Sprite_Init(&p->sprites[i], &p->anims[i], 0);
        p->sprites[i].b7 = 0;
        p->sprites[i].flags = 0x30;
        if (i == 0)
            p->sprites[0].flags |= 4;
        if (i == 1)
            AnimData_SelectPaletteBank(&p->anims[1], 0x1B1);
        p->sprites[i].x = 0x38 + (i * 0x60);
        p->sprites[i].y = 0x58;
        p->sprites[i].z = 0;
    }

    Widget_InitLinked(&p->w538, resA, 0x21, 0x30, 0, 0, 0x3C, (void *)0);
    Pos_Set(&pos, -0x85, -0x18, 0);
    Widget_InitLinked(&p->w598, resB, 0x7E, 0x30, 0, 0, 0x3C, &pos);
    Widget_InitLinked(&p->w5F8, resB, 0x7F, 0x30, 0, 0, 0x3C, &pos);
}
