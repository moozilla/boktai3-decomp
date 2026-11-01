#include "global.h"
#include "display_animation.h"
#include "display_sprite_playback.h"
#include "tile_trigger.h"
#include "engine.h"
#include "ram.h"
struct TileTriggerManager {
    u8 unknown0[0x18];
    void *animationData;
};
void TileTrigger_ApplyContactEffects(struct TileTrigger *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void TileTrigger_EmitParticles(struct TileTrigger *);
// confidence=low: restarts the trigger animation at frame 1, steps it once and enters state 6.
void sub_08107D54(struct TileTriggerManager *manager, struct TileTrigger *p)
{
    p->sprite.flags &= ~1;
    Anim_SetSequence(&p->animation, manager->animationData, 0, (u8)p->sequence, 0);
    p->animation.index = 1;
    PlayerCue_StepSequenceEnded(&p->sprite, &p->animation);
    p->state = 6;
    p->triggerFlags = (p->triggerFlags & ~0x20) | 2;
}
