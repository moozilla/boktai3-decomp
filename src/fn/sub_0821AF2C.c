#include "global.h"
extern u32 gUnk_02000610[];
u32 Script_PushArgumentFrame(u32, u32);
void Script_RestoreArgumentStack(u32);
u8 *Script_DecodePacket(u8 *, u32 *);
u32 Script_ExecuteNativeCommand(u8 *);
u32 Script_CallInlineProcedure(u8 *);
u32 Script_EvalExpression(u8 *);
u32 Script_ExecuteBlock(u8 *p, u32 a, u32 b)
{
    u32 length, saved;
    saved = Script_PushArgumentFrame(a, b);
    while (p) {
        switch (*p & 0xF0) {
            case 0x60:
                p = Script_DecodePacket(p, &length);
                if (Script_ExecuteNativeCommand(p) == 1) {
                    p = (u8 *)1;
                    goto done;
                }
                p += length;
                break;
            case 0x70:
                p = Script_DecodePacket(p, &length);
                gUnk_02000610[1] = Script_CallInlineProcedure(p);
                p += length;
                break;
            case 0x30:
                p = Script_DecodePacket(p, &length);
                gUnk_02000610[1] = Script_EvalExpression(p);
                p += length;
                break;
            case 0:
                goto stop;
        }
    }
stop:
    p = 0;
done:
    Script_RestoreArgumentStack(saved);
    return (u32)p;
}
