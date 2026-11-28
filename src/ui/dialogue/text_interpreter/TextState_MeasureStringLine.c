#include "ui/dialogue/message_window.h"
int Text_ComparePrefix(const u8 *, const u8 *, int);
int Text_ParseDecimal(const u8 *, int);
int TextState_GetNumberWidth(struct MessagePlaybackState *, int);
u16 Text_DecodeGlyphEscape(const u8 *);
const u8 *Text_FindChar(const u8 *, u8);
extern const u8 gUnk_0824DDF4[];
int TextState_MeasureStringLine(struct MessagePlaybackState *state, int index)
{
    const u8 *text = state->strings[index];
    int width = 0;
    u32 c;

    for (;;) {
        c = *text;
        c <<= 24;
        if ((s32)c >= 0) {
            c >>= 24;
            if (!c)
                break;
            if (c == '\n')
                break;
            if (*text == '<') {
                const u8 *end;

                text++;
                if (!*text)
                    return width;
                end = text;
                if (!Text_ComparePrefix(text, gUnk_0824DDF4, 3)) {
                    int hasValue = 0;
                    int length = 0;
                    int number;

                    while (*end != '>') {
                        int ch;

                        if (!*end) {
                            end++;
                            break;
                        }
                        ch = *end;
                        if (ch == '=')
                            hasValue = 1;
                        else if (hasValue)
                            state->tag.value[length++] = ch;
                        end++;
                    }
                    state->tag.value[length] = 0;
                    if (hasValue && length)
                        number = Text_ParseDecimal(state->tag.value, length);
                    else
                        number = 0;
                    width += TextState_GetNumberWidth(state, number);
                    Text_FindChar(end, '>');
                } else {
                    Text_FindChar(text, '>');
                }
                if (!*text)
                    break;
            } else if (*text == 0x1F) {
                u16 code = Text_DecodeGlyphEscape(text);

                if (code == 0xFFFF)
                    width += 2;
                else if (code & 0xFF00)
                    width += 2;
                else
                    width++;
                text += 2;
            } else {
                width++;
                text++;
            }
        } else {
            width += 2;
            text += 2;
        }
    }
    return width;
}
