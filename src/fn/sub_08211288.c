#include "global.h"
struct Object {
    u8 data[0x2c];
};
struct Entry {
    u8 data[0x60];
};
void Sprite_Remove(void *);
void Anim_Remove(void *);
void sub_08211288(u8 *s)
{
    struct Object *obj = (struct Object *)(s + 0x65c);
    struct Entry *p;
    void *a, *b;
    int i = 26;
    do {
        Sprite_Remove(obj);
        obj++;
        i--;
    } while (i >= 0);
    a = s + 0xa4;
    b = s + 0x44;
    p = (struct Entry *)(s + 0x1c4);
    i = 3;
    do {
        Anim_Remove(p);
        p++;
        i--;
    } while (i >= 0);
    Anim_Remove(s + 0x164);
    Anim_Remove(s + 0x104);
    Anim_Remove(a);
    Anim_Remove(b);
}
