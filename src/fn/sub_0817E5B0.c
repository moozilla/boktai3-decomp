#include "global.h"
#define SceneEventActor_UpdateRemovalSequence sub_0817E5B0
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
    u8 unknownB3[0x1F9];
    u8 animation[0x20];
    u8 drawable[0x1D];
    u8 eventFlags;
    u8 unknown2EA[2];
    u8 scripts[1];
};
void SceneActors_RunFirstEventScript(void *, void *);
void SceneActors_RunSecondEventScript(void *, void *);
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
void SceneSteeringActor_RequestRemovalAll(void);
void Sound_PlaySong(u32);
s32 Player_IsSlotInActionTwelveOrThirtyEight(s32);
void sub_08220F00(void *, u32);
void sub_0817E5B0(struct SceneEventActor *actor)
{
    if (actor->restart) {
        actor->restart = 0;
        actor->phase = 0;
        actor->timer = 0;
        SceneActors_StartEightDirectionAnimation(actor, actor->drawable, actor->animation, 0x114, 2, 1, 4);
        Sound_PlaySong(0x2EB);
        SceneSteeringActor_RequestRemovalAll();
    }
    switch (actor->phase) {
    case 0:
        if (++actor->timer > 15 && !Player_IsSlotInActionTwelveOrThirtyEight(0)) {
            SceneActors_RunFirstEventScript(actor, actor->scripts);
            actor->phase = 1;
            actor->timer = 0;
        }
        break;
    case 1:
        if (!actor->timer) {
            SceneActors_StartEightDirectionAnimation(actor, actor->drawable, actor->animation, 0x148, 2, 1, 4);
            sub_08220F00(actor->drawable, 0x100);
            actor->timer++;
        }
        if (actor->animationComplete) {
            { u32 phase = 2; u32 zero = 0; actor->phase = phase; actor->timer = zero; }
        }
        break;
    case 2:
        if (actor->eventFlags & 2) {
            { u32 phase = 3; u32 zero = 0; actor->phase = phase; actor->timer = zero; }
            SceneActors_RunSecondEventScript(actor, actor->scripts);
        }
        break;
    }
    { u32 mask = 0xD; actor->flags |= mask; }
}
