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
// MATCHING_CONCESSION: register-pinning
int TextState_MeasureStringLine(struct Vm *arg, int index)
{
    struct Vm *p = arg;
    const u8 *s = p->strings[index];
    int n = 0;
    u32 c;
    goto test;
loop:
    asm("" ::: "memory");
    c = *s;
    if (c == '<') {
        const u8 *q;
        s++;
        if (!*s)
            goto done;
        q = s;
        if (!Text_ComparePrefix(s, gUnk_0824DDF4, 3)) {
            int has = 0;
            int length = 0;
            u8 *text;
            c = *s;
            text = p->text;
            if (c == '>')
                goto parsed;
            if (!c) {
                q = s + 1;
                goto parsed;
            }
            do {
                int ch = *q;
                if (ch == '=')
                    has = 1;
                else if (has)
                    text[length++] = ch;
                q++;
                c = *q;
                if (c == '>')
                    goto parsed;
            } while (c);
            q++;
        parsed:
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
            goto done;
        goto test;
    }
    if (c == 31) {
        u32 code = (u16)Text_DecodeGlyphEscape(s);
        if (code == 0xFFFF)
            goto wide;
        {
            u32 mask = 0xFF;
            asm("" : "+r"(mask));
            mask <<= 8;
            if (mask & code)
                goto wide;
        }
        n++;
        goto wideNext;
    }
    n++;
    s++;
    goto test;
wide:
    n += 2;
wideNext:
    s += 2;
test:
    c = *s;
    c <<= 24;
    if ((s32)c < 0)
        goto wide;
    c >>= 24;
    if (!c)
        goto done;
    if (c != 10)
        goto loop;
done:
    return n;
}
