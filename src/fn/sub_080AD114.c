#define SceneActorContext_RunPendingScript Vorn_RunPendingScript
#include "global.h"
#include "scene_actor_reaction.h"
#include "player.h"
struct SceneActorFacingTarget {
    u8 unknown0[0x30];
    s16 x;
    u8 unknown32[2];
    s16 z;
};
s32 Math_Atan2(s32, s32);
void Sound_PlaySong(u32);
void Vorn_BeginPursuitRoute(void *, u32);
void Vorn_RunPendingScript(void *);
void SceneActorReaction_UpdateFacingAttack(struct SceneActorRegistryResetView *actor, s32 frame)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        if ((*(struct SceneActorFacingTarget **)gPlayers))
            actor->body.direction =
                Math_Atan2((*(struct SceneActorFacingTarget **)gPlayers)->x - actor->body.position.x,
                           (*(struct SceneActorFacingTarget **)gPlayers)->z - actor->body.position.z);
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
        Vorn_RunPendingScript(actor->context);
    }
    if (frame > 60)
        Vorn_BeginPursuitRoute(actor, 1);
}
