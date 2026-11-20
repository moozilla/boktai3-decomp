#include "password.h"

int Text_CopyWithoutTags(u8 *dst, u8 *src, int limit)
{
    int tag = 0, count = 0;
    u32 c;
    while ((c = *src) != 0) {
        s8 first = c;
        if (first >= 0) {
            u8 ascii = first;
            if (ascii == '<') {
                tag = 1;
            } else if (tag == 1 && ascii == '>') {
                tag = 0;
            } else if (!tag) {
                *dst++ = *src;
                if (++count >= limit) {
                    break;
                }
            }
            src++;
        } else {
            tag = 0;
            *dst = c;
            src++;
            dst++;
            if (++count >= limit) {
                break;
            }
            if (!*src) {
                break;
            }
            *dst++ = *src++;
            if (++count >= limit) {
                break;
            }
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

int Password_ValidateNameGlyphs(struct PasswordEntry *e)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (e->decoded.name[i] != 0) {
            int found = 0;
            int t;
            for (t = 0; t < 2; t++) {
                u8 *glyphs = Text_LookupString(Script_ParseStringRef((void *)e->script) + t);
                for (; *glyphs; glyphs += 2) {
                    if (glyphs[1] == e->decoded.name[i]) {
                        found = 1;
                        break;
                    }
                }
            }
            if (found == 0) {
                if (e->decoded.name[i] != 0xFF)
                    return 1;
                e->decoded.name[i] = 0xF0;
            }
        }
    }
    return 0;
}

int NameEntry_DecodeAndValidate(struct PasswordEntry *m)
{
    struct EncodedAlternateRecordFields *decoded = &m->decoded;
    u32 pos = 0;
    m->checksum = EncodedRecord_ReadBits(m->codes, &pos, 16, 6);
    EncodedRecord_ReadBits(m->codes, &pos, 2, 6);
    m->version = EncodedRecord_ReadBits(m->codes, &pos, 3, 6);
    m->key = EncodedRecord_ReadBits(m->codes, &pos, 3, 6);
    if ((Crc_ComputeCcitt16(m, 3, 24) & 65535) != m->checksum)
        return -1;
    EncodedRecord_DecodeAlternateFields(decoded, m->codes, m->key);
    if (m->version != 1)
        return -2;
    if (!m->decoded.name[0] || Password_ValidateNameGlyphs(m))
        return -3;
    return 0;
}

void Password_ApplyDecoded(struct PasswordEntry *m, u8 *output)
{
    struct EncodedAlternateRecordFields *decoded = &m->decoded;
    u8 *name = m->decoded.name;
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
    {
        u16 *field;
        field = &gGameState->values08[0];
        *field = 1;
        field = &gGameState->values08[1];
        *field = decoded->sessionValue;
        field = &gGameState->values08[2];
        *field = decoded->value660;
        field = &gGameState->values08[3];
        *field = decoded->value536;
    }
}

s32 Password_DecodeEntry(u8 *src, s32 tableRef, s32 script, u8 *output)
{
    struct PasswordEntry *e = Heap_Alloc(sizeof(*e));
    s32 result;
    if (e)
        goto initialize;
    return -1;
invalidLength:
    result = -1;
    goto release;
initialize:
    Memory_Clear(e, sizeof(*e));
    e->table = Text_LookupString(Script_ParseStringRef((void *)tableRef));
    e->script = script;
    PASSWORD_REQUIRE(Text_CopyWithoutTags(e->text, src, sizeof(e->text)) == sizeof(e->text),
                     goto invalidLength);
    result = Password_EncodeSymbols(e);
    PASSWORD_REQUIRE(result >= 0, goto release);
    result = NameEntry_DecodeAndValidate(e);
    PASSWORD_REQUIRE(result >= 0, goto release);
    Password_ApplyDecoded(e, output);
release:
    Heap_Free(e);
    return result;
}

void Password_DecodeWordList(void)
{
    u8 *a, *b, *c;
    s32 i;
    PASSWORD_REQUIRE(Script_SeekToKeyword('c'), return);
    a = Script_GetPc();
    PASSWORD_REQUIRE(Script_SeekToKeyword('n'), return);
    b = Script_GetPc();
    PASSWORD_REQUIRE(b != 0, return);
    PASSWORD_REQUIRE(Script_SeekToKeyword('d'), return);
    c = Script_GetPc();
    PASSWORD_REQUIRE(c != 0, return);
    i = 0;
    do {
        u8 *r = Text_LookupString(Script_ParseStringRef(c) + i);
        Password_DecodeEntry(r, (s32)a, (s32)b, gGameState->playerName);
        i++;
    } while (i <= 0x1e49);
}
