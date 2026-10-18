#define SceneEffect_SetEntryState SceneBurstEmitter_TransitionIfComplete
#include "global.h"
struct SceneEffectLink {
    u8 unknown[0x38];
    u16 frame;
    u8 state;
};
void SceneEffect_CreateLinkedEntry(struct SceneEffectLink *);
void LinkEffect_UpdateProjectileBurst(struct SceneEffectLink *);
void LinkEffect_UpdateSecondOrb(struct SceneEffectLink *);
void SceneBurstEmitter_TransitionIfComplete(struct SceneEffectLink *, u32);
s32 SceneEffect_UpdateEntryState(struct SceneEffectLink *p)
{
    switch (p->state) {
        case 0:
        default:
            break;
        case 1:
            SceneEffect_CreateLinkedEntry(p);
            SceneBurstEmitter_TransitionIfComplete(p, 2);
            break;
        case 2:
            LinkEffect_UpdateProjectileBurst(p);
            SceneBurstEmitter_TransitionIfComplete(p, 3);
            break;
        case 3:
            LinkEffect_UpdateSecondOrb(p);
            SceneBurstEmitter_TransitionIfComplete(p, 0);
            break;
    }
    p->frame++;
    return 0;
}
