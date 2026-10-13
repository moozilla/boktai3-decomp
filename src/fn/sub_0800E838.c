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
void Script_Call(u32, void *);
void Task_Kill(void *);
void Sound_PlaySong(int);
void StatusScreen_FadeAndExit(Screen *);
void StatusScreen_FadeOutPalette(Screen *);
void sub_0800EF90(Screen *);
void StatusScreen_Close(Screen *);
void StatusScreen_Close(Screen *p)
{
    if (!p->timer) {
        p->start = 12;
        p->end = 16;
        p->blend = 0x1084;
        p->fade = 0;
        p->fadeStep = 2;
        p->effect = StatusScreen_FadeOutPalette;
    }
    if (p->timer == p->closeDelay) {
        Script_Call(p->script, p->args);
        Task_Kill(p);
        {
            u16 *flags = &gUnk_03004BD8;
            s32 mask;
            mask = ~256;
            mask &= *flags;
            *flags = mask;
        }
    }
    {
        u32 flags = p->dirty;
        u32 result;
        result = 4;
        result |= flags;
        {
            u32 mask = 65532;
            p->dirty = result & mask;
        }
    }
    p->timer++;
}
