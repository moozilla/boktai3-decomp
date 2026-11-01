#include "scene_steering_actor.h"
#include "player_cue_sequence.h"
void MotionEntry_UpdatePosition(void *);
void Hresvelgr_ReleaseEffect(void *);
void Hresvelgr_UpdateMotionEntry(struct SceneSteeringActor *p)
{
    u16 *counter;
    MotionEntry_UpdatePosition(p);
    if (((u8 *)p)[0x58] != 3)
        PlayerCue_StepSequence((struct ContactSwarmSprite *)p,
                               (struct DisplaySequenceState *)&p->sequence);
    counter = (u16 *)((u8 *)p + 0x50);
    if (--*counter == 0)
        Hresvelgr_ReleaseEffect(p);
}
