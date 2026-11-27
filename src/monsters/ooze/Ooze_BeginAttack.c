#include "monsters/ooze.h"

#include "engine/video/display_animation.h"
#include "player/cutscene/player_cue_sequence.h"

void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
void Ooze_UpdateAttackDelay(void);
s32 Ooze_BeginAttack(struct Ooze *p)
{
    s32 phase = p->phase;
    switch (phase) {
        case 0:
            PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)&p->sprite, &p->sequence);
            if (p->attackTimer > p->attackDelay) {
                if (p->repeatAttack)
                    Anim_RestartSequence(&p->sequence, p->resource, p->startSequence, (u8)p->facing, 0);
                else
                    Anim_RestartSequence(&p->sequence, p->resource, p->holdSequence, (u8)p->facing, 8);
                p->phase = 1;
                Sound_PlaySong(0x523);
            }
            break;

        case 1: {
            struct ContactCollider *collider = &p->collider;
            if (PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)&p->sprite, &p->sequence)) {
                if (p->repeatAttack) {
                    if ((++p->repeatCount) >= p->repeatLimit) {
                        Anim_RestartSequence(&p->sequence, p->resource, p->endSequence, (u8)p->facing,
                                             0);
                        p->phase = 2;
                    }
                } else {
                    Anim_RestartSequence(&p->sequence, p->resource, p->endSequence, (u8)p->facing, 0);
                    p->phase = 2;
                }
            }
            Collider_SetPosition(collider, &p->sprite.position, 0);
            Collider_Refresh(collider);
        } break;

        case 2:
            if (PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)&p->sprite, &p->sequence)) {
                Anim_RestartSequence(&p->sequence, p->resource, p->idleSequence, (u8)p->facing, 8);
                p->state = Ooze_UpdateAttackDelay;
                p->phase = 0;
                p->attackTimer = 0;
                p->repeatCount = 0;
            }
            break;
    }

    p->sprite.flags &= ~1;
    p->attackTimer++;
    return 0;
}
