#include "global.h"
#include "engine.h"
void sub_0822E9FC(s32 slot, s32 value);
void sub_0822EA8C(s32 value, s32 slot);
/* game state fields, addressed index-first as the original does */
#define FRUIT_SLOT(i) (*(s16 *)((u8 *)gUnk_02000710 + (i) * 2 + 0x90))
/* Registry routine FE88 (Solar Fruit): clears the eight entries set by
   sub_0822E9FC, the eight slots at +0x90 and +0x54, then -n count, -w count
   values for sub_0822E9FC and -q eight values for sub_0822EA8C. */
void SolarFruit_Command(void)
{
    s32 i, count;
    for (i = 0; i <= 7; i++)
        sub_0822E9FC(i, -1);
    for (i = 0; i <= 7; i++)
        FRUIT_SLOT(i) = -1;
    *(u16 *)((u8 *)gUnk_02000710 + 0x54) = 0;
    if (Script_SeekToKeyword('n'))
        count = Script_GetValue();
    else
        count = 0;
    if (count > 0) {
        if (Script_SeekToKeyword('w'))
            for (i = 0; i < count; i++)
                sub_0822E9FC(i, Script_GetValue());
        if (Script_SeekToKeyword('q'))
            for (i = 0; i <= 7; i++)
                sub_0822EA8C(Script_GetValue(), i);
    }
}
