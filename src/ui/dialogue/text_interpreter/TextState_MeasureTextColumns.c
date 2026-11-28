#include "global.h"
#include "ui/dialogue/text_state.h"
u16 Text_DecodeGlyphEscape(const u8 *);
int TextState_MeasureTextColumns(struct TextState *state)
{
    const u8 *text = state->pc;
    int columns = 0;
    u32 c;

    while ((c = *text)) {
        c <<= 24;
        if ((s32)c >= 0) {
            c >>= 24;
            if (c == '<') {
                do {
                    c = *++text;
                } while (c != '>' && c && c != '\n');
                text++;
            } else if (c == 0x1F) {
                u16 code = Text_DecodeGlyphEscape(text);

                if (code == 0xFFFF) {
                    columns++;
                    text++;
                } else if (code & 0xFF00) {
                    columns += 2;
                    text += 2;
                } else {
                    columns++;
                    text += 2;
                }
            } else {
                columns++;
                text++;
            }
        } else {
            columns += 2;
            text += 2;
        }
        if (columns > 254) {
            columns = 255;
            break;
        }
    }
    return columns;
}
