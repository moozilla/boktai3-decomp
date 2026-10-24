#include "global.h"
#include "engine.h"
struct BlendFade {
    u8 task[0x18];
    u8 mode;
    s8 start[2];
    s8 delta[2];
    u8 pad1D[3];
    u16 firstTarget, secondTarget;
    s32 timer;
    s32 duration;
};
void Video_SetBlendControl(u32, u32, u32);
void Video_SetBlendAlpha(s32, s32);
void Video_SetBlendBrightness(s32);
// confidence=low: reads a blend fade from the script: mode 'm', start coefficients 'i', their
// change 'a', targets 's' and 'd' and duration 't' (default 60), then applies the start values.
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Wrapping the first start = p->start in a do-while(0) reaches 86% (loop weight changes the allocation). The zero-fill loops still differ: the ROM keeps the bound and the 0 in low registers.
s32 sub_08018E00(struct BlendFade *p)
{
    u8 *start;
    u8 *delta;
    u8 *s;
    s32 i;
    p->mode = Script_GetKeywordValue('m', 1);
    if (Script_SeekToKeyword('i')) {
        start = p->start;
        delta = p->delta;
        s = start;
        do {
            s32 value = (s32)Script_GetPc();
            if (value)
                value = Script_GetValue();
            *s = value;
            s++;
        } while (((s32)s) <= ((s32)(start + 1)));
    } else {
        start = p->start;
        delta = p->delta;
        s = &p->start[1];
        do {
            *s = 0;
            s--;
        } while (((s32)s) >= ((s32)start));
    }
    if (Script_SeekToKeyword('a')) {
        i = 0;
        s = delta;
        do {
            s32 value = (s32)Script_GetPc();
            if (value)
                value = Script_GetValue() - start[i];
            *s = value;
            s++;
            i++;
        } while (i <= 1);
    } else {
        do {
            *s = 0;
            s--;
        } while (((s32)s) >= ((s32)delta));
        s = delta + 1;
    }
    p->firstTarget = Script_GetKeywordValue('s', 0);
    p->secondTarget = Script_GetKeywordValue('d', 0);
    p->duration = Script_GetKeywordValue('t', 60);
    p->timer = 0;
    if (p->duration == 0)
        p->duration = 60;
    Video_SetBlendControl(p->mode, p->firstTarget, p->secondTarget);
    if (p->mode == 1)
        Video_SetBlendAlpha(p->start[0], p->start[1]);
    else
        Video_SetBlendBrightness(p->start[0]);
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08018E00);
#endif
