#include "global.h"
#include "engine.h"
/* Parameters at 0x020002C0 (meaning unknown). */
struct ScreenParams2C0 {
    u8 *mBlock, *eBlock;
    s32 s, r, x, c, n, b, n2, p;
};
extern struct ScreenParams2C0 gUnk_020002C0;
/* Script: -m and -e blocks, -s, -r, -x, -c, -n (two values, the second
   default 0), -b (default 0) and -p. */
void ScreenParams2C0_SetFromScript(void)
{
    struct ScreenParams2C0 *p = &gUnk_020002C0;
    p->b = 0;
    p->n2 = 0;
    if (Script_SeekToKeyword('m'))
        p->mBlock = Script_GetPc();
    if (Script_SeekToKeyword('e'))
        p->eBlock = Script_GetPc();
    if (Script_SeekToKeyword('s'))
        p->s = Script_GetValue();
    if (Script_SeekToKeyword('r'))
        p->r = Script_GetValue();
    if (Script_SeekToKeyword('x'))
        p->x = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        p->c = Script_GetValue();
    if (Script_SeekToKeyword('n')) {
        p->n = Script_GetValue();
        p->n2 = Script_GetValue();
    }
    if (Script_SeekToKeyword('b'))
        p->b = Script_GetValue();
    if (Script_SeekToKeyword('p'))
        p->p = Script_GetValue();
}
