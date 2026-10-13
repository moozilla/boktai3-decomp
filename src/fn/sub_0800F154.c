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
void sub_0800E77C(Screen *);
int BgGfx_LoadTilemapSimple(int, int, int, int, int);
int StatusScreen_Update(Screen *p)
{
    int zero;
    if (p->state)
        p->state(p);
    if (p->effect)
        p->effect(p);
    if (p->pad2d4)
        ((void (*)(Screen *))p->pad2d4)(p);
    {
        u16 next = 1 - *(u16 *)((u8 *)p + 0x2AC);
        zero = 0;
        *(u16 *)((u8 *)p + 0x2AC) = next;
    }
    sub_0800E77C(p);
    return BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
}
