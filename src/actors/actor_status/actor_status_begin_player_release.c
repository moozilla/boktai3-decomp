#include "actors/paired_scene_actor/paired_scene_actor.h"
#include "engine/shared/global.h"
#include "engine/shared/scene_actor_reset.h"
#include "engine/shared/scene_actor_control_motion.h"
#include "actors/scene_actor_animation_and_scripted_actions/scene_actor_animation.h"
#include "player/player.h"
#include "player/player_cue/player_cue_sequence.h"
// confidence=low: positions player before setting release phase
void Player_ApplyPositionRequest(void *, void *, u32, u32, u32, u32, u32, u32);
void ActorStatus_BeginPlayerRelease(struct VanargandAttackPair *owner, struct PairedSceneHalf *s)
{
    u32 mask = 12;
    s->actionFlags |= mask;
    if (s->entered) {
        s->entered = 0;
        PairedSceneHalf_BeginActionPhase(s, 0);
    }
    switch (s->actionCounters[0]) {
        case 0:
            Player_ApplyPositionRequest(gPlayers[0], &s->sprite.position, 80, 32, 128, 0, 0, 0);
            {
                void *data = (void *)owner->effectResource;
                Anim_SetSequence(&s->graphics.sequence, data, 9, (u8)s->mode, 0);
            }
            PlayerCue_StepSequence(&s->sprite, &s->graphics.sequence);
            {
                u32 next = 1;
                s->actionCounters[0] = next;
                s->actionCounters[1] = 0;
                s->actionEntered = next;
            }
            break;
        case 1:
            s->finished = 1;
            break;
    }
}
