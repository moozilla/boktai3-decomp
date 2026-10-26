#include "global.h"
#include "ram.h"
struct ScreenScriptDelay {
    u8 unknown0[0x128];
    s32 state;
    u32 unknown12C;
    s32 elapsed;
    s32 exponent;
};
struct ScreenDelayParameters {
    u32 unknown0;
    u32 script;
};
void Script_Call(u32, void *);
void Task_Kill(void *);
void ScreenScript_UpdateDelay(struct ScreenScriptDelay *delay)
{
    if (delay->state == 1) {
        if (delay->elapsed > (1 << delay->exponent) || delay->exponent == 0) {
            Script_Call(gScreenDelayParameters.script, 0);
            Task_Kill(delay);
        }
        delay->elapsed++;
    }
}
