#include "global.h"
struct Object {
    u8 data[0x2c];
};
struct Entry {
    u8 data[0x60];
};
void sub_08033590(void);
void Sprite_Remove(void *);
void Anim_Remove(void *);
void sub_08213810(u8 *s)
{
    struct Object *obj = (struct Object *)(s + 0x500);
    struct Entry *p;
    void *a, *b;
    int i = 9;
    do {
        Sprite_Remove(obj);
        obj++;
        i--;
    } while (i >= 0);
    Anim_Remove(s + 0x364);
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
    sub_08033590();
}
