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
void sub_0821AD08(u32, void *);
void sub_0821A0C0(void *);
void sub_0822B2F8(int);
void StatusScreen_FadeAndExit(Screen *);
void StatusScreen_FadeOutPalette(Screen *);
void sub_0800EF90(Screen *);
void StatusScreen_Close(Screen *);

void sub_082156B8(int);
void StatusScreen_FadeAndExit(Screen *p)
{
    int factor = 32 - p->timer, bank, i;
    int blueMask;
    register u16 *buffer asm("r9");
    bank = 0;
    buffer = p->buffer;
    blueMask = 31744;
    for (; bank < 2; bank++)
        for (i = 0; i < 16; i++) {
            {
                register int index asm("r6") = i;
                i = index;
            }
            {
                register u16 *out asm("r4") = &buffer[bank * 16 + i];
                register int color asm("r1") = *out;
                int r = ((color & 31) * factor) & 0x7C0;
                int g = ((color & 992) * factor) & 0xF800;
                int blue = color & blueMask;
                {
                    int b = (blue * factor) & 0x1F0000;
                    *out = (r | g | b) >> 6;
                }
            }
        }
    if (++p->timer > 31) {
        sub_082156B8(0);
        sub_0821AD08(p->script, p->args);
        sub_0821A0C0(p);
    } else
        p->dirty |= 15;
}
