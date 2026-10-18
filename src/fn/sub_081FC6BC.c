#include "global.h"
struct SceneProjectile {
    u32 age;
    u32 flags;
    u8 pad08[0x22];
    u16 lifetime;
    u8 pad2C[3];
    s8 pulse;
    s32 phase;
};
s32 Mod(s32, s32);
void sub_081FD4C0(void *);
s32 SceneProjectile_UpdatePulse(struct SceneProjectile *p)
{
    if (p->age > p->lifetime)
        return 1;
    if (Mod(p->age, 2) == 0)
        sub_081FD4C0(p);
    if (p->age == 1) {
        s32 v = p->pulse >> 2;
        p->pulse = v;
        if (v <= 0)
            p->pulse = 1;
    } else if (p->age == 30) {
        p->pulse <<= 2;
    } else if (p->age == 90 || p->age == 120) {
        s32 v = p->pulse >> 1;
        p->pulse = v;
        if (v <= 0)
            p->pulse = 1;
    }
    if (p->age < 120) {
        if (Mod(p->age, 10) == 0)
            p->phase--;
    }
    if (p->age >= 120) {
        if (Mod(p->age, 10) == 0)
            p->phase += 2;
    }
    {
        u32 mask = 2;
        p->flags = mask | p->flags;
    }
    return 0;
}
