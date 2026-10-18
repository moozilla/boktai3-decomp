#include "global.h"
u8 *Script_GetProcedure(u32, u32 *);
u32 Script_DecodePacket(u8 *, u32 *);
u32 Script_RunProcedureBlock(u32, u32, u32);
u32 Script_Call(u32 id, u32 b)
{
    u32 x, y;
    return Script_RunProcedureBlock(Script_DecodePacket(Script_GetProcedure(id, &x), &y), b, x);
}
