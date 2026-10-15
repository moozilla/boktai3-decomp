// MATCHING_CONCESSION: register-pinning
#include "scene_range_watch.h"
static inline s32 Read(s32 fallback)
{
    return Script_GetPc() ? Script_GetValue() : fallback;
}
static inline u16 ReadHalf(void)
{
    return Script_GetPc() ? (u16)Script_GetValue() : 0;
}
s32 SceneRangeWatch_Initialize(struct SceneRangeWatch *input)
{
    struct SceneRangeWatch *watch = input;
    u8 *minimum;
    u8 *maximum;
    if (Script_SeekToKeyword('p')) {
        watch->enterScript = Read(0);
        watch->leaveScript = Read(0);
    } else {
        watch->enterScript = 0;
        watch->leaveScript = 0;
    }
    if (Script_SeekToKeyword('A')) {
        u32 *arg;
        s32 i;
        minimum = &watch->minimum;
        maximum = &watch->maximum;
        arg = watch->arguments;
        i = 1;
        do {
            *arg++ = ReadHalf();
        } while (--i >= 0);
    } else {
        s32 start;
        s32 arg;
        u32 zero;
        minimum = &watch->minimum;
        {
            register u32 last asm("r1") = 0x2B;
            asm("" : "+r"(last));
            last += (u32)watch;
            asm("" : "+r"(last));
            maximum = (u8 *)last;
        }
        start = (s32)watch->arguments;
        zero = 0;
        arg = (s32)&watch->arguments[1];
        do {
            *(u32 *)arg = zero;
            arg -= 4;
        } while (arg >= start);
    }
    {
        s32 value;
        if (Script_SeekToKeyword('u')) {
            *minimum = Read(1);
            value = Read(1);
        } else {
            *minimum = 1;
            value = 8;
        }
        *maximum = value;
    }
    watch->interval = Script_GetKeywordValue('f', 0);
    watch->frame = 0;
    watch->inside = 0;
    return 0;
}
