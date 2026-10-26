#include "global.h"
#include "game_state.h"
struct SceneEventActor {
    u8 unknown0[0x9C];
    u32 flags;
    u8 unknownA0[2];
    u8 phase;
    u8 unknownA3;
    u32 timer;
    u8 unknownA8[9];
    u8 restart;
    u8 animationComplete;
    u8 unknownB3[0x51];
    void *update;
    u8 unknown108[0x1A4];
    u8 animation[0x20];
    u8 drawable[0x1D];
    u8 eventFlags;
    u8 unknown2EA[2];
    u8 scripts[1];
};
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);
void ProximityVolume_SetPosition(void *, void *);
void SceneEventActor_UpdateDelayedFinish(struct SceneEventActor *actor)
{
    if (actor->restart)
        actor->restart = 0;
    switch (actor->phase) {
        case 0:
            if (!actor->timer) {
                SceneActors_StartEightDirectionAnimation(actor, actor->drawable, actor->animation,
                                                         0x148, 2, 1, 1);
                actor->timer++;
            }
            if (actor->animationComplete) {
                ProximityVolume_SetPosition((u8 *)actor + 0x428, (u8 *)actor + 0x54);
                actor->phase = 1;
                actor->timer = 0;
            }
            break;
        case 1:
            if (!actor->timer)
                SceneActors_StartEightDirectionAnimation(actor, actor->drawable, actor->animation,
                                                         0x14D, 1, 1, 1);
            {
                u32 mask = 0x200000;
                actor->flags |= mask;
            }
            actor->timer++;
            {
                u32 mask = 0x50000;
                actor->flags |= mask;
            }
            if (actor->timer > 120) {
                ProximityVolume_SetPosition((u8 *)actor + 0x428, (u8 *)actor + 0x54);
                {
                    u32 phase = 2;
                    u32 zero = 0;
                    actor->phase = phase;
                    actor->timer = zero;
                }
            }
            break;
        case 2: {
            if (gGameState->unk876 <= 0) {
                u32 mask = 0x10010;
                actor->flags |= mask;
            } else {
                u32 mask = 0x50000;
                actor->flags |= mask;
            }
            *((u8 *)actor + 0xAF) = 1;
        } break;
    }
    if (!(u8)Callback_InvokeActor(actor, actor->update)) {
        u32 mask = 0x400000;
        actor->flags |= mask;
    }
}
