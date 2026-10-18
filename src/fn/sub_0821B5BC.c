#include "global.h"
struct Reference {
    u8 bytes[4];
    u16 first, index;
};
extern u8 *gUnk_0200070C, *gUnk_02000704;
u8 *Script_DecodeVariableReference(u8 *, struct Reference *);
s32 Script_ReadReferenceValue(struct Reference *, s32);
void Script_WriteTypedVariable(u8 *, u32, s32, s32);
u8 *Script_BackupVariableOperand(u8 *p)
{
    struct Reference ref;
    u8 *r = (u8 *)&ref;
    u8 *end, *base;
    s32 index = 0;
    s32 value;
    u32 descriptor;
    end = Script_DecodeVariableReference(p, &ref);
    value = Script_ReadReferenceValue(&ref, 0);
    descriptor = (r[0] << 24) | (r[1] << 16) | (r[2] << 8) | r[3];
    if ((descriptor & 0xF00000) == 0x800000)
        base = gUnk_0200070C;
    else
        base = gUnk_02000704;
    base += descriptor & 0xFFFF;
    if (((descriptor >> 24) & 0xF0) == 0x20)
        index += *(u16 *)(r + 6);
    Script_WriteTypedVariable(base, descriptor, index, value);
    return end;
}
