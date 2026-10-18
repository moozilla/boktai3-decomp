#include "global.h"
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
void SceneActorCollision_InitBodyWithFlags(u8 *, struct Z *, struct Z *, void (*)(void), u8 *, u32);
void SceneActorCollision_InitAttack(u8 *, struct Z *, struct Z *, void (*)(void), u32, u8 *);
void SceneActorContact_ApplyWithinBounds(void);
void Spider_OnAttackCollision(void);
void Spider_InitColliders(u8 *p)
{
    struct Z s1, s2, s3, s4;
    s1.a.lo = 0xa0;
    s1.a.hi = 0x80;
    s1.b.lo = 0xa0;
    *(u32 *)&s2.a = 0x800000;
    *(u32 *)&s2.b &= 0xFFFF0000;
    SceneActorCollision_InitBodyWithFlags(p, &s1, &s2, SceneActorContact_ApplyWithinBounds, p, 0x510B);
    s3.a.lo = 0x40;
    s3.a.hi = 0x64;
    s3.b.lo = 0x40;
    *(u32 *)&s4.a = 0x800000;
    *(u32 *)&s4.b &= 0xFFFF0000;
    SceneActorCollision_InitAttack(p, &s3, &s4, Spider_OnAttackCollision, 0, p);
}
