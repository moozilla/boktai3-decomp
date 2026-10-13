#include "global.h"
extern u8 *gUnk_02000710;
int Script_ParseStringRef(int);
u8 *Text_LookupString(int);
void Password_ApplyDecoded(u8 *m, u8 *output)
{
    u32 *decoded = (u32 *)(m + 0x48);
    u8 *name = m + 0x74;
    u8 *out = output;
    int i = 0;
    while (i < 5 && *name) {
        if (*name == 240) {
            u8 *s = Text_LookupString(Script_ParseStringRef(*(int *)(m + 0x80)));
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
