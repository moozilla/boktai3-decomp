#include "global.h"
/* Tentative semantic names; field offsets are established by static accesses. */
struct TextLabelStyle {
    u8 index;
    u8 unknown1[0xB];
    u8 specialC;
    u8 unknownD[3];
    u8 special10;
    u8 unknown11[3];
    u8 suffixKind;
};
void TextLabel_FormatSuffix(struct TextLabelStyle *style, u8 *text)
{
    u32 length = 0;
    switch (style->suffixKind) {
    case 1:
        text[0] = 0x80;
        text[1] = 0xCA;
        length = 2;
        break;
    case 2:
        text[0] = 0x80;
        text[1] = 0xCB;
        length = 2;
        break;
    }
    if (style->specialC || style->special10) {
        text[length] = 'S';
        text[length + 1] = 'P';
        length += 2;
    }
    text[length] = 0;
}
