#include "password.h"

s32 Password_EncodeSymbols(struct PasswordEntry *s)
{
    u8 *in = s->text;
    s32 k = 2;
    s32 i;
    for (i = 0; i <= 0x17; i++, in += k) {
        u32 a = in[0];
        u32 b = in[1];
        u32 r = GlyphTable_FindPair(s->table, a, b);
        if (r > 0x3f)
            return -1;
        s->codes[i] = r;
    }
    return 0;
}
