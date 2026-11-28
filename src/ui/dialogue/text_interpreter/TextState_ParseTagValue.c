#include "global.h"
struct Tag {
    u8 pad[0x30];
    int hasValue, length;
    u8 text[64];
};
const u8 *TextState_ParseTagValue(struct Tag *tag, const u8 *text)
{
    int n;
    u8 c;

    tag->hasValue = 0;
    tag->length = 0;
    for (n = 0; *text != '>' && *text;) {
        if ((c = *text) == '=')
            tag->hasValue = TRUE;
        else if (tag->hasValue)
            tag->text[tag->length++] = c;
        n++;
        text++;
        if (n > 63)
            break;
    }
    tag->text[tag->length] = 0;
    return text;
}
