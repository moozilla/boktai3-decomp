#include "password.h"
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
