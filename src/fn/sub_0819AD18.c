#define SceneEffect_SetMovingState SceneOrbitHazard_TransitionIfComplete
#include "global.h"
struct SceneEffectMoving {
    u8 unknown[0xDE];
    u16 frame;
    u8 state;
};
void SceneOrbitHazard_UpdateOrbit(struct SceneEffectMoving *);
void SceneOrbitHazard_UpdateTravel(struct SceneEffectMoving *);
void SceneOrbitHazard_TransitionIfComplete(struct SceneEffectMoving *, u32);
s32 SceneEffect_UpdateMovingState(struct SceneEffectMoving *p)
{
    switch (p->state) {
        case 0:
        default:
            break;
        case 1:
            SceneOrbitHazard_UpdateOrbit(p);
            SceneOrbitHazard_TransitionIfComplete(p, 2);
            break;
        case 2:
            SceneOrbitHazard_UpdateTravel(p);
            SceneOrbitHazard_TransitionIfComplete(p, 0);
            break;
    }
    p->frame++;
    return 0;
}
