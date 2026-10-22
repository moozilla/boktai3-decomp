#include "global.h"
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Tail {
    u32 lo : 16;
    u32 : 16;
};
struct Z {
    struct H a;
    struct Tail b;
};
void SceneActorCollision_InitBody(u8 *, struct Z *, struct Z *, void (*)(void), u8 *);
void SceneActorCollision_InitAttack(u8 *, struct Z *, struct Z *, void (*)(void), u32, u8 *);
void Golem_OnBodyCollision(void);
void Golem_OnAttackCollision(void);
void Golem_InitColliders(u8 *p)
{
    struct Z s1, s2, s3, s4;
    s1.a.lo = 0x40;
    s1.a.hi = 0x80;
    s1.b.lo = 0x40;
    s2.a.lo = 0;
    s2.a.hi = 0x80;
    s2.b.lo = 0;
    SceneActorCollision_InitBody(p, &s1, &s2, Golem_OnBodyCollision, p);
    s3.a.lo = 0x40;
    s3.a.hi = 0x78;
    s3.b.lo = 0x40;
    s4.a.lo = 0;
    s4.a.hi = 0x80;
    s4.b.lo = 0;
    SceneActorCollision_InitAttack(p, &s3, &s4, Golem_OnAttackCollision, 0, p);
}
