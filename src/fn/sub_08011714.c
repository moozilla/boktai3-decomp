#include "password.h"
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
