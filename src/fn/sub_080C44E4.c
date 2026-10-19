#include "global.h"
/* Actor_StepTowardTargetAxis: steps toward a target point along one axis (the larger delta, or the current facing on ties); turns first if the axis changes */
#define Actor_StepTowardTargetAxis sub_080C44E4

struct Unk080C44E4State {
    u8 unk000[0x68E];
    u8 angle;
};
struct Unk080C44E4Obj {
    u8 unk00[0x59];
    u8 direction;
    u8 unk5A[2];
    s16 x;
    u8 unk5E[2];
    s16 z;
    u8 unk62[2];
    u16 posX;
    u8 unk66[2];
    u16 posZ;
    u8 unk6A[0x3D0 - 0x6A];
    struct Unk080C44E4State *state;
};
struct Unk080C44E4Target {
    u8 unk00[0x1C];
    s16 x;
    u8 unk1E[2];
    s16 z;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
static inline s32 Abs(s32 x)
{
    return x < 0 ? -x : x;
}

u32 Actor_StepTowardTargetAxis(struct Unk080C44E4Obj *obj, struct Unk080C44E4Target *target,
                               u32 nearX, u32 nearZ, s32 speed)
{
    struct Unk080C44E4State *state = obj->state;
    s32 dx = target->x - obj->x;
    s32 dz = target->z - obj->z;
    u8 angle;

    if ((u32)Abs(dx) <= nearX && (u32)Abs(dz) <= nearZ) {
        if (Abs(dx) > Abs(dz))
            dz = 0;
        else
            dx = 0;
        state->angle = Math_Atan2(dx, dz);
        obj->posX += (gUnk_086149C4[(state->angle + 0x40) & 0xFF] * speed) >> 16;
        obj->posZ += (gUnk_086149C4[state->angle] * speed) >> 16;
        return 1;
    }
    if (dx || dz) {
        if (Abs(dx) > Abs(dz))
            goto zero_dz;
        if (Abs(dz) > Abs(dx))
            goto zero_dx;
        switch (((state->angle + 0x20) & 0xFF) >> 6) {
            case 0:
            case 2:
            zero_dz:
                dz = 0;
                break;
            default:
            zero_dx:
                dx = 0;
                break;
        }
        angle = Math_Atan2(dx, dz);
        if (angle != state->angle) {
            obj->direction = angle;
            state->angle = angle;
            return 1;
        }
        obj->posX += (gUnk_086149C4[(angle + 0x40) & 0xFF] * speed) >> 16;
        obj->posZ += (gUnk_086149C4[angle] * speed) >> 16;
        obj->direction = angle;
        state->angle = angle;
    }
    return 0;
}
