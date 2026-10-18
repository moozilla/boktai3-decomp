#define ActorMotion_UpdateDampedFall sub_08120330
#define ActorSound_UpdateSong Character_UpdateLoopingSound
#include "global.h"
struct DampedMotionActor {
    u8 unknown0[0xac];
    u8 grounded;
    u8 unknownAD[0x477];
    u16 song;
    u8 unknown526[0x86];
    s32 verticalVelocity;
    u8 unknown5B0[4];
    s32 horizontalVelocity;
    u8 unknown5B8[0x2d4];
    u8 action[0x12];
    u16 remaining;
};
s32 sub_0811DB84(struct DampedMotionActor *, void *);
void Character_UpdateLoopingSound(struct DampedMotionActor *);
s32 Div(s32, s32);
s32 sub_08120330(struct DampedMotionActor *actor)
{
    struct DampedMotionActor *motion = actor;
    s32 result = sub_0811DB84(actor, actor->action);
    if (!actor->grounded && !result) {
        actor->verticalVelocity -= 32;
        actor->song = 0x87;
    }
    motion->horizontalVelocity = Div(motion->horizontalVelocity * 9, 10);
    Character_UpdateLoopingSound(motion);
    if (!result && !actor->remaining)
        return 1;
    return 0;
}
