#include "global.h"

#define P8(o)  (*(u8 *)((u8 *)p + (o)))
#define P16(o) (*(u16 *)((u8 *)p + (o)))
#define P32(o) (*(u32 *)((u8 *)p + (o)))

void CpuSet(const void *src, void *dst, u32 control);
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, s32, const void *);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void *ScriptProc_Start(u16, u16);
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
extern u8 *gUnk_030042E4;

#define WI(w, par, id, type, a, b, pos)                                                            \
    Widget_InitLinked((w), (par), (id), (type), (a), (b), 60, (pos))

struct sub_08176F70_Anim {
    u8 *cmds;
    u8 flags;
    u8 count;
    u8 f6;
    u8 dur;
    u16 idx;
    u16 pad;
    u16 speed;
    u16 elapsed;
};

struct sub_08176F70_Sprite {
    u32 flags;
    u8 pad[12];
    u16 frame;
};

// inlined animation step (same shape as Anim_Step)
static inline void sub_08176F70_Step(struct sub_08176F70_Anim *a, struct sub_08176F70_Sprite *s)
{
    u16 *cmd = (u16 *)(a->cmds + a->idx * 2);
    u32 r0;
    s->frame = *cmd >> 6;
    if ((a->flags & 1) != (((*cmd & 0x30) >> 4) & 1))
        s->flags |= 4;
    else
        s->flags &= ~4;
    if ((u8)(a->flags & 2) != (((*cmd & 0x30) >> 4) & 2))
        s->flags |= 8;
    else
        s->flags &= ~8;
    a->elapsed++;
    if (a->elapsed >= a->dur) {
        a->elapsed = 0;
        if (a->flags & 4) {
            if (a->idx == 0) {
                if (a->flags & 8) {
                    a->elapsed = a->dur;
                    goto after;
                }
                r0 = a->count;
            } else {
                r0 = a->idx;
            }
            a->idx = r0 - 1;
        } else {
            a->idx++;
            if (a->idx >= a->count) {
                if (a->flags & 8) {
                    a->elapsed = a->dur;
                    a->idx = a->count - 1;
                } else {
                    a->idx = 0;
                }
            }
        }
    after:
        cmd = (u16 *)(a->cmds + a->idx * 2);
        a->f6 = *cmd & 0xF;
        a->dur = (a->speed * a->f6) >> 6;
        if (a->dur == 0)
            a->dur = 1;
    }
}

#ifdef NONMATCHING
// fuzzy: 39.1%
// scope shape 698 (differences once low registers are set aside)
// confidence=low: UI screen init: sets up ~150 linked widgets (rows, columns, icons) and a sprite
void sub_08176F70(u8 *p)
{
    s16 v[4];
    u32 zero = 0;
    u8 *A = p + 0x64;
    u8 *B = p + 0xA4;
    u8 *C = p + 0x84;
    u8 *D = p + 0xC4;
    u8 *w, *w2, *w3, *w4, *w5;
    s32 i;
    struct sub_08176F70_Anim *anim;

    CpuSet(&zero, v, 0x05000002);
    WI(p + 0x1420, A, 0x51, 0xB0, 0, 0, v);
    WI(p + 0x1480, A, 0x43, 0xB1, 0, 0, v);
    WI(p + 0x14E0, A, 0x44, 0xB1, 0, 0, v);
    WI(p + 0x1720, A, 0x63, 0xB1, 0, 0, v);
    WI(p + 0x1780, A, 0x70, 0xB1, 0, 0, v);
    WI(p + 0x1540, A, 0x40, 0xB1, 0, 0, v);
    WI(p + 0x15A0, A, 0x4D, 0xB1, 0, 0, v);
    WI(p + 0x1660, A, 0x4E, 0xB1, 0, 0, v);
    w = p + 0x2860;
    WI(w, A, 0x4D, 0xB1, 0, 0, v);
    Anim_LoadKeyframe(w, A, 0x4D, 1);
    WI(p + 0x1600, A, 0x4D, 0xB1, 0, 0, v);
    WI(p + 0x3040, A, 0x7F, 0xB1, 0, 0, v);
    WI(p + 0x30A0, A, 0x7F, 0xB1, 0, 0, v);

    w = p + 0x17E0;
    for (i = 0x13; i >= 0; i--) {
        WI(w, B, 0x68, 0xB1, 0, 0, v);
        w += 0x60;
    }
    w = p + 0x1F60;
    for (i = 0xF; i >= 0; i--) {
        WI(w, A, 0x50, 0xB1, 0, 0, v);
        w += 0x60;
    }
    w = p + 0x26E0;
    w2 = p + 0x28C0;
    for (i = 3; i >= 0; i--) {
        WI(w, B, 0x68, 0xB1, 0, 0, v);
        WI(w2, A, 0x50, 0xB1, 0, 0, v);
        w2 += 0x60;
        w += 0x60;
    }
    v[0] = 0xD8;
    v[1] = 0x20;
    WI(p + 0x2560, B, 0x193, 0xB1, 0, 0, v);
    v[0] = 0xD8;
    v[1] = 0x38;
    WI(p + 0x25C0, B, 0x192, 0xB1, 0, 0, v);
    v[0] = 0xD8;
    v[1] = 0x50;
    WI(p + 0x2620, B, 0x191, 0xB1, 0, 0, v);
    v[0] = 0xD8;
    v[1] = 0x68;
    WI(p + 0x2680, B, 0x190, 0xB1, 0, 0, v);
    v[0] = 0;
    v[1] = 0x80;
    WI(p + 0x2A40, B, 0xCF, 0xB1, 0, 0, v);

    w = p + 0x2AA0;
    w2 = p + 0x41E0;
    for (i = 1; i >= 0; i--) {
        WI(w, B, 0x68, 0xB1, 0, 0, v);
        WI(w2, A, 0x50, 0xB1, 0, 0, v);
        w2 += 0x60;
        w += 0x60;
    }

    v[0] = 0x34;
    v[1] = 0x64;
    WI(p + 0x2FE0, p + 0x104, 0, 0x94, 0, 0, v);
    Anim_SetKeyframes(p + 0x2FE0, p + 0x104, 0xD, 1, 0);
    v[0] = 0;
    v[1] = 0;

    w = p + 0x2B60;
    for (i = 0xB; i >= 0; i--) {
        WI(w, A, 0xA, 0xB1, 0, 0, v);
        w += 0x60;
    }

    v[0] = 0;
    v[1] = 0;
    WI(p + 0x16C0, A, 0x4B, 0x91, 0, 0, v);
    P32(0x1DC) = (u32)ScriptProc_Start(0x922E, 0x4BBF);
    AnimData_Load(p + 0x1B0, 0x593A);
    Sprite_Init(p + 0x184, p + 0x1B0, 0xD1);
    P8(0x18B) = 0;
    anim = (struct sub_08176F70_Anim *)(p + 0x1CC);
    Anim_SetSequence(anim, (void *)P32(0x1DC), 0, 0, 0);
    sub_08176F70_Step(anim, (struct sub_08176F70_Sprite *)(p + 0x184));

    P16(0x1A0) = 0;
    P16(0x1A2) = 0;
    P16(0x1A4) = 0;
    v[0] = 0;
    v[1] = 0;
    WI(p + 0x3100, A, 0x62, 0x91, 0, 0, v);
    v[0] = 0;
    v[1] = 0;
    WI(p + 0x3160, A, 0x77, 0x91, 0, 0, v);
    v[0] = 0;
    v[1] = 0;
    w = p + 0x31C0;
    for (i = 3; i >= 0; i--) {
        WI(w, A, 0x5B, 0x91, 0, 0, v);
        v[1] += 0xD;
        w += 0x60;
    }
    v[0] = 0;
    v[1] = 0;
    WI(p + 0x3340, A, 0x7D, 0x91, 0, 0, v);
    WI(p + 0x33A0, A, 0x7E, 0x91, 0, 0, v);
    v[0] = 0;
    v[1] = 0;

    w = p + 0x3400;
    w2 = p + 0x3520;
    w3 = p + 0x3640;
    w4 = p + 0x1FE0;
    w5 = p + 0x2100;
    for (i = 2; i >= 0; i--) {
        v[2] = 0;
        WI(w, A, 0x79, 0x91, 1, 0, v);
        *(w4 + 0x143A) = 3;
        v[2] -= 8;
        WI(w2, A, 0x7A, 0x91, 1, 0, v);
        *(w5 + 0x143A) = 3;
        v[2] -= 8;
        WI(w3, A, 0x7B, 0x91, 1, 0, v);
        *(p + 0x2220 + (2 - i) * 0x60 + 0x143A) = 3;
        v[1] += 8;
        w += 0x60;
        w2 += 0x60;
        w3 += 0x60;
        w4 += 0x60;
        w5 += 0x60;
    }

    v[0] = 0;
    v[1] = 0;
    w = p + 0x38E0;
    for (i = 0; i <= 3; i++) {
        WI(w, C, 0x40 + i * 0x20, 0x91, 1, 0, v);
        Anim_SetKeyframes(w, C, i, 1, 0);
        w += 0x60;
    }
    P16(0x39C0) += 0x10;
    WI(p + 0x3820, C, 0, 0x91, 1, 0, v);
    WI(p + 0x3880, C, 1, 0x91, 1, 0, v);

    v[0] = 0;
    v[1] = 0;
    WI(p + 0x3A60, A, 0x75, 0xB0, 0, 0, v);
    WI(p + 0x3AC0, A, 0x76, 0xB0, 0, 0, v);
    WI(p + 0x3B20, A, 0x73, 0xB0, 0, 0, v);
    WI(p + 0x3B80, A, 0x74, 0xB0, 0, 0, v);
    v[0] = 0;
    v[1] = 0;
    WI(p + 0x3760, A, 0x45, 0xB1, 0, 0, v);
    WI(p + 0x37C0, A, 0x46, 0xB1, 0, 0, v);
    v[0] = 0;
    v[1] = 0;
    WI(p + 0x3BE0, D, 4, 0xB0, 0, 0, v);
    WI(p + 0x3C40, D, 0x19, 0xB0, 0, 0, v);
    WI(p + 0x3CA0, D, 0x1B, 0xB0, 0, 0, v);
    v[0] = 0;
    v[1] = 0;

    w = p + 0x3D60;
    for (i = 2; i >= 0; i--) {
        WI(w, D, 0, 0xB0, 0, 0, v);
        v[0] += 8;
        w += 0x60;
    }
    v[0] = 0x10;
    v[1] = -4;
    WI(p + 0x3D00, D, 0x12, 0xB0, 0, 0, v);
    v[0] = -0x38;
    v[1] = 0x38;
    w = p + 0x3E80;
    for (i = 3; i >= 0; i--) {
        WI(w, D, 0, 0xB0, 0, 0, v);
        v[0] += 8;
        w += 0x60;
    }
    v[0] = 0x58;
    v[1] = 0x38;
    WI(p + 0x4000, D, 0, 0xB0, 0, 0, v);
    v[0] = -0x30;
    v[1] = 0x48;
    w = p + 0x4060;
    for (i = 3; i >= 0; i--) {
        WI(w, D, 0, 0xB0, 0, 0, v);
        v[0] += 8;
        w += 0x60;
    }

    w = p + 0xC54;
    AnimData_Load(w, 0xA47C);
    Sprite_Init(p + 0xC28, w, 0x91);
    AnimData_SelectPaletteBank(w, 0x1B4);
    P8(0xC2F) = 0;
    P16(0xC44) = 0xA0;
    P16(0xC46) = 0x58;
    P16(0xC48) = 0;
    w = p + 0x164;
    Widget_InitLinked(p + 0x4304, w, 2, 0x91, 0, 0, 0, 0);
    Widget_InitLinked(p + 0x42A4, w, 0, 0x91, 0, 0, 0, 0);
    CpuSet(gUnk_030042E4 + 0x7540, p + 0x43B4, 0x04000008);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08176F70);
#endif
