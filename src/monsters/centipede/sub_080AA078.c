#include "global.h"
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
void SceneActorCollision_InitBody(u8 *, struct Z *, struct Z *, void (*)(void), u8 *);
void SceneActorCollision_InitAttack(u8 *, struct Z *, struct Z *, void (*)(void), u32, u8 *);
void TentacleBody_OnBodyContact(void);
void TentacleBody_OnAttackCollision(void);
void TentacleActor_InitCollision(u8 *p)
{
    struct Z s1, s2, s3, s4;
    s1.a.lo = 0x40;
    s1.a.hi = 0x80;
    s1.b.lo = 0x40;
    *(u32 *)&s2.a = 0x800000;
    *(u32 *)&s2.b &= 0xFFFF0000;
    SceneActorCollision_InitBody(p, &s1, &s2, TentacleBody_OnBodyContact, p);
    s3.a.lo = 0x40;
    s3.a.hi = 0x64;
    s3.b.lo = 0x40;
    *(u32 *)&s4.a = 0x800000;
    *(u32 *)&s4.b &= 0xFFFF0000;
    SceneActorCollision_InitAttack(p, &s3, &s4, TentacleBody_OnAttackCollision, 0, p);
}
