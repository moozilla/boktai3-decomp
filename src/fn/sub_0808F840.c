#include "global.h"
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
struct W {
    u16 a, b, c;
};
void SceneActorCollision_InitBody(u8 *, struct Z *, struct Z *, void (*)(void), u8 *);
void SceneActorCollision_InitAttack(u8 *, struct Z *, struct Z *, void (*)(void), u32, u8 *);
void Slime_ResolveBodyContact(void);
void Slime_ApplyBodyHit(void);
void Slime_InitColliders(u8 *p)
{
    struct Z s1, s2;
    s1.a.lo = 0x40;
    s1.a.hi = 0x80;
    s1.b.lo = 0x40;
    *(u32 *)&s2.a = 0x800000;
    *(u32 *)&s2.b &= 0xFFFF0000;
    SceneActorCollision_InitBody(p, &s1, &s2, Slime_ResolveBodyContact, p);
    ((struct W *)&s1)->a = 0x40;
    ((struct W *)&s1)->b = 0x64;
    ((struct W *)&s1)->c = 0x40;
    ((struct W *)&s2)->a = 0;
    ((struct W *)&s2)->b = 0x80;
    ((struct W *)&s2)->c = 0;
    SceneActorCollision_InitAttack(p, &s1, &s2, Slime_ApplyBodyHit, 0, p);
}
