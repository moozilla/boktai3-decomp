#include "global.h"
#include "engine.h"
struct Nidhoggr {
    u8 pad0[0x7268];
    u16 field7268, field726A, field726C, field726E, field7270, field7272, field7274, field7276;
    u8 pad7278[0x28];
    u16 value0, value1, value2;
    u8 valueMode;
    u8 pad72A7[0x19];
    u32 field72C0;
};
/* Script options of the Nidhoggr boss (both of its creators call this):
   -l (4000, kept twice, likely current and maximum life), -g, -Y, -e, -A,
   -D, -E, -S, and -c two or -f three values with a mode byte (0 or 1). */
void Nidhoggr_ReadOptions(struct Nidhoggr *p)
{
    u32 value;
    u8 zero8;
    u16 zero;
    p->field7268 = value = Script_GetKeywordValue('l', 4000);
    zero8 = 0;
    zero = 0;
    p->field7276 = value;
    p->field726A = Script_GetKeywordValue('g', 100);
    p->field726C = Script_GetKeywordValue('Y', 1000);
    p->field72C0 = Script_GetKeywordValue('e', 0);
    p->field726E = Script_GetKeywordValue('A', 40);
    p->field7270 = Script_GetKeywordValue('D', 20);
    p->field7272 = Script_GetKeywordValue('E', 100);
    p->field7274 = Script_GetKeywordValue('S', 0);
    p->value0 = zero;
    p->value1 = zero;
    p->value2 = zero;
    p->valueMode = zero8;
    if (Script_SeekToKeyword('c')) {
        p->value0 = Script_GetValue();
        p->value1 = Script_GetValue();
        p->valueMode = zero8;
    }
    if (Script_SeekToKeyword('f')) {
        p->value0 = Script_GetValue();
        p->value1 = Script_GetValue();
        p->value2 = Script_GetValue();
        p->valueMode = 1;
    }
}
