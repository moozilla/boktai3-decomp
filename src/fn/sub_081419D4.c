#include "global.h"
extern const u16 gUnk_08610980[], gUnk_086109F4[], gUnk_08610A68[], gUnk_08610ADC[];
s32 ActorVariant_GetTableValue(s32 kind, s32 index)
{
    s32 result;
    const u16 *table;
    if (kind == 0) {
        table = gUnk_08610980;
        result = table[index];
    } else if (kind == 1) {
        table = gUnk_086109F4;
        result = table[index];
    } else if (kind == 2) {
        table = gUnk_08610A68;
        result = table[index];
    } else {
        if (kind != 6)
            return 0;
        else {
            table = gUnk_08610ADC;
            result = table[index];
        }
    }
    return result;
}
