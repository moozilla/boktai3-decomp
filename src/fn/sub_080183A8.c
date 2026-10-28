#include "global.h"
#include "ram.h"
#include "engine_globals.h"
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
    else if (gWeatherCooldownRange > 0) {
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 offset = gRandomIndex * 2;
            result = Mod(*(u16 *)((u8 *)gRandomTable + offset), gWeatherCooldownRange);
        }
    }
    gWeatherCooldownBase = base;
    gWeatherCooldownRange = spread;
}
