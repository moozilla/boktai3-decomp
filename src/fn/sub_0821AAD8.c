#include "global.h"
extern u8 *gUnk_02000610;
u8 *Script_GetPc(void);
u8 *Script_DecodeVariableReference(u8 *, u32);
void Script_ReadReference(u32 a)
{
    gUnk_02000610 = Script_DecodeVariableReference(Script_GetPc(), a);
}
