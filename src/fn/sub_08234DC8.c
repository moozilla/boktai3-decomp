#include "global.h"
extern u32 gUnk_0300523C;
extern s16 gUnk_086149C4[];
s32 Div(s32, s32);
void Sound_PlaySong(u32);
void MusicSlots_StopAllAndRetainResumable(void);
void HealthRestoreEffect_StartParticleBurst(u8 *);
void HealthRestoreEffect_UpdateShake(u8 *);
#define H(o) (*(u16 *)(p + (o)))
#define S(o) (*(s16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
static inline s32 scale12(s32 v)
{
    if (v >= 0)
        v = v >> 12;
    else
        v = -((-v) >> 12);
    return v;
}
// confidence=low: the 16-frame entry of the health restore effect. Stops the music, then slides
// the sprite from its start to its target position with a sine bob; afterwards it starts the
// shake.
#ifdef NONMATCHING
// fix.py best after the inlined-insn fix
// ROM keeps the incremented timer untruncated in a copy for the 0x50/0x51 bytes; u8 t adds a truncation and wider types fold +1+0x22.
void sub_08234DC8(u8 *p)
{
    u16 *timer;
    s32 y;
    u16 t;
    if ((*((u16 *)(p + 0x270))) == 0) {
        MusicSlots_StopAllAndRetainResumable();
        {
            s32 four = 4;
            gUnk_0300523C |= four;
        }
    } else if ((*((u16 *)(p + 0x270))) == 1) {
        Sound_PlaySong(0xE6);
        *((u32 *)(p + 0x24)) &= ~1;
    }
    timer = &(*((u16 *)(p + 0x270)));
    t = ++(*timer);
    if ((*timer) > 15) {
        *((u32 *)(p + 0x24)) &= ~2;
        HealthRestoreEffect_StartParticleBurst(p);
        *((void (**)(u8 *))(p + 0x274)) = HealthRestoreEffect_UpdateShake;
        *timer = 0;
        Sound_PlaySong(0x284);
        return;
    }
    {
        s32 v = t + 0x22;
        p[0x51] = v;
        p[0x50] = v;
    }
    *((u16 *)(p + 0x3C)) =
        Div(((*((s16 *)(p + 0x7C))) * (*timer)) + ((*((s16 *)(p + 0x84))) * (16 - (*timer))), 16);
    y = Div(((*((s16 *)(p + 0x7E))) * (*timer)) + ((*((s16 *)(p + 0x86))) * (16 - (*timer))), 16);
    *((u16 *)(p + 0x3E)) = y - scale12(gUnk_086149C4[Div((*timer) << 7, 20) & 0xFF] << 4);
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08234DC8);
#endif
