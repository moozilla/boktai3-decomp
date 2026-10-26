#include "global.h"
extern int gUnk_02000080, gUnk_02000084;
extern u32 gRandomIndex;
extern u16 gUnk_0203B400[];
int Script_SeekToKeyword(int);
int Script_GetValue(void);
int Mod(int, int);
int Weather_ConfigureCooldown(void)
{
    int base, spread, result;
    if (Script_SeekToKeyword('c')) {
        base = Script_GetValue();
        spread = Script_GetValue();
    } else {
        base = spread = 1200;
    }
    result = Script_SeekToKeyword('i');
    if (result)
        result = Script_GetValue();
    else if (gUnk_02000084 > 0) {
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 offset = gRandomIndex * 2;
            result = Mod(*(u16 *)((u8 *)gUnk_0203B400 + offset), gUnk_02000084);
        }
    }
    gUnk_02000080 = base;
    gUnk_02000084 = spread;
}
