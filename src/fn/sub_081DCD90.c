#include "global.h"
#include "engine.h"
s32 Script_GetValueSafe(void);
/* Parameters at 0x02000310 (meaning unknown). */
struct ScreenParams310 {
    u8 *nBlock;
    s32 d, o, c, f;
};
extern struct ScreenParams310 gUnk_02000310;
/* Script: -n block, -d (read with Script_GetValueSafe), -o, -c and -f
   (default 0). */
void ScreenParams310_SetFromScript(void)
{
    struct ScreenParams310 *p = &gUnk_02000310;
    p->f = 0;
    if (Script_SeekToKeyword('n'))
        p->nBlock = Script_GetPc();
    if (Script_SeekToKeyword('d'))
        p->d = Script_GetValueSafe();
    if (Script_SeekToKeyword('o'))
        p->o = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        p->c = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        p->f = Script_GetValue();
}
