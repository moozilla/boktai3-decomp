#include "global.h"
#define SceneActors_ApplyDamage sub_08021450
void sub_08249248(void *, int, int);
int sub_0824924C(void *, int, int);
int SceneActors_ApplyDamage(u8 *p, int kind, int amount)
{
    u8 *actor = *(u8 **)(p + 0x50);
    int alive = 1;
    if (!kind) {
        int damage = amount - *(int *)(actor + 0x158);
        if (damage < 0)
            damage = 0;
        *(u16 *)(actor + 0x94) -= damage;
        if (*(s16 *)(actor + 0x94) <= 0) {
            if (*(int *)(actor + 0x170))
                (*(int (**)(void *, int, int))(actor + 0x170))(actor, 1, damage);
            alive = 0;
        } else if (*(int *)(actor + 0x170)) {
            if (!(*(int (**)(void *, int, int))(actor + 0x170))(actor, 0, damage))
                *(u16 *)(actor + 0x94) += damage;
        }
    } else if (*(int *)(actor + 0x170))
        (*(int (**)(void *, int, int))(actor + 0x170))(actor, 2, amount);
    return alive;
}
