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
// confidence=low: plays the hold sequence while the 0x20 trigger flag is set; moves to state 6 when it ends.
void TileTrigger_StepHold(struct TileTrigger *p)
{
    struct TileTrigger *self;
    struct TileTriggerManager *manager;
    manager = gTileTriggerManager;
    self = p;
    if (p->triggerFlags & 0x20) {
        p->sprite.flags &= ~1;
        {
            u8 *forced = &p->forced;
            if (*forced) {
                TileTrigger_EmitParticles(p);
                Anim_SetSequence(&p->animation, manager->animationData, 0, (u8)p->sequence, 0);
                *forced = 0;
                switch (p->sequence) {
                    case 0:
                    case 1:
                        if (gEngineFlags & 0x800)
                            Sound_PlaySong(0x26D);
                        else
                            Sound_PlaySong(0x1AB);
                        break;
                }
            }
        }
        if ((p->animation.index == 0 && p->animation.elapsed > 2) ||
            (p->animation.index == 1 && p->animation.elapsed != 0))
            TileTrigger_ApplyContactEffects(p);
        if (PlayerCue_StepSequenceEnded(&self->sprite, &p->animation)) {
            p->state = 6;
            p->triggerFlags = (p->triggerFlags & ~0x20) | 2;
        }
    }
}
