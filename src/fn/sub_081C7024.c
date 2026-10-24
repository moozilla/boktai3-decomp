#include "global.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 85.4%
struct Vec {
    s16 x, y, z, pad;
};
struct StateActor {
    u8 pad0, kind;
    u8 pad2[5];
    u8 done, active, entered;
    u8 padA[0x18 - 0xA];
    u8 phase, facing4, facing8, angle;
    u8 pad1C[0x54 - 0x1C];
    struct Vec target;
    u8 pad5C[9];
    u8 moveAngle;
    u8 pad66[2];
    struct Vec position, velocity;
    u8 pad78[0x208 - 0x78];
    s32 speed;
    u32 stopDistance;
};
extern const s16 gUnk_086149C4[];
void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);
void Actor_PlayFootstepSound(void *, s32);
s32 Math_Atan2(s32, s32);
static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}
static inline void Polar(struct Vec *v, u32 angle, s32 speed)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle] * speed);
}
// confidence=low: turns toward a target and stops inside its squared-distance threshold.
#define StateActor_UpdateTargetApproach sub_081C7024
void StateActor_UpdateTargetApproach(void *context, struct StateActor *input)
{
    register struct StateActor *p asm("r5") = input;
    u32 r;
    s32 dx, dz;
    register struct Vec *position asm("r8");
    struct Vec *target;
    if (p->entered) {
        p->entered = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
                r = 0;
                break;
            case 27:
                r = 9;
                break;
            case 28:
            case 29:
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 0, 4);
        p->phase = 0;
    }
    position = &p->position;
    target = &p->target;
    dx = target->x - position->x;
    dz = target->z - position->z;
    if ((u32)dx * dx + (u32)dz * dz <= p->stopDistance) {
        p->position = p->target;
        p->done = 1;
    } else {
        register u8 *angle asm("sb") = &p->moveAngle;
        Polar(&p->velocity, *angle, p->speed);
        p->angle = Math_Atan2(target->x - position->x, target->z - position->z);
        *angle = p->angle;
        p->facing8 = ((p->angle + 0x70) >> 5) & 7;
        p->facing4 = ((p->angle + 0x60) >> 6) & 3;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
                r = 0;
                break;
            case 27:
                r = 9;
                break;
            case 28:
            case 29:
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 0, 1);
    }
    Actor_PlayFootstepSound(p, 3);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081C7024);
#endif
