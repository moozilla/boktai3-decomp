#include "global.h"

#ifdef NONMATCHING
// fuzzy: 85.6%
// Remaining differences: instruction ordering and register allocation.
#include "scene_actor_reset.h"
extern u8 *gUnk_02000580[];
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
void sub_08098344(void *, void *);
void sub_080980FC(void *);
s32 Math_Atan2(s32, s32);
s32 Mod(s32, s32);
static inline u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline s32 Scale(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}
static inline void Polar(struct ContactPosition *p, u8 angle, s32 speed)
{
    p->x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    p->y = 0;
    p->z = Scale(gUnk_086149C4[angle & 255] * speed);
}
static inline u32 Rand(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return gUnk_0203B400[gUnk_03005308];
}
static inline void Delta(struct ContactPosition *o, struct ContactPosition *a,
                         struct ContactPosition *b)
{
    o->x = a->x - b->x;
    o->y = a->y - b->y;
    o->z = a->z - b->z;
}
// confidence=low: Ghost action table movement callback follows a randomized loop around the player.
void Ghost_FollowPlayerLoop(struct SceneActorRegistryResetView *a)
{
    u8 *ctx = a->context;
    struct ContactPosition v, dest;
    u32 entered = TakeFlag(&a->field2B1);
    struct ContactPosition *pos = &a->position;
    if (entered) {
        void *callback = a->entryTarget228;
        u32 one = 1;
        a->field2B2 = one;
        a->field2B0 = 0;
        a->field2B5 = one;
        a->target = callback;
        a->field2C8 = 0;
        {
            u32 *flags = &a->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        a->word104 = 0;
        ctx[0x5B0] = 0;
        dest = *(struct ContactPosition *)(gUnk_02000580[0] + 0x30);
        Polar(&v, (u8)Rand(), 512);
        dest.x += v.x;
        dest.z += v.z;
        Delta(&v, &dest, pos);
        Polar((void *)(ctx + 0x5FC), (Rand() & 63) + a->fieldE9, 32);
        sub_08098344(a, &dest);
        *(u16 *)(ctx + 0x566) = 1;
    }
    Delta(&v, (void *)(ctx + 0x568 + ctx[0x5B0] * 8), pos);
    sub_080980FC(a);
    if (v.x * v.x + v.z * v.z > 1024) {
        struct ContactPosition *target = (void *)(ctx + 0x568 + ctx[0x5B0] * 8);
        a->direction = Math_Atan2(*(s16 *)(ctx + 0x568 + ctx[0x5B0] * 8) - a->position.x,
                                  *(s16 *)(ctx + 0x56C + ctx[0x5B0] * 8) - a->position.z);
        Polar(&v, a->direction, *(u16 *)(ctx + 0x566));
        a->motion.x += v.x;
        a->motion.z += v.z;
        *(struct ContactPosition *)(ctx + 0x5FC) = a->motion;
    } else {
        if (++ctx[0x5B0] > 8) {
            Delta((void *)(ctx + 0x5FC), (void *)(ctx + 0x5A8), pos);
            *(u16 *)(ctx + 0x5FE) = 0;
            pos->x = *(u16 *)(ctx + 0x5A8);
            pos->z = *(u16 *)(ctx + 0x5AC);
            ctx[0x5B0] = 0;
            dest = *(struct ContactPosition *)(gUnk_02000580[0] + 0x30);
            Polar(&v, (Rand() & 63) + a->fieldE9, 512);
            dest.x += v.x;
            dest.z += v.z;
            Delta(&v, &dest, pos);
            sub_08098344(a, &dest);
            *(u16 *)(ctx + 0x566) >>= 1;
            return;
        }
    }
    if (*(u16 *)((u8 *)a + 0x1E6) == 0) {
        if (Mod(a->counter2AC, 16) == 0 && *(u16 *)(ctx + 0x566) <= 11)
            ++*(u16 *)(ctx + 0x566);
    } else {
        if (Mod(a->counter2AC, 8) == 0 && *(u16 *)(ctx + 0x566) <= 11)
            *(u16 *)(ctx + 0x566) += 2;
    }
    a->counter2AC++;
}

#else
INCLUDE_ASM("asm/nonmatching", Ghost_FollowPlayerLoop);
#endif
