#include "global.h"

struct DirectionalActor {
    u8 pad0[0x9C];
    u32 flags;
    u8 padA0[0x11];
    u8 restart;
    u8 padB2;
    u8 special;
    u8 padB4[0x50];
    void *callback;
    u8 pad108[0x1A4];
    u8 animationState[0x20];
    u8 animation;
};
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);

void ActorAnimation_UpdateDirectional(struct DirectionalActor *p)
{
    if (p->restart) {
        p->restart = 0;
        SceneActors_StartEightDirectionAnimation(p, &p->animation, p->animationState, 0xF4, 1, 1,
                                                 4);
        {
            u32 flags = 0x400004;
            p->flags |= flags;
        }
    } else {
        SceneActors_StartEightDirectionAnimation(p, &p->animation, p->animationState, 0xF4, 1, 1,
                                                 1);
    }
    if ((u8)Callback_InvokeActor(p, p->callback) == 0) {
        if (p->special) {
            u32 flags = 0x400004;
            p->flags |= flags;
        } else {
            u32 flags = 0x40;
            p->flags |= flags;
        }
    }
}
