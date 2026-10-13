#include "global.h"
// MATCHING_CONCESSION: register-pinning
typedef struct Screen Screen;
struct Screen {
    u8 task[40];
    u16 dirty, delay;
    u8 pad2c[0x282];
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

int StatusScreen_CheckConfirm(Screen *);
void StatusScreen_Reveal(Screen *);
extern Screen *gUnk_02000044;
void StatusScreen_WaitForStart(Screen *p)
{
    u32 mask = 256;
    gUnk_03004BD8 &= ~mask;
    if (!StatusScreen_CheckConfirm(p)) {
        u32 old = p->delay;
        p->delay = old + 65535;
        asm("" : "+r"(old));
        if ((u16)old == 0) {
            {
                register int flags asm("r1") = gUnk_03004BD8;
                {
                    int result = mask;
                    gUnk_03004BD8 = result | flags;
                }
            }
            {
                Screen *target = gUnk_02000044;
                {
                    register void (*state)(Screen *) asm("r1") = StatusScreen_Reveal;
                    target->state = state;
                    target->timer = 0;
                }
            }
        }
    }
}
