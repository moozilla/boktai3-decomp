#include "global.h"

struct S {
    u8 pad[0x348];
    s32 a;
    s32 b;
    s32 c;
    u32 d;
};

void Menu_SetBackgroundMap(u32 p, u32 b);
void sub_081DCFE4(struct S *s);

void NameEntry_ToggleCharacterPage(struct S *s)
{
    if (s->a == 0) {
        if (s->b == 0x4f) {
            s->b = 0x4e;
        }
        s->a = 1;
        Menu_SetBackgroundMap(s->d, 1);
    } else {
        s->a = 0;
        Menu_SetBackgroundMap(s->d, 0);
    }
    sub_081DCFE4(s);
}
