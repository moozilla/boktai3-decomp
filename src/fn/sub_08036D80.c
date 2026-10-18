#include "global.h"
u16 *WirelessText_GetCharacterTable(void);
u16 WirelessText_GetCharacterTableByte(u8);
u8 WirelessText_FindCharacterIndex(u16);
int Script_SeekToKeyword(int);
void *Script_GetPc(void);
int Script_ParseStringRef(void *);
const u8 *Text_LookupString(int);
void WirelessText_InitCharacterTableFromScript(void)
{
    void *script;
    u16 *table;
    int alias[2];
    int capacity;
    if (Script_SeekToKeyword(115)) {
        script = Script_GetPc();
    } else {
        script = 0;
    }
    if (!script) {
        return;
    }
    table = WirelessText_GetCharacterTable();
    alias[1] = 0;
    alias[0] = 0;
    capacity = 256;
    /* do-while(0) only keeps agbcc's block layout */
    do {
        const u8 *text = Text_LookupString(Script_ParseStringRef(script) + alias[0]);
        int first = *text;
        u16 *entry;
        alias[0]++;
        if (!first) {
            continue;
        }
        if (alias[1] == capacity) {
            continue;
        }
        entry = (u16 *)((alias[1] << 1) + ((u32)table));
        /* do-while(0) only keeps agbcc's block layout */
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
            alias[1]++;
            first = *text;
            if (!first) {
                break;
            }
        } while (alias[1] != capacity);
    } while (alias[0] <= 4);
    table[0] = 0;
}