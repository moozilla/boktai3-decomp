#include "global.h"
struct Vm {
    u8 pad0[0x38];
    u8 text[64];
    u8 pad78[0x98];
    const u8 *strings[16];
};
int Text_ComparePrefix(const u8 *, const u8 *, int);
int Text_ParseDecimal(const u8 *, int);
int TextState_GetNumberWidth(struct Vm *, int);
u16 Text_DecodeGlyphEscape(const u8 *);
const u8 *Text_FindChar(const u8 *, u8);
extern const u8 gUnk_0824DDF4[];
int TextState_MeasureStringLine(struct Vm *p, int index)
{
    const u8 *s = p->strings[index];
    int n = 0;
    while (1) {
        u32 c = *s;
        c <<= 24;
        if ((s32)c >= 0) {
            c >>= 24;
            if (!c)
                break;
            if (c == '\n')
                break;
            if (*s == '<') {
                const u8 *q;
                s++;
                if (!*s)
                    return n;
                q = s;
                if (!Text_ComparePrefix(s, gUnk_0824DDF4, 3)) {
                    int has = 0;
                    int length = 0;
                    u8 *text;
                    c = *q;
                    text = p->text;
                    while (c != '>') {
                        int ch;
                        if (!c) {
                            q++;
                            break;
                        }
                        ch = *q;
                        if (ch == '=')
                            has = 1;
                        else if (has)
                            text[length++] = ch;
                        q++;
                        c = *q;
                    }
                    text[length] = 0;
                    if (has && length)
                        index = Text_ParseDecimal(text, length);
                    else
                        index = 0;
                    n += TextState_GetNumberWidth(p, index);
                    Text_FindChar(q, '>');
                } else
                    Text_FindChar(s, '>');
                if (!*s)
                    break;
            } else if (*s == 0x1F) {
                u32 code = (u16)Text_DecodeGlyphEscape(s);
                if (code == 0xFFFF)
                    n += 2;
                else if (code & 0xFF00)
                    n += 2;
                else
                    n++;
                s += 2;
            } else {
                n++;
                s++;
            }
        } else {
            n += 2;
            s += 2;
        }
    }
    return n;
}
