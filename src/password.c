#include "password.h"

// MATCHING_CONCESSION: register-pinning
int Text_CopyWithoutTags(u8 *dst, u8 *src, int limit)
{
    int tag = 0, count = 0;
    u32 c;
    while ((c = *src) != 0) {
        {
            register u32 held asm("r3") = c;
            asm("" : "+r"(held));
            c = held;
        }
        if ((s8)c >= 0) {
            if ((u8)c == '<')
                tag = 1;
            else if (tag == 1 && (u8)c == '>')
                tag = 0;
            else if (!tag) {
                *dst++ = *src;
                if (++count >= limit)
                    break;
            }
            src++;
        } else {
            tag = 0;
            *dst = c;
            src++;
            dst++;
            if (++count >= limit)
                break;
            if (!*src)
                break;
            *dst++ = *src++;
            if (++count >= limit)
                break;
        }
    }
    return count;
}

s32 GlyphTable_FindPair(u8 *p, u32 a, u32 b)
{
    s32 i = 0;
    while (i <= 0x3F) {
        if (p[0] == a && p[1] == b) {
            return i;
        }
        i++;
        p += 2;
    }
    return -1;
}

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

#ifdef NONMATCHING

// fuzzy: 79.7%
// register allocation differs (name/q/found) and the inner glyph-search loop is rotated differently (original jumps into the test)
int sub_080115DC(struct PasswordEntry *e)
{
    int i;
    u8 *name;
    for (i = 0, name = e->name; i < 5; i++, name++) {
        if (*name != 0) {
            int found = 0;
            int j;
            int *sp = &e->script;
            u8 *q = name;
            for (j = 0; j < 2; j++) {
                u8 *s = Text_LookupString(Script_ParseStringRef((void *)*sp) + j);
                u32 c = s[0];
                while (c) {
                    if (s[1] != *q) {
                        s += 2;
                        c = s[0];
                        continue;
                    }
                    found = 1;
                    break;
                }
            }
            if (found == 0) {
                if (*name != 0xFF)
                    return 1;
                *name = 0xF0;
            }
        }
    }
    return 0;
}

#else

INCLUDE_ASM("asm/nonmatching", sub_080115DC);

#endif

int NameEntry_DecodeAndValidate(struct PasswordEntry *m)
{
    u8 *decoded = m->decoded;
    int pos = 0;
    m->checksum = sub_08044CF4(m, &pos, 16, 6);
    sub_08044CF4(m, &pos, 2, 6);
    m->version = sub_08044CF4(m, &pos, 3, 6);
    m->key = sub_08044CF4(m, &pos, 3, 6);
    if ((sub_08221594(m, 3, 24) & 65535) != m->checksum)
        return -1;
    sub_080453DC(decoded, m, m->key);
    if (m->version != 1)
        return -2;
    if (!m->name[0] || sub_080115DC(m))
        return -3;
    return 0;
}

void Password_ApplyDecoded(struct PasswordEntry *m, u8 *output)
{
    u32 *decoded = (u32 *)m->decoded;
    u8 *name = m->name;
    u8 *out = output;
    int i = 0;
    while (i < 5 && *name) {
        if (*name == 240) {
            u8 *s = Text_LookupString(Script_ParseStringRef((void *)m->script));
            out[0] = s[160];
            out[1] = s[161];
        } else {
            out[0] = 128;
            out[1] = *name;
        }
        i++;
        out += 2;
        name++;
    }
    *out = 0;
    *(u16 *)(gUnk_02000710 + 8) = 1;
    *(u16 *)(gUnk_02000710 + 10) = decoded[0];
    *(u16 *)(gUnk_02000710 + 12) = decoded[1];
    *(u16 *)(gUnk_02000710 + 14) = decoded[10];
}

s32 sub_08011794(u8 *src, s32 tableRef, s32 script, u8 *output)
{
    struct PasswordEntry *e = Heap_Alloc(0x90);
    s32 result;
    if (!e)
        return -1;
    Memory_Clear(e, 0x90);
    e->table = Text_LookupString(Script_ParseStringRef((void *)tableRef));
    e->script = script;
    do {
        if (Text_CopyWithoutTags(e->text, src, 0x30) != 0x30) {
            result = -1;
            break;
        }
        result = Password_EncodeSymbols(e);
        if (result < 0)
            break;
        result = NameEntry_DecodeAndValidate(e);
        if (result < 0)
            break;
        Password_ApplyDecoded(e, output);
    } while (0);
    Heap_Free(e);
    return result;
}

void sub_08011814(void)
{
    u8 *a, *b, *c;
    s32 i;
    if (!Script_SeekToKeyword(0x63))
        return;
    a = Script_GetPc();
    if (!Script_SeekToKeyword(0x6e))
        return;
    b = Script_GetPc();
    if (b == 0)
        return;
    if (!Script_SeekToKeyword(0x64))
        return;
    c = Script_GetPc();
    if (c == 0)
        return;
    i = 0;
    do {
        u8 *r = Text_LookupString(Script_ParseStringRef(c) + i);
        sub_08011794(r, (s32)a, (s32)b, gUnk_02000710 + 0x780);
        i++;
    } while (i <= 0x1e49);
}
