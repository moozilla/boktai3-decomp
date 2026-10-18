#define ScreenScript_UpdateDelay sub_081EC1A8
#define gScreenDelayParameters gUnk_02000420
#include "global.h"
struct ScreenScriptDelay { u8 unknown0[0x128]; s32 state; u32 unknown12C; s32 elapsed; s32 exponent; };
struct ScreenDelayParameters { u32 unknown0; u32 script; };
extern struct ScreenDelayParameters gUnk_02000420;
void Script_Call(u32, void *);
void Task_Kill(void *);
void sub_081EC1A8(struct ScreenScriptDelay *delay)
{
    if (delay->state == 1) {
        if (delay->elapsed > (1 << delay->exponent) || delay->exponent == 0) {
            Script_Call(gUnk_02000420.script, 0);
            Task_Kill(delay);
        }
        delay->elapsed++;
    }
}
