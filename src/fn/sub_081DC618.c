#include "global.h"
#include "engine.h"
#include "ram.h"
/* Parameters read by the screen task around 081DC1B4-081DC470 (meaning unknown). */
struct ScreenParams {
    u8 *script;
    s32 N, s, y, n, c;
};
void ScreenParams_SetFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        gScreenParams.script = Script_GetPc();
    if (Script_SeekToKeyword('N'))
        gScreenParams.N = Script_GetValue();
    if (Script_SeekToKeyword('s'))
        gScreenParams.s = Script_GetValue();
    if (Script_SeekToKeyword('y'))
        gScreenParams.y = Script_GetValue();
    if (Script_SeekToKeyword('n'))
        gScreenParams.n = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gScreenParams.c = Script_GetValue();
}
