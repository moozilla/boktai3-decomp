#include "global.h"
#include "engine.h"
#include "terrain_cell_probe.h"

// confidence=low: advances an actor's impact animation and cue sounds, then spawns hazards at a jittered point of its impact cell.
typedef struct Actor08025FFC {
    u8 pad0[0x9c];
    u32 flags;
    u8 padA0[0xf];
    u8 signalAF;
    u8 padB0;
    u8 changed;
    u8 padB2[0x52];
    void *callbackA;
    void *callbackB;
    u8 *q;
    u32 *copyFrom;
    u8 pad114[0x149];
    u8 field25D;
    u8 pad25E[0x10c];
    s16 field36A;
    u8 pad36C[0x40];
    u32 mode;
} Actor08025FFC;

typedef struct Map08025FFC {
    u8 pad0[4];
    u8 *cells;
    u8 pad8[0x1c];
    u16 rows[1];
} Map08025FFC;

static inline s32 s08025FFC_Trunc12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}

#ifdef NONMATCHING
// fuzzy: 61.1%
// scope shape 78 (differences once low registers are set aside)
// scope shape 99 (differences once low registers are set aside)
void sub_08025FFC(Actor08025FFC *p, s32 arg)
{
    u32 copy[2];
    u16 d[4];
    u16 *s = (u16 *)copy;
    u8 *cd;
    s32 cell;
    s32 t;
    u32 bits;
    u32 idx;
    u16 a;
    u16 b;
    s32 base;
    s32 nb;
    s32 cx;
    s32 cy;
    Map08025FFC *map;
    u32 *src;
    s32 f;
    s32 kind;

    if (p->changed) {
        s32 seq;
        p->changed = 0;
        seq = (p->mode & 1) ? 0x1a : 0x40;
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, seq, 2, 1, 4);
        Sound_PlaySongChecked(0x35A);
    }
    if ((u8)Callback_InvokeActor(p, p->callbackA))
        return;
    p->flags |= 0x40;
    if ((u8)Callback_InvokeActor(p, p->callbackB))
        return;
    if (arg == 0x10)
        Sound_PlaySongChecked(0x351);
    f = arg & 0xf;
    if (f == 0xf) {
        src = p->copyFrom;
        copy[0] = src[0];
        copy[1] = src[1];
        base = p->q[0x2d];
        idx = (gUnk_03005308 + 1) & 0x3ff;
        a = gUnk_0203B400[idx] & f;
        base = base + a;
        base -= 8;
        idx = (idx + 1) & 0x3ff;
        gUnk_03005308 = idx;
        b = gUnk_0203B400[idx] & 0x7f;
        nb = b + 0x20;
        d[0] = s08025FFC_Trunc12(gUnk_086149C4[(base + 0x40) & 0xff] * nb);
        d[1] = 0;
        base &= 0xff;
        d[2] = s08025FFC_Trunc12(gUnk_086149C4[base] * nb);
        d[0] += s[0];
        d[1] += s[1];
        d[2] += s[2];
        Pos16_SnapToTerrain((struct Pos16 *)d);
        if (d[1] == s[1]) {
            AnimatedHazard_Create(d, p->field36A, 0x3c);
        } else {
            AnimatedHazard_Create(s, p->field36A, 0x3c);
        }
    }
    if (p->field25D & 2)
        p->signalAF = 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08025FFC);
#endif
