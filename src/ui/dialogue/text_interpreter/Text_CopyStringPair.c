#include "global.h"
s32 Script_ParseStringRef(const void *);
const u8 *Text_LookupString(s32);
// Copies two looked-up strings (ids offset by c and d) back to back into dst.
s32 Text_CopyStringPair(u8 *dst, const void *first, const void *second, s32 firstOffset,
                        s32 secondOffset)
{
    const u8 *text;
    s32 id;
    u8 c;

    id = Script_ParseStringRef(first);
    text = Text_LookupString(id + firstOffset);
    if (!text) {
        *dst = 0;
        return -1;
    }
    while ((c = *text)) {
        *dst = *text;
        text++;
        dst++;
    }
    id = Script_ParseStringRef(second);
    text = Text_LookupString(id + secondOffset);
    if (!text) {
        *dst = 0;
        return -1;
    }
    while ((c = *text)) {
        *dst = *text;
        text++;
        dst++;
    }
    *dst = c;
    return 0;
}
