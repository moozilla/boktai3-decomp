#include "global.h"
#include "engine.h"
#include "ram.h"
#define gMotionActorSlots gUnk_020004F0
struct MotionActorAnimation {
    u8 unknown0[0x60];
};
struct MotionActorState {
    u8 unknown0[0x56C];
    u32 resource;
    u8 unknown570[0x88];
    struct MotionActorAnimation animations[5];
};
struct MotionActor {
    u8 unknown0[0x18];
    struct MotionActorState state;
    u8 unknown7F0[0x80];
    u32 display;
    u8 unknown874[0x84];
    s8 slot;
};
extern struct MotionActor *gUnk_020004F0[];
void Anim_Remove(void *);
void Sound_StopSong(u32);
void SceneMessages_UnregisterReceiver(void *);
void SpatialIndex_RemoveEntry(void *);
s32 MotionActor_Destroy(struct MotionActor *actor)
{
    struct MotionActorState *state = &actor->state;
    struct MotionActorAnimation *anim = actor->state.animations;
    s32 i = 4;
    do {
        Anim_Remove(anim);
        anim++;
        i--;
    } while (i >= 0);
    SpatialIndex_RemoveEntry(&state->resource);
    {
        u32 mask = 0x800;
        if (!((gEngineFlags | gScenePauseFlags) & mask))
            SceneMessages_UnregisterReceiver(&actor->display);
    }
    Sound_StopSong(0x87);
    Sound_StopSong(0x88);
    Sound_StopSong(0x89);
    Sound_StopSong(0x8A);
    Sound_StopSong(0x8B);
    Sound_StopSong(0x93);
    gUnk_020004F0[actor->slot] = 0;
    return 0;
}
