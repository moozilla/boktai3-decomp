#include "global.h"
#include "engine.h"
/* Parameters at 0x020003D0 (meaning unknown). */
struct Params3D0 {
    s32 g, o, l, s, d, c;
    u16 t, b;
    s32 f[2];
    s32 M;
};
extern struct Params3D0 gUnk_020003D0;
/* Script: -M (default 0), -g, -o, -l, -s, -d, -c, -t, -b and -f (two values). */
void Params3D0_SetFromScript(void)
{
    struct Params3D0 *p = &gUnk_020003D0;
    p->M = 0;
    if (Script_SeekToKeyword('g'))
        p->g = Script_GetValue();
    if (Script_SeekToKeyword('o'))
        p->o = Script_GetValue();
    if (Script_SeekToKeyword('l'))
        p->l = Script_GetValue();
    if (Script_SeekToKeyword('s'))
        p->s = Script_GetValue();
    if (Script_SeekToKeyword('d'))
        p->d = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        p->c = Script_GetValue();
    if (Script_SeekToKeyword('t'))
        p->t = Script_GetValue();
    if (Script_SeekToKeyword('b'))
        p->b = Script_GetValue();
    if (Script_SeekToKeyword('f')) {
        p->f[0] = Script_GetValue();
        p->f[1] = Script_GetValue();
    }
    if (Script_SeekToKeyword('M'))
        p->M = Script_GetValue();
}
