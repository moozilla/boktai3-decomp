#define SceneEffect_UpdateMovingState sub_0819AD18
#define SceneEffect_SetMovingState SceneOrbitHazard_TransitionIfComplete
#include "global.h"
struct SceneEffectMoving {
    u8 unknown[0xDE];
    u16 frame;
    u8 state;
};
void sub_0819AB90(struct SceneEffectMoving *);
void sub_0819AC1C(struct SceneEffectMoving *);
void SceneOrbitHazard_TransitionIfComplete(struct SceneEffectMoving *, u32);
s32 sub_0819AD18(struct SceneEffectMoving *p)
{
    switch (p->state) {
        case 0:
        default:
            break;
        case 1:
            sub_0819AB90(p);
            SceneOrbitHazard_TransitionIfComplete(p, 2);
            break;
        case 2:
            sub_0819AC1C(p);
            SceneOrbitHazard_TransitionIfComplete(p, 0);
            break;
    }
    p->frame++;
    return 0;
}
