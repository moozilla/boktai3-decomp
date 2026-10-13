#include "global.h"
struct Entry {
    u8 data[0x60];
};
void Sprite_Remove(void *);
void Task_Kill(void *);
void Anim_Remove(void *);
void sub_08212870(u8 *s)
{
    struct Entry *p;
    void *a, *b;
    int i;
    Sprite_Remove(s + 0xb10);
    Task_Kill(*(void **)(s + 0xb5c));
    Task_Kill(*(void **)(s + 0xb58));
    Anim_Remove(s + 0xa88);
    p = (struct Entry *)(s + 0x3a8);
    i = 17;
    do {
        Anim_Remove(p);
        p++;
        i--;
    } while (i >= 0);
    a = s + 0xc4;
    b = s + 0x64;
    p = (struct Entry *)(s + 0x1e4);
    i = 3;
    do {
        Anim_Remove(p);
        p++;
        i--;
    } while (i >= 0);
    Anim_Remove(s + 0x184);
    Anim_Remove(s + 0x124);
    Anim_Remove(a);
    Anim_Remove(b);
}
