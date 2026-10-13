#include "global.h"
// MATCHING_CONCESSION: register-pinning
typedef struct Screen Screen;
struct Screen {
    u8 task[32];
    u16 *buffer, *source;
    u16 dirty;
    u8 pad2a[0x284];
    u16 reveal, revealStep, top, bottom;
    u8 pad2b6[8];
    u16 revealDuration, closeDelay, start, end, blend, fade, fadeStep;
    void (*state)(Screen *);
    void (*effect)(Screen *);
    u32 pad2d4;
    u16 timer;
    u8 pad2da[2];
    u32 args[10], script;
};
extern u16 gUnk_03005260[], gUnk_03004BD8;
void sub_082279A8(int, int, int, int, int, int, int);
void Script_Call(u32, void *);
void Task_Kill(void *);
void Sound_PlaySong(int);
void StatusScreen_FadeAndExit(Screen *);
void StatusScreen_FadeOutPalette(Screen *);
void sub_0800EF90(Screen *);
void StatusScreen_Close(Screen *);

void StatusScreen_FadeInPalette(Screen *p)
{
    int factor = p->fade, red, green, blue, base, bank, i;
    int other;
    if (factor > 256)
        factor = 256;
    other = 256 - factor;
    red = p->blend & 31;
    green = p->blend & 992;
    blue = p->blend & 31744;
    for (base = 0, bank = 0; bank < 2;) {
        i = p->start;
        {
            u16 *end = &p->end;
            int nextBase = base + 16;
            int nextBank = bank + 1;
            for (; i < *end; i++) {
                u16 *source = &p->source[base + i], *dest = &p->buffer[base + i];
                register int color asm("r1") = *source;
                int r;
                int g;
                int b;
                r = ((color & 31) * factor + other * red) & 0x1F00;
                g = ((color & 992) * factor + other * green) & 0x3E000;
                b = color & 31744;
                b = (b * factor + other * blue) & 0x7C0000;
                *dest = (r | g | b) >> 8;
            }
            base = nextBase;
            bank = nextBank;
        }
    }
    if (p->fade > 256) {
        p->effect = 0;
        p->fade = 0;
    }
    p->fade += p->fadeStep;
}
