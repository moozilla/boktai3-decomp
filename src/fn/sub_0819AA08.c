#define SceneEffect_UpdateEntryState  sub_0819AA08
#define SceneEffect_CreateLinkedEntry sub_0819A7B8
#define SceneEffect_SetEntryState SceneBurstEmitter_TransitionIfComplete
#include "global.h"
struct SceneEffectLink {
    u8 unknown[0x38];
    u16 frame;
    u8 state;
};
void sub_0819A7B8(struct SceneEffectLink *);
void sub_0819A814(struct SceneEffectLink *);
void sub_0819A954(struct SceneEffectLink *);
void SceneBurstEmitter_TransitionIfComplete(struct SceneEffectLink *, u32);
s32 sub_0819AA08(struct SceneEffectLink *p)
{
    switch (p->state) {
        case 0:
        default:
            break;
        case 1:
            sub_0819A7B8(p);
            SceneBurstEmitter_TransitionIfComplete(p, 2);
            break;
        case 2:
            sub_0819A814(p);
            SceneBurstEmitter_TransitionIfComplete(p, 3);
            break;
        case 3:
            sub_0819A954(p);
            SceneBurstEmitter_TransitionIfComplete(p, 0);
            break;
    }
    p->frame++;
    return 0;
}
