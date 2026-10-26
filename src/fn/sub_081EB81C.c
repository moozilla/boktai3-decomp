#define gScreenParameters gScreenTextParameters
#include "global.h"
#include "ram.h"
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
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
s32 ScrollingDialogue_SetScript(const u8 *);
void ScreenText_ShowIndexedString(s32 index)
{
    if (gScreenTextParameters.mode != 2) {
        const void *text = gScreenTextParameters.text;
        index = ((struct ScreenTextRequest *)index)->index;
        ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(text) + index));
    } else {
        index = ((struct ScreenTextRequest *)index)->index;
        if (index > 0) {
            const void *text = gScreenTextParameters.text;
            index++;
            ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(text) + index));
        } else {
            ScrollingDialogue_SetScript(
                Text_LookupString(Script_ParseStringRef(gScreenTextParameters.text) + index));
        }
    }
}
