#include "global.h"
struct Escape {
    u16 key, value;
};
extern const struct Escape gUnk_086055E4[];
u16 Text_DecodeGlyphEscape(const u8 *text)
{
    const struct Escape *escape;

    for (escape = gUnk_086055E4; escape->key; escape++) {
        if (escape->key == text[1])
            return escape->value;
    }
    return 0xFFFF;
}
