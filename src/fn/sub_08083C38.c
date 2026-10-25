#include "global.h"
#include "scene_actor_control_helpers.h"
#include "engine.h"

typedef struct {
    u32 a, b;
} Sub08083C38Pair;

static inline s32 Sub08083C38Div1024(s32 x)
{
    return x >= 0 ? x >> 10 : -((-x) >> 10);
}

#ifdef NONMATCHING
// fuzzy: 48.9%
// scope shape 90 (differences once low registers are set aside)
// confidence=low: one step of the position curve: writes start + Hermite offset (end, sway tangent) to the point at 0x3FC; returns 1 when field3E0 runs past 0x3FF.
s32 sub_08083C38(struct SceneActorRegistryResetView *arg0)
{
    struct SceneActorControlMotionState *state;
    struct {
        u32 fill;
        Sub08083C38Pair start;
        Sub08083C38Pair end;
        struct ContactPosition delta;
        struct ContactPosition origin;
    } tmp;
    u16 *startW;
    u16 *endW;
    u16 *point;
    s32 sway;
    s32 v, m1, m2, c0, h01, tan;
    s32 next;
    u32 mask;

    state = SceneActorControl_GetMotionState(arg0);
    tmp.start = *(Sub08083C38Pair *)&state->vectors[0];
    tmp.end = *(Sub08083C38Pair *)&state->vectors[1];
    mask = 0xFFFF;
    tmp.start.a &= mask;
    tmp.end.a &= mask;
    startW = (u16 *)&tmp.start;
    endW = (u16 *)&tmp.end;
    point = (u16 *)((u8 *)state + 0x3FC);
    v = state->field3E0;
    sway = state->sway;
    tmp.delta.x = endW[0] - startW[0];
    tmp.delta.y = endW[1] - startW[1];
    tmp.delta.z = endW[2] - startW[2];
    tmp.fill = 0;
    CpuSet(&tmp.fill, &tmp.origin, 0x05000002);

    m1 = Sub08083C38Div1024(v * v);
    m2 = Sub08083C38Div1024(m1 * v);
    c0 = 2 * m2 - 3 * m1 + 1;
    h01 = 3 * m1 - 2 * m2;
    tan = sway * (m2 - 2 * m1 + v);
    point[0] = startW[0] + Sub08083C38Div1024(tmp.origin.x * c0 + tmp.delta.x * h01 + tan);
    point[1] = startW[1] + Sub08083C38Div1024(tmp.origin.y * c0 + tmp.delta.y * h01 + tan);
    point[2] = startW[2] + Sub08083C38Div1024(tmp.origin.z * c0 + tmp.delta.z * h01 + tan);

    if (state->field3E0 > 0x3FF) {
        state->field3E0 = 0;
        return 1;
    }
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    next = state->field3E0 + *(u8 *)(gUnk_03005308 * 2 + (u32)gUnk_0203B400) + 0x100;
    state->field3E0 = next;
    if ((u16)next > 0x400) {
        state->field3E0 = 0x400;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08083C38);
#endif
