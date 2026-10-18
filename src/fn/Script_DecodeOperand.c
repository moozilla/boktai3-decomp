#include "global.h"
u8 *Script_DecodeVariableValue(u8 *, s32 *, s32 *);
s32 Script_GetCallArgument(s32);
s32 Script_GetStackArgument(s32);
u8 *Script_DecodePacket(u8 *, u32 *);
s32 Script_EvalExpression(u8 *);
u8 *Text_LookupString(u16);
u8 *Script_DecodeOperand(u8 *p, s32 *type, s32 *value)
{
    u32 length;
    u8 *cursor = p;
    s32 opcode = *cursor;
    s32 family = opcode & 0xF0;
    if ((family & 0xC0) == 0xC0) {
        *type = 9;
        *value = (*cursor & ~0xC0) - 1;
        cursor++;
    } else if (!family) {
        *type = opcode;
        cursor++;
        switch (opcode) {
            case 0:
                cursor = 0;
                break;
            case 8: {
                s32 high = cursor[1] << 8;
                *value = cursor[0] | high;
                cursor += 2;
                break;
            }
            case 1: {
                s32 high = cursor[1] << 8;
                *value = (s16)(cursor[0] | high);
                cursor += 2;
                break;
            }
            case 9:
            case 10:
            case 13:
                *value = (cursor[3] << 24) | (cursor[2] << 16) | (cursor[1] << 8) | cursor[0];
                cursor += 4;
                break;
            case 6: {
                s32 high = cursor[1] << 8;
                *value = high | cursor[0];
                cursor += 2;
                break;
            }
            case 2:
            case 3:
            case 4:
                *value = *cursor++;
                break;
            case 7:
                *value = (s32)(cursor + 1);
                cursor += *cursor + 1;
                break;
            case 14: {
                s32 high = cursor[1] << 8;
                *value = (s32)Text_LookupString(cursor[0] | high);
                *type = 7;
                cursor += 2;
                break;
            }
        }
    } else {
        *type = family;
        switch (family) {
            case 0x10:
            case 0x20:
                return Script_DecodeVariableValue(p, type, value);
            case 0x40:
                if ((*cursor & 15) == 15) {
                    *value = Script_GetCallArgument(cursor[1] + 15);
                    cursor++;
                } else
                    *value = Script_GetCallArgument(*cursor & 15);
                *type = 9;
                cursor++;
                break;
            case 0x90:
                *value = Script_GetStackArgument(*cursor & 15);
                *type = 9;
                cursor++;
                break;
            case 0x80:
                cursor = Script_DecodePacket(cursor, &length);
                *value = (s32)cursor;
                cursor += length;
                break;
            case 0x30:
                cursor = Script_DecodePacket(cursor, &length);
                *value = Script_EvalExpression(cursor);
                cursor += length;
                break;
            case 0x50:
                cursor = Script_DecodePacket(cursor, &length);
                *type |= *cursor << 16;
                *value = (s32)(cursor + 1);
                cursor += length;
                break;
        }
    }
    return cursor;
}
