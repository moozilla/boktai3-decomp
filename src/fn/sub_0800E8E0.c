#include "global.h"
typedef struct Screen Screen;
struct Screen {
    u8 task[40];
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
// MATCHING_CONCESSION: register-pinning
void StatusScreen_BeginClose(Screen *p)
{
    if (!p->timer) {
        sub_0822B2F8(227);
        p->top = 116;
        p->bottom = 160;
        p->reveal = 0;
        p->revealStep = 4;
        p->pad2d4 = (u32)sub_0800EF90;
    }
    if (p->timer++ >= p->revealDuration) {
        p->dirty &= ~3;
        {
            register void (*state)(Screen *) asm("r1") = StatusScreen_Close;
            asm("" : "+r"(p) : "r"(state));
            p->state = state;
            p->timer = 0;
        }
    }
    p->dirty |= 4;
}

// CFLAGS: -O2 -mthumb-interwork -fno-expensive-optimizations
