#define gScreenParameters gUnk_020003F8
#include "global.h"
struct ScreenTextRequest {
    u8 unknown0[0x1C];
    s32 index;
};
struct ScreenTextParameters {
    u32 unknown0;
    const void *text;
    u8 unknown8[0x1C];
    s32 mode;
};
extern struct ScreenTextParameters gUnk_020003F8;
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
s32 ScrollingDialogue_SetScript(const u8 *);
void ScreenText_ShowIndexedString(s32 index)
{
    if (gUnk_020003F8.mode != 2) {
        const void *text = gUnk_020003F8.text;
        index = ((struct ScreenTextRequest *)index)->index;
        ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(text) + index));
    } else {
        index = ((struct ScreenTextRequest *)index)->index;
        if (index > 0) {
            const void *text = gUnk_020003F8.text;
            index++;
            ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(text) + index));
        } else {
            ScrollingDialogue_SetScript(
                Text_LookupString(Script_ParseStringRef(gUnk_020003F8.text) + index));
        }
    }
}
