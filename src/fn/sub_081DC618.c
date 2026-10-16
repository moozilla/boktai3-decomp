#include "global.h"
#include "engine.h"
/* Parameters read by the screen task around 081DC1B4-081DC470 (meaning unknown). */
struct ScreenParams {
    u8 *script;
    s32 N, s, y, n, c;
};
extern struct ScreenParams gUnk_020002F8;
/* Script: sets the screen parameters: -m block, -N, -s, -y, -n, -c. */
void ScreenParams_SetFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        gUnk_020002F8.script = Script_GetPc();
    if (Script_SeekToKeyword('N'))
        gUnk_020002F8.N = Script_GetValue();
    if (Script_SeekToKeyword('s'))
        gUnk_020002F8.s = Script_GetValue();
    if (Script_SeekToKeyword('y'))
        gUnk_020002F8.y = Script_GetValue();
    if (Script_SeekToKeyword('n'))
        gUnk_020002F8.n = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gUnk_020002F8.c = Script_GetValue();
}
