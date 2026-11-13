#include "wireless_text.h"

u16 *WirelessText_GetCharacterTable(void)
{
    return (u16 *)0x0203FC00;
}

u16 WirelessText_GetCharacterCode(u8 i)
{
    return WirelessText_GetCharacterTable()[i];
}

u8 WirelessText_FindCharacterIndex(u32 character)
{
    u16 code = character;
    u16 *base = WirelessText_GetCharacterTable();
    u16 *table;
    int i = 0;
    int result;
    table = base;
    do {
        if (*table == code) {
            {
                u32 narrowed = i << 24;
                result = narrowed >> 24;
            }
            goto done;
        }
        table++;
    } while (++i <= 255);
    result = 0;
done:
    return result;
}

void WirelessText_DecodeString(const u8 *source, u8 *destination, int count)
{
    int i = 0;
    while (i < count) {
        u16 code = WirelessText_GetCharacterCode(source[i]);
        if (code <= 127) {
            *destination++ = code;
        } else {
            destination[0] = code;
            destination[1] = code >> 8;
            destination += 2;
        }
        i++;
    }
    *destination = 0;
}

void WirelessText_EncodeString(const u8 *source, u8 *destination, int count)
{
    const u8 **alias;
    int i = 0;
    while (i < count) {
        u32 first = *source;
        u32 code;
        if (!first) {
            break;
        }
        first <<= 24;
        alias = &source;
        if (((s32)first) >= 0) {
            code = *(source++);
        } else {
            {
                int high;
                code = source[0];
                high = (*alias)[1] << 8;
                code |= high;
            }
            source += 2;
        }
        destination[i] = WirelessText_FindCharacterIndex(code);
        i++;
    }

    while (i < count) {
        destination[i++] = 0;
    }

    destination[count] = 0;
}

void WirelessText_InitCharacterTableFromScript(void)
{
    void *script;
    u16 *table;
    struct {
        int stringIndex, characterCount;
    } progress;
    int capacity;
    if (Script_SeekToKeyword('s')) {
        script = Script_GetPc();
    } else {
        script = 0;
    }
    if (!script) {
        return;
    }
    table = WirelessText_GetCharacterTable();
    progress.characterCount = 0;
    progress.stringIndex = 0;
    capacity = 256;
    do {
        const u8 *text = Text_LookupString(Script_ParseStringRef(script) + progress.stringIndex);
        int first = *text;
        u16 *entry;
        progress.stringIndex++;
        if (!first) {
            continue;
        }
        if (progress.characterCount == capacity) {
            continue;
        }
        entry = (u16 *)((progress.characterCount * sizeof(*table)) + (u32)table);
        do {
            int sign = first << 24;
            if (sign >= 0) {
                *entry = *text;
                text++;
            } else {
                int high = text[1] << 8;
                int code = text[0] | high;
                *entry = code;
                text += 2;
            }
            entry++;
            progress.characterCount++;
            first = *text;
            if (!first) {
                break;
            }
        } while (progress.characterCount != capacity);
    } while (progress.stringIndex <= 4);
    table[0] = 0;
}
