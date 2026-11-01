#include "global.h"
#include "display_animation.h"
#include "display_sprite_playback.h"
#include "tile_trigger.h"
#include "ram.h"
struct TileTriggerManager {
    u8 unknown0[0x18];
    void *animationData;
};
void TileTrigger_ApplyContactEffects(struct TileTrigger *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
// confidence=low: runs the trigger animation once the countdown passes 30 and advances the state when it ends.
void TileTrigger_StepActivation(struct TileTrigger *p)
{
    struct TileTrigger *self = p;
    struct TileTriggerManager *manager = gTileTriggerManager;
    if (p->timer > 30) {
        u8 *forced = &p->forced;
        if (*forced) {
            Anim_RestartSequence(&p->animation, manager->animationData, 0, (u8)p->sequence, 4);
            *forced = 0;
            switch (p->sequence) {
                case 0:
                case 1:
                    Sound_PlaySong(0x1AC);
                    break;
            }
        }
        if (!PlayerCue_StepSequenceEnded(&self->sprite, &p->animation))
            goto tick;
        self->sprite.flags |= 1;
        p->state = 1;
        p->timer = 0;
        return;
    } else if (p->animation.index == 1) {
        TileTrigger_ApplyContactEffects(p);
    }
tick:
    p->timer++;
}

void TileTrigger_StepRelease(struct TileTrigger *p)
{
    struct TileTrigger *self;
    struct TileTriggerManager *manager;
    self = p;
    manager = gTileTriggerManager;
    p->sprite.flags &= ~1;
    if (p->triggerFlags & 0x40) {
        u8 *forced = &p->forced;
        if (*forced) {
            Anim_RestartSequence(&p->animation, manager->animationData, 0, (u8)p->sequence, 4);
            *forced = 0;
            switch (p->sequence) {
                case 0:
                case 1:
                    Sound_PlaySong(0x1AC);
                    break;
            }
        }
        if (!PlayerCue_StepSequenceEnded(&self->sprite, &p->animation))
            goto tick;
        self->sprite.flags |= 1;
        p->state = 3;
        p->timer = 0;
        p->triggerFlags &= ~0x42;
        return;
    } else {
        TileTrigger_ApplyContactEffects(p);
    }
tick:
    p->timer++;
}
