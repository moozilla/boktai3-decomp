#include "global.h"
#ifdef NONMATCHING
// fuzzy: 83.7%
// foundry draft (worker best 83.7%); what differs is mostly register allocation and instruction order
#define RecordPanel_DrawTime sub_0810D688
#define RecordPanel_DrawNumber sub_0810D60C
struct RecordPanel { u8 pad0[0x268]; u32 spriteFlags; };
extern u32 gUnk_030053F4, gUnk_030053F0;
s32 SelectionPanel_GetSelection(struct RecordPanel *);
s32 GameState_GetChallengeBestScore(u32);
void Video_DrawBackgroundRowMarkers(u32,u32);
void Registry3C9F_SetSelectedDisplayResource(struct RecordPanel *);
s32 Div(s32,s32);
void sub_0810D60C(s32,s32,s32,s32,s32);
u16 *Bg_GetWrappedTilemapEntry(u32,u32,u32);
void sub_0810D688(struct RecordPanel *p)
{
    struct RecordPanel *panel = p;
    s32 seconds;
    s32 fraction;
    s32 index;
    s32 time;
    u32 mask;
    index = SelectionPanel_GetSelection(p);
    time = GameState_GetChallengeBestScore(index);
    mask = 0x800;
    if ((gUnk_030053F4 | gUnk_030053F0) & mask) {
        index -= 6;
        if (time < 0) goto unavailable;
        if (index <= 4) goto available;
        goto unavailable;
    } else if (time < 0) {
    unavailable:
        Video_DrawBackgroundRowMarkers(2,12);
        panel->spriteFlags |= 1;
        return;
    }
available:
    Registry3C9F_SetSelectedDisplayResource(panel);
    panel->spriteFlags &= ~1;
    if (time > 59999) {
        seconds = 999;
        fraction = seconds;
    } else {
        seconds = Div(time,60);
        fraction = Div((time - seconds * 60) * 169,10);
    }
    {
        int zero = 0;
        sub_0810D60C(seconds,100,2,12,zero);
        sub_0810D60C(fraction,100,6,12,zero);
    }
    *Bg_GetWrappedTilemapEntry(3,9,12) = 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0810D688);
#endif
