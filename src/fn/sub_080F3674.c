#include "global.h"
/* Tentative family name: owner of the active child-record group. */
#define ChildGroup_ReadScriptParameters sub_080F3674
struct ChildGroup {
    u8 pad0[0xB4C];
    u16 parameterB4C;
    u8 padB4E[2];
    u16 parameterB50, parameterB52, parameterB54;
};
s32 Script_GetPc(void);
s32 Script_GetValue(void);
s32 Script_SeekToKeyword(u32);
static inline s32 ReadParameter(s32 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
void sub_080F3674(struct ChildGroup *p)
{
    if (Script_SeekToKeyword('P')) {
        p->parameterB4C = ReadParameter(60);
        p->parameterB50 = ReadParameter(2);
        p->parameterB52 = ReadParameter(80);
        p->parameterB54 = ReadParameter(1);
    } else {
        p->parameterB4C = 60;
        p->parameterB50 = 2;
        p->parameterB52 = 80;
        p->parameterB54 = 1;
    }
}
