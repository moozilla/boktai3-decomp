#include "global.h"
#include "ram.h"

void AnimData_SelectPaletteBank(void *, u32);
s32 Script_GetValue(void);
s32 Script_SeekToKeyword(u32);
void Sound_PlaySong(u32);
void SparkleEmitter_UpdateEmitting(void *);

void SparkleEmitter_ReconfigureFromScript(void)
{
    u8 *p = (u8 *)gSparkleEmitter;
    s32 v776;
    s32 v774;
    s32 v768;

    if (p == 0)
        return;

    if (Script_SeekToKeyword(0x70) != 0)
        v776 = Script_GetValue();
    else
        v776 = *(s16 *)(p + 0x776);

    if (Script_SeekToKeyword(0x64) != 0) {
        u32 t = (u32)Script_GetValue();
        v774 = ((t << 29) + 0xA0000000) >> 24;
    } else {
        v774 = *(s16 *)(p + 0x774);
    }

    if (Script_SeekToKeyword(0x63) != 0)
        v768 = Script_GetValue();
    else
        v768 = *(s32 *)(p + 0x768);

    *(s16 *)(p + 0x776) = v776;
    *(s16 *)(p + 0x774) = v774;
    *(s32 *)(p + 0x768) = v768;
    if ((u32)v768 <= 14)
        AnimData_SelectPaletteBank(p + 0x718, (u32)v768 + 0x255);
    Sound_PlaySong(0x83);
    {
        void (*fn)(void *) = SparkleEmitter_UpdateEmitting;
        *(void (**)(void *))(p + 0x77C) = fn;
    }
    *(u32 *)(p + 0x770) = 0;
}
