#include "global.h"
// MATCHING_CONCESSION: register-pinning
// The optimization override reproduces this body; it is not historical compiler evidence.
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
void Script_Call(u32, void *);
void Task_Kill(void *);
void Sound_PlaySong(int);
void StatusScreen_FadeAndExit(Screen *);
void StatusScreen_FadeOutPalette(Screen *);
void sub_0800EF90(Screen *);
void StatusScreen_Close(Screen *);
int StatusScreen_CheckConfirm(Screen *);
void StatusScreen_FadeInPalette(Screen *);
void StatusScreen_WaitForReveal(Screen *);
void StatusScreen_Reveal(Screen *p)
{
    if (!StatusScreen_CheckConfirm(p)) {
        if (!p->timer) {
            Sound_PlaySong(225);
            p->top = 0;
            p->bottom = 160;
            p->reveal = 256;
            p->revealStep = -2;
            p->pad2d4 = (u32)sub_0800EF90;
            p->blend = 0x1084;
            p->fade = 0;
            p->fadeStep = 4;
            p->effect = StatusScreen_FadeInPalette;
        }
        if (p->timer++ >= *(u16 *)((u8 *)p + 0x2B8)) {
            {
                register void (*state)(Screen *) asm("r0") = StatusScreen_WaitForReveal;
                asm("" : "+r"(p) : "r"(state));
                p->state = state;
            }
            p->timer = 0;
        }
        p->dirty |= 15;
    }
}

// CFLAGS: -O2 -mthumb-interwork -fno-expensive-optimizations
