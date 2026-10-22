#include "global.h"
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void SceneObject_StartBackgroundScript(u8 *);
void TimeResult_InitAnimations(u8 *);
void TimeResult_LoadPalette(u8 *);
void TimeResult_BeginFadeIn(u8 *);

s32 sub_081C6570(u8 *p)
{
    s32 left, right;
    if (Script_SeekToKeyword(0x65))
        *(s32 *)(p + 0x364) = Script_GetValue();
    if (Script_SeekToKeyword(0x6c))
        left = Script_GetValue();
    else
        left = 0;
    if (Script_SeekToKeyword(0x72))
        right = Script_GetValue();
    else
        right = 0;
    if (Script_SeekToKeyword(0x41)) {
        s32 category = Script_GetValue();
        *(s32 *)(p + 0x358) = category;
        if ((u32)(category - 1) > 4)
            *(s32 *)(p + 0x358) = 1;
    } else {
        *(s32 *)(p + 0x358) = 1;
    }
    *(s32 *)(p + 0x368) = left - right;
    *(s32 *)(p + 0x36c) = 0;
    *(s32 *)(p + 0x370) = 0;
    SceneObject_StartBackgroundScript(p);
    TimeResult_InitAnimations(p);
    TimeResult_LoadPalette(p);
    TimeResult_BeginFadeIn(p);
    return 0;
}
