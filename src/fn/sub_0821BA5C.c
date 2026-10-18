#include "global.h"
struct Slot {
    s32 value;
    u8 *reference;
};
struct Frame {
    struct Slot stack[8];
    s32 type, value;
};
extern u32 gUnk_02000610[];
u8 *Script_DecodeOperand(u8 *, s32 *, s32 *);
void Script_SetStackArgument(s32, u32);
u8 *Script_AssignVariableOperand(u8 *, s32);
s32 Script_ApplyOperator(s32, s32, s32);
u32 Script_ExecuteBlock(u8 *, u32, u32);
u32 Script_EvalExpression(u8 *p)
{
    struct Frame frame;
    struct Slot *slot = (struct Slot *)((u32)frame.stack - 8);
    for (;;) {
        frame.type = *p;
        if ((frame.type & 0xE0) == 0xA0) {
            s32 op = frame.type & ~0xE0;
            slot = (struct Slot *)((u32)slot - 8);
            if (!op)
                return ((struct Slot *)((u32)slot + 8))->value;
            if (op == 0x16) {
                struct Slot *saved;
                u8 *reference = slot->reference;
                u32 type = *reference;
                u32 category = type & 0xF0;
                saved = slot;
                if (category == 0x90)
                    Script_SetStackArgument(type & 15, ((struct Slot *)((u32)slot + 8))->value);
                else
                    Script_AssignVariableOperand(reference, ((struct Slot *)((u32)slot + 8))->value);
                saved->value = ((struct Slot *)((u32)slot + 8))->value;
            } else {
                slot->value =
                    Script_ApplyOperator(op, slot->value, ((struct Slot *)((u32)slot + 8))->value);
                slot->reference = 0;
            }
            p++;
        } else {
            ((struct Slot *)((u32)slot + 8))->reference = p;
            p = Script_DecodeOperand(p, &frame.type, &frame.value);
            if (frame.type == 0x80) {
                Script_ExecuteBlock((u8 *)frame.value, 0, 0);
                ((struct Slot *)((u32)slot + 8))->value = gUnk_02000610[1];
            } else
                ((struct Slot *)((u32)slot + 8))->value = frame.value;
            slot = (struct Slot *)((u32)slot + 8);
        }
    }
}
