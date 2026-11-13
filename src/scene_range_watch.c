#include "scene_range_watch.h"
#include "script_utility.h"

void SceneRangeWatch_CheckRange(struct SceneRangeWatch *watch)
{
    s32 watchedValue = gGameState->unk876;
    u32 inside;
    if (watch->minimum <= watchedValue && watch->maximum >= watchedValue) {
        inside = 1;
        if (watch->inside == 1)
            return;
    } else {
        inside = 0;
        if (watch->inside == 0)
            return;
    }
    {
        s32 values[3];
        struct ScriptUtilityArguments args;
        values[0] = watchedValue;
        values[1] = watch->arguments[0];
        values[2] = watch->arguments[1];
        /* Assign only the count bitfield; leave upper-half padding unspecified. */
        args.count = 3;
        args.values = values;
        if (inside != 0) {
            if (watch->enterScript != 0) {
                Script_Call(watch->enterScript, &args);
                watch->inside = 1;
            }
        } else {
            if (watch->leaveScript != 0) {
                Script_Call(watch->leaveScript, &args);
                watch->inside = inside;
            }
        }
    }
}

s32 Mod(s32, s32);

u32 SceneRangeWatch_Tick(struct SceneRangeWatch *watch)
{
    if (watch->interval == 0)
        SceneRangeWatch_CheckRange(watch);
    else if (Mod(watch->frame, watch->interval) == 0)
        SceneRangeWatch_CheckRange(watch);
    watch->frame += 1;
    return 0;
}

u32 SceneRangeWatch_Destroy(void *unused)
{
    return 0;
}

static inline s32 SceneRangeWatch_ReadOptionalValue(s32 fallback)
{
    return Script_GetPc() ? Script_GetValue() : fallback;
}

static inline u16 SceneRangeWatch_ReadOptionalHalfword(void)
{
    return Script_GetPc() ? (u16)Script_GetValue() : 0;
}

s32 SceneRangeWatch_Initialize(struct SceneRangeWatch *input)
{
    struct SceneRangeWatch *watch = input;
    u8 *minimum;
    u8 *maximum;
    if (Script_SeekToKeyword('p')) {
        watch->enterScript = SceneRangeWatch_ReadOptionalValue(0);
        watch->leaveScript = SceneRangeWatch_ReadOptionalValue(0);
    } else {
        watch->enterScript = 0;
        watch->leaveScript = 0;
    }
    if (Script_SeekToKeyword('A')) {
        u32 *argument;
        s32 index;
        minimum = &watch->minimum;
        maximum = &watch->maximum;
        argument = watch->arguments;
        index = 1;
        do {
            *argument++ = SceneRangeWatch_ReadOptionalHalfword();
        } while (--index >= 0);
    } else {
        u32 *start;
        u32 *argument;
        u32 zero;
        minimum = &watch->minimum;
        maximum = &watch->maximum;
        start = watch->arguments;
        zero = 0;
        argument = &watch->arguments[1];
        do {
            *argument = zero;
            argument--;
        } while ((s32)argument >= (s32)start);
    }
    {
        s32 value;
        if (Script_SeekToKeyword('u')) {
            *minimum = SceneRangeWatch_ReadOptionalValue(1);
            value = SceneRangeWatch_ReadOptionalValue(1);
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

struct SceneRangeWatch *SceneRangeWatch_CreateFromScript(void)
{
    struct SceneRangeWatch *watch = Task_Create(8, sizeof(struct SceneRangeWatch));
    if (watch) {
        Task_SetCallbacks(watch, (void (*)(void *))SceneRangeWatch_Tick,
                          (void (*)(void *))SceneRangeWatch_Destroy);
        if (SceneRangeWatch_Initialize(watch) < 0) {
            Task_Kill(watch);
            return 0;
        }
    }
    return watch;
}
