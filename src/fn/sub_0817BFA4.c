#include "global.h"
#define SceneActorState_UpdateAnimationRequest sub_0817BFA4
struct SceneActorAnimationRequest {
    u8 pad0[0xaa];
    u8 state;
    u8 padAB[3];
    u8 animation;
    u8 phase;
    u8 pending;
    u8 padB1[0x13];
    u32 timer;
    u8 padC8[0x420];
    u8 request;
    u8 requestedAnimation;
    u8 pad4EA;
    u8 requestPhase;
};
static inline u8 consume(struct SceneActorAnimationRequest *p)
{
    if (p->pending) {
        p->pending = 0;
        p->phase = 0;
        return 1;
    }
    return 0;
}
void sub_0817BFA4(struct SceneActorAnimationRequest *p)
{
    if (consume(p)) {
        u32 state = 7;
        p->state = state;
    }
    if (p->phase == 1) {
        p->state = 1;
        p->requestedAnimation = p->animation;
        p->request = 1;
        p->requestPhase = 0;
    }
    p->timer++;
}
