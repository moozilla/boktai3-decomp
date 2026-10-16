#include "global.h"
#include "engine.h"
/* Parameters at 0x02000358 (meaning unknown). */
struct ScreenParams358Params {
    u8 *pBlock;
    u8 *PBlock;
    u8 *NBlock;
    s32 s;
    s32 f;
    s32 c;
    s32 S;
};
extern struct ScreenParams358Params gUnk_02000358;
/* Script: sets the parameter block (see the option letters). */
void ScreenParams358_SetFromScript(void)
{
    struct ScreenParams358Params *p = &gUnk_02000358;
    p->S = 0;
    if (Script_SeekToKeyword('p'))
        p->pBlock = Script_GetPc();
    if (Script_SeekToKeyword('P'))
        p->PBlock = Script_GetPc();
    if (Script_SeekToKeyword('N'))
        p->NBlock = Script_GetPc();
    if (Script_SeekToKeyword('s'))
        p->s = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        p->f = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        p->c = Script_GetValue();
    if (Script_SeekToKeyword('S'))
        p->S = 1;
}
