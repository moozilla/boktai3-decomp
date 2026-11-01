#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "player.h"
#include "player_cue_sequence.h"
// confidence=low: positions player before setting release phase
struct Status {
    u8 pad[0x160];
    u32 flags, state, timer;
    u8 a, b, entered, phase, done;
};
void Player_ApplyPositionRequest(void *, void *, u32, u32, u32, u32, u32, u32);
void ActorStatus_BeginPlayerRelease(u8 *owner, struct Status *s)
{
    u32 mask = 12;
    s->flags |= mask;
    if (s->entered) {
        s->entered = 0;
        s->state = 0;
        s->timer = 0;
        s->phase = 1;
    }
    switch (s->state) {
        case 0:
            Player_ApplyPositionRequest((*(void **)gPlayers), (u8 *)s + 0x1c, 80, 32, 128, 0, 0, 0);
            {
                void *data = *(void **)(owner + 0x4d0);
                Anim_SetSequence((u8 *)s + 0x48, data, 9, *(u8 *)((u8 *)s + 0x15c), 0);
            }
            PlayerCue_StepSequence((struct ContactSwarmSprite *)s,
                                   (struct DisplaySequenceState *)((u8 *)s + 0x48));
            {
                u32 next = 1;
                s->state = next;
                s->timer = 0;
                s->phase = next;
            }
            break;
        case 1:
            s->done = 1;
            break;
    }
}
