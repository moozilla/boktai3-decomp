// MATCHING_CONCESSION: register-pinning
#include "global.h"
u16 *sub_08036CA4(void);
u16 sub_08036CAC(u8);
u8 sub_08036CC4(u16);
int Script_SeekToKeyword(int);
void *Script_GetPc(void);
int Script_ParseStringRef(void *);
const u8 *Text_LookupString(int);
void sub_08036D80(void)
{
    void *script;
    u16 *table;
    int n;
    register int i asm("r4");
    register int capacity asm("r8");
    if (Script_SeekToKeyword(115))
        script = Script_GetPc();
    else
        script = 0;
    if (!script)
        return;
    table = sub_08036CA4();
    n = 0;
    i = 0;
    capacity = 256;
    do {
        const u8 *text = Text_LookupString(Script_ParseStringRef(script) + i);
        int first = *text;
        u16 *entry;
        i++;
        if (!first)
            continue;
        if (n == capacity)
            continue;
        entry = (u16 *)((n << 1) + (u32)table);
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
            n++;
            first = *text;
            if (!first)
                break;
        } while (n != capacity);
    } while (i <= 4);
    table[0] = 0;
}