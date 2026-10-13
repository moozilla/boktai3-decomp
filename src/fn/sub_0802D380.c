#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Object {
    u8 pad0[0xc];
    struct Vec pos;
    u8 pad14[0x38];
    int health;
    u8 pad50[8];
    int damage;
};
void sub_0802D020(struct Object *, int, int, struct Vec *);
void sub_0802C954(struct Object *, int);
void sub_0802D040(struct Object *);
void sub_080474E4(int, int, struct Vec *, int, int, int);
int sub_0802D380(struct Object *p)
{
    int damage = p->damage;
    if (damage > 0) {
        struct Vec size;
        size.x = 16;
        size.y = 0;
        size.z = 16;
        sub_0802D020(p, damage, 0, &size);
        sub_0802C954(p, damage);
        if (p->health <= 0) {
            sub_080474E4(0, 0, &p->pos, 0, 256, 256);
            return 1;
        }
        sub_0802D040(p);
    }
    return 0;
}
