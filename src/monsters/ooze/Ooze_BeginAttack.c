#include "monsters/ooze.h"

#include "engine/video/display_animation.h"
#include "player/cutscene/player_cue_sequence.h"

void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
void Ooze_UpdateAttackDelay(void);
s32 Ooze_BeginAttack(struct Ooze *ooze)
{
    switch (ooze->phase) {
        case 0:
            PlayerCue_StepSequenceEnded(&ooze->sprite, &ooze->sequence);
            if (ooze->attackTimer > ooze->attackDelay) {
                if (ooze->repeatAttack)
                    Anim_RestartSequence(&ooze->sequence, ooze->resource, ooze->startSequence,
                                         (u8)ooze->facing, 0);
                else
                    Anim_RestartSequence(&ooze->sequence, ooze->resource, ooze->holdSequence,
                                         (u8)ooze->facing, 8);
                ooze->phase = 1;
                Sound_PlaySong(0x523);
            }
            break;

        case 1: {
            struct ContactCollider *collider = &ooze->collider;
            if (PlayerCue_StepSequenceEnded(&ooze->sprite, &ooze->sequence)) {
                if (ooze->repeatAttack) {
                    if (++ooze->repeatCount >= ooze->repeatLimit) {
                        Anim_RestartSequence(&ooze->sequence, ooze->resource, ooze->endSequence,
                                             (u8)ooze->facing, 0);
                        ooze->phase = 2;
                    }
                } else {
                    Anim_RestartSequence(&ooze->sequence, ooze->resource, ooze->endSequence,
                                         (u8)ooze->facing, 0);
                    ooze->phase = 2;
                }
            }
            Collider_SetPosition(collider, &ooze->sprite.position, 0);
            Collider_Refresh(collider);
        } break;

        case 2:
            if (PlayerCue_StepSequenceEnded(&ooze->sprite, &ooze->sequence)) {
                Anim_RestartSequence(&ooze->sequence, ooze->resource, ooze->idleSequence,
                                     (u8)ooze->facing, 8);
                ooze->state = Ooze_UpdateAttackDelay;
                ooze->phase = 0;
                ooze->attackTimer = 0;
                ooze->repeatCount = 0;
            }
            break;
    }

    ooze->sprite.flags &= ~1;
    ooze->attackTimer++;
    return 0;
}
