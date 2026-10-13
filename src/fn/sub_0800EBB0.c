#include "global.h"
// MATCHING_CONCESSION: register-pinning
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
void StatusScreen_WaitForClose(Screen *);
void StatusScreen_ClearScanlineNoise(Screen *);
void StatusScreen_WaitForReveal(Screen *p)
{
    if (!StatusScreen_CheckConfirm(p)) {
        if (p->timer++ >= *(u16 *)((u8 *)p + 0x2B6)) {
            p->pad2d4 = (u32)StatusScreen_ClearScanlineNoise;
            {
                register void (*state)(Screen *) asm("r1") = StatusScreen_WaitForClose;
                asm("" : "+r"(p) : "r"(state));
                p->state = state;
                p->timer = 0;
            }
        }
        p->dirty |= 15;
    }
}
