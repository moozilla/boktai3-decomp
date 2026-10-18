#define SceneActorReaction_UpdateFacingAttack sub_080AD114
#define SceneActorContext_RunPendingScript    sub_080B309C
#define gSceneActorFacingTarget               gUnk_02000580
#include "global.h"
#include "scene_actor_reaction.h"
struct SceneActorFacingTarget {
    u8 unknown0[0x30];
    s16 x;
    u8 unknown32[2];
    s16 z;
};
extern struct SceneActorFacingTarget *gUnk_02000580;
s32 Math_Atan2(s32, s32);
void Sound_PlaySong(u32);
void sub_080B23A0(void *, u32);
void sub_080B309C(void *);
void sub_080AD114(struct SceneActorRegistryResetView *actor, s32 frame)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        if (gUnk_02000580)
            actor->direction = Math_Atan2(gUnk_02000580->x - actor->position.x,
                                          gUnk_02000580->z - actor->position.z);
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        actor->field2EA = 80;
        Sound_PlaySong(0x1CF);
    }
    {
        u32 mask = 4;
        actor->flags2D0 |= mask;
    }
    if (frame == 14) {
        actor->field2EA = 140;
        sub_080B309C(actor->context);
    }
    if (frame > 60)
        sub_080B23A0(actor, 1);
}
