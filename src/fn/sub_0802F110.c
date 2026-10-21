// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define Actor_OnPathFollowUpdate sub_0802F110
// Provisional name: Actor_OnPathFollowUpdate (tentative)
struct Vec {
    s16 x, y, z, pad;
};
struct Actor;
struct AnimSub {
    u8 pad0[0x1d];
    u8 state;
};
struct Sub100 {
    u8 pad0[0x14];
    u16 flags;
    u8 pad16[7];
    u8 state;
};
struct PathCtx {
    u8 pad0[8];
    u16 *target;
};
#define U8(p, o)  (*(u8 *)((u8 *)(p) + (o)))
#define U16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define U32(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define PTR(p, o) ((void *)((u8 *)(p) + (o)))
int Actor_PlayAnimationEntry(void *, int, int, int, int);
void Actor_PlayFootstepSound(void *, int);
int Math_Atan2(int, int);
int DisplayAnimation_TestFinished(void *);
u32 sub_0821E67C(void *, void *, void *, void *, u32, u32, u32);
void Actor_OnPathFollowUpdate(void *unused, struct Actor *p)
{
    u8 *path = PTR(p, 0x5c);
    register struct PathCtx *ctx asm("r8") = PTR(p, 0x19c);
    struct Vec *pos = PTR(p, 0x68);
    u32 *anim = PTR(p, 0xd4);
    if (U8(p, 9)) {
        int zero = 0;
        U8(p, 9) = zero;
        U8(p, 7) = zero;
        {
            register struct PathCtx *c3 asm("r3") = ctx;
            u16 *t = c3->target;
            int r = Math_Atan2(t[0] - pos->x, t[1] - pos->z);
            u16 a;
            U8(p, 0x65) = r;
            a = r;
            {
                register int t1 asm("r2") = a + 112;
                asm("" : "+r"(t1));
                t1 >>= 5;
                U8(p, 0x1a) = t1 & 7;
            }
            {
                int t2 = a + 96;
                register int k asm("r1");
                asm("" : "+r"(t2));
                t2 >>= 6;
                k = 3;
                k &= t2;
                U8(p, 0x19) = k;
            }
        }
        Actor_PlayAnimationEntry(p, 1, 1, 0, 4);
        U8(p, 0x1f0) = zero;
        U8(p, 0x217) = zero;
    }
    {
        u32 mask = 4096;
        U16(p, 0xe) |= mask;
    }
    if ((u8)(*(int (**)(void *))((u8 *)p + 0x2e4))(p) != 1) {
        if (U8(p, 0x217) == 0) {
            if (*anim == 2) {
                if (((struct Sub100 *)PTR(p, 0x100))->state & 4) {
                    u32 v;
                    if (((struct Sub100 *)PTR(p, 0x100))->flags & 1)
                        v = U16(p, 0x1f4) >> 1;
                    else
                        v = U16(p, 0x1f4);
                    U8(p, 0x217) = sub_0821E67C(ctx, path + 0xc, path + 0x14, path + 9, v, v, v);
                }
            } else {
                u32 v = U16(p, 0x1f4);
                U8(p, 0x217) = sub_0821E67C(ctx, pos, PTR(p, 0x70), PTR(p, 0x65), v, v, v);
            }
        }
        if (U8(p, 0x217)) {
            if (*anim == 2) {
                if (({
                        struct AnimSub *sa = (struct AnimSub *)((u8 *)anim + 0x2c);
                        asm("" : "+r"(sa));
                        sa->state;
                    }) &
                    2)
                    U8(p, 7) = 1;
            } else if (DisplayAnimation_TestFinished(PTR(p, 0xd4)))
                U8(p, 7) = 1;
        }
        Actor_PlayFootstepSound(p, 1);
        Actor_PlayAnimationEntry(p, 1, 1, 0, 1);
    }
}
