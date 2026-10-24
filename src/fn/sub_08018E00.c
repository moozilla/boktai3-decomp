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
/* Clear in reverse order and return the cursor after the final store. */
static inline u8 *BlendFade_ClearCoefficients(u8 *first)
{
    s32 zero = 0;
    u8 *cursor = first + 1;
    do {
        *cursor = zero;
        cursor--;
    } while ((s32)cursor >= (s32)first);
    first = cursor;
    return first;
}
// confidence=low: reads a blend fade from the script: mode 'm', start coefficients 'i', their
// change 'a', targets 's' and 'd' and duration 't' (default 60), then applies the start values.
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
    read_start: {
        s32 value = (s32)Script_GetPc();
        if (value)
            value = Script_GetValue();
        *s = value;
        s++;
    }
        if ((s32)s <= (s32)(start + 1))
            goto read_start;
    } else {
        start = p->start;
        delta = p->delta;
        s = BlendFade_ClearCoefficients(start);
    }
    if (Script_SeekToKeyword('a')) {
        i = 0;
        s = delta;
        do {
            u8 coefficient;
            s32 value = (s32)Script_GetPc();
            if (value)
                value = Script_GetValue() - start[i];
            coefficient = value;
            *s = coefficient;
            s++;
            i++;
        } while (i <= 1);
    } else {
        s = BlendFade_ClearCoefficients(delta);
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
