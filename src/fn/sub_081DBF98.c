#include "global.h"
#include "engine.h"
/* Parameters at 0x020002E8 (meaning unknown). */
struct ScreenParams2E8 {
    u8 *lBlock;
    s32 o, c, f;
};
extern struct ScreenParams2E8 gUnk_020002E8;
/* Script: -l block, -o, -c (default 0) and -f (default 0). */
void ScreenParams2E8_SetFromScript(void)
{
    struct ScreenParams2E8 *p = &gUnk_020002E8;
    p->c = 0;
    p->f = 0;
    if (Script_SeekToKeyword('l'))
        p->lBlock = Script_GetPc();
    if (Script_SeekToKeyword('o'))
        p->o = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        p->c = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        p->f = Script_GetValue();
}
