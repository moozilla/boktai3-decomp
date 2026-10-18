#define ActorRecoil_UpdateMotion     sub_081167CC
#define ActorRecoil_UpdateStateSound Character_UpdateLoopingSound
// Tentative family and semantic names; layouts follow observed field offsets.
#include "global.h"
struct RecoilBody {
    u8 unknown0[0xac];
    u8 flagAC;
    u8 unknownAD[3];
    void *callback;
    u8 unknownB4[0x524 - 0xb4];
    u16 stateSound;
    u16 previousSound;
    u8 unknown528[0x84];
    s32 impulse;
    s32 unknown5B0;
    s32 velocity;
};
struct RecoilMotionActor {
    u8 unknown[0x18];
    struct RecoilBody body;
    u8 unknown5D0[0x8c8 - 0x5d0];
    u8 collisionInfo[1];
};
s32 sub_0811DB84(struct RecoilBody *, void *);
s32 sub_0824923C(void *, void *);
void sub_0811CF68(struct RecoilBody *);
s32 Div(s32, s32);
u8 sub_08116C74(struct RecoilMotionActor *);
void Character_UpdateLoopingSound(struct RecoilBody *);
s32 sub_081167CC(struct RecoilMotionActor *p)
{
    struct RecoilBody *body = &p->body;
    s32 result = sub_0811DB84(body, p->collisionInfo);
    s32 changed = 0;
    if (sub_0824923C(body, body->callback)) {
        if (body->stateSound == 0x89)
            body->stateSound = 0x8b;
        else
            body->stateSound = 0x8a;
        changed = 1;
    }
    sub_0811CF68(body);
    if (!body->flagAC && !result) {
        body->impulse -= 0x20;
        if (!changed)
            body->stateSound = 0x87;
    }
    body->velocity = Div(body->velocity * 9, 10);
    if (sub_08116C74(p))
        Character_UpdateLoopingSound(body);
    return result;
}
