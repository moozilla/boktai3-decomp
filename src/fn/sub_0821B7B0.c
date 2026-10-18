#include "global.h"
u8 *Script_DecodeOperand(u8 *, s32 *, s32 *);
void Script_SetPc(u8 *);
s32 Script_ReadNextKeyword(void);
s32 Script_GetValue(void);
u8 *Script_GetPc(void);
s32 Script_ExecuteBlock(u8 *, u32, u32);
s32 Script_ExecuteSwitch(u8 *p)
{
    s32 type, value, block = 0;
    Script_SetPc(Script_DecodeOperand(p, &type, &value));
    for (;;) {
        s32 option = Script_ReadNextKeyword();
        if (!option)
            return 0;
        if (option == 'c') {
            if (Script_GetValue() != value)
                continue;
            Script_DecodeOperand(Script_GetPc(), &type, &block);
            break;
        }
        if (option == 'd') {
            Script_DecodeOperand(Script_GetPc(), &type, &block);
            break;
        }
    }
    return Script_ExecuteBlock((u8 *)block, 0, 0);
}
