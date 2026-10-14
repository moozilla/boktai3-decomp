#include "global.h"
u32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u8 *Script_GetPc(void);
s32 Script_GetKeywordValue(s32, s32);
u8 *Script_DecodeOperand(u8 *, s32 *, s32 *);
u8 *sub_08228368(u32, void *, s32, s32);
/* Script command: run a script later. Keywords: t delay, P urgent, e script
 * (by reference) or p script (by id, flag 1), a up to eight argument values. */
u8 *sub_082283F4(u32 owner, u32 unused, s32 repeat)
{
    s32 delay = 0;
    s32 script = 0;
    s32 urgent, flag;
    u8 *t;
    if (Script_SeekToKeyword('t'))
        delay = Script_GetValue();
    urgent = Script_GetKeywordValue('P', 0);
    flag = 0;
    if (Script_SeekToKeyword('e')) {
        s32 type;
        s32 value;
        Script_DecodeOperand(Script_GetPc(), &type, &value);
        script = value;
    } else if (Script_SeekToKeyword('p')) {
        flag = 1;
        script = Script_GetValue();
    }
    t = sub_08228368(script, 0, delay, urgent);
    if (t) {
        *(u32 *)(t + 0x18) = owner;
        *(u16 *)(t + 0x2e) = flag;
        if (repeat) {
            flag |= 2;
            *(u16 *)(t + 0x2e) = flag;
        }
        if (Script_SeekToKeyword('a')) {
            s32 i;
            *(u32 **)(t + 0x20) = (u32 *)(t + 0x30);
            for (i = 0; i <= 7 && Script_GetPc(); i++)
                (*(u32 **)(t + 0x20))[i] = Script_GetValue();
            *(u16 *)(t + 0x1c) = i;
        }
    }
    return t;
}
