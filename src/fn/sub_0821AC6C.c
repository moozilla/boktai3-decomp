#include "global.h"
u32 Script_FindCommand(u32);
u8 *Script_DecodeCommandOffset(u8 *, u32 *);
void Script_PushCtrlNextKeyword(u8 *);
void Script_PopCtrlNextKeyword(void);
void Script_SetPc(u8 *);
u8 *sub_0824923C(u8 *, u32);
u8 *Script_ExecuteNativeCommand(u8 *p)
{
    u32 len;
    u32 id = (p[1] << 8) | p[0];
    u32 *e;
    p += 2;
    e = (u32 *)Script_FindCommand(id);
    p = Script_DecodeCommandOffset(p, &len);
    Script_PushCtrlNextKeyword(p + len);
    Script_SetPc(p);
    p = sub_0824923C(p, e[1]);
    Script_PopCtrlNextKeyword();
    return p;
}
