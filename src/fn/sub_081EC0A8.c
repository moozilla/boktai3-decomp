#include "global.h"
#include "engine.h"
#include "ram.h"
/* Parameters at 0x020003F8 (meaning unknown). */
struct ScreenParams3F8Params {
    u8 *mBlock;
    u8 *eBlock;
    s32 b;
    s32 d;
    s32 R;
    s32 t;
    s32 c;
    s32 B;
    s32 f;
    s32 M;
};
void ScreenParams3F8_SetFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        (*(struct ScreenParams3F8Params *)&gScreenTextParameters).mBlock = Script_GetPc();
    if (Script_SeekToKeyword('e'))
        (*(struct ScreenParams3F8Params *)&gScreenTextParameters).eBlock = Script_GetPc();
    if (Script_SeekToKeyword('b'))
        (*(struct ScreenParams3F8Params *)&gScreenTextParameters).b = Script_GetValue();
    if (Script_SeekToKeyword('d'))
        (*(struct ScreenParams3F8Params *)&gScreenTextParameters).d = Script_GetValue();
    if (Script_SeekToKeyword('R'))
        (*(struct ScreenParams3F8Params *)&gScreenTextParameters).R = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        (*(struct ScreenParams3F8Params *)&gScreenTextParameters).c = Script_GetValue();
    if (Script_SeekToKeyword('t'))
        (*(struct ScreenParams3F8Params *)&gScreenTextParameters).t = Script_GetValue();
    if (Script_SeekToKeyword('B'))
        (*(struct ScreenParams3F8Params *)&gScreenTextParameters).B = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        (*(struct ScreenParams3F8Params *)&gScreenTextParameters).f = Script_GetValue();
    if (Script_SeekToKeyword('M'))
        (*(struct ScreenParams3F8Params *)&gScreenTextParameters).M = Script_GetValue();
}
