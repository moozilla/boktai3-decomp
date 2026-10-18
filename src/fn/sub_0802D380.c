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
void Actor_ShowDamageNumber(struct Object *, int, int, struct Vec *);
void Actor_SubtractHealth(struct Object *, int);
void Actor_StartDamageFlash(struct Object *);
void TransientSpriteEffects_Spawn(int, int, struct Vec *, int, int, int);
int Actor_OnDamage(struct Object *p)
{
    int damage = p->damage;
    if (damage > 0) {
        struct Vec size;
        size.x = 16;
        size.y = 0;
        size.z = 16;
        Actor_ShowDamageNumber(p, damage, 0, &size);
        Actor_SubtractHealth(p, damage);
        if (p->health <= 0) {
            TransientSpriteEffects_Spawn(0, 0, &p->pos, 0, 256, 256);
            return 1;
        }
        Actor_StartDamageFlash(p);
    }
    return 0;
}
