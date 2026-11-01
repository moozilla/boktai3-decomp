#include "global.h"

#include "display_animation.h"
#include "player_cue_sequence.h"

struct Ooze {
    u8 pad0[0x20];
    struct DisplaySequenceSprite sprite;
    u8 pad4c[0x104 - 0x4c];
    void (*state)(void);
    struct DisplaySequenceState sequence;
    s16 startDelay, unknown11a, unknown11c, timer;
    u8 pad120[0x18];
    s16 repeatAttack, repeatLimit, repeatCount, attackDelay, phase, unknown142;
    u8 facing, unknown145;
    u16 idleSequence, startSequence, holdSequence, endSequence;
    u8 pad14e[0xA];
    void *resource;
};
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
void Ooze_UpdateAttackDelay(void);
s32 Ooze_BeginAttack(struct Ooze *p)
{
    s32 phase = p->phase;
    switch (phase) {
        case 0:
            PlayerCue_StepSequenceEndedWithFirstFlipMask((struct ContactSwarmSprite *)&p->sprite,
                                                         &p->sequence, 1);
            if (p->timer > p->attackDelay) {
                if (p->repeatAttack)
                    Anim_RestartSequence(&p->sequence, p->resource, p->startSequence, p->facing, 0);
                else
                    Anim_RestartSequence(&p->sequence, p->resource, p->holdSequence, p->facing, 8);
                p->phase = 1;
                Sound_PlaySong(0x523);
            }
            break;

        case 1: {
            void *collider = ((u8 *)p) + 0x68;
            if (PlayerCue_StepSequenceEndedWithFirstFlipMask(
                    (struct ContactSwarmSprite *)&p->sprite, &p->sequence, phase)) {
                if (p->repeatAttack) {
                    if ((++p->repeatCount) >= p->repeatLimit) {
                        Anim_RestartSequence(&p->sequence, p->resource, p->endSequence, p->facing,
                                             0);
                        p->phase = 2;
                    }
                } else {
                    Anim_RestartSequence(&p->sequence, p->resource, p->endSequence, p->facing, 0);
                    p->phase = 2;
                }
            }
            Collider_SetPosition(collider, ((u8 *)p) + 0x3c, 0);
            Collider_Refresh(collider);
        } break;

        case 2:
            if (PlayerCue_StepSequenceEndedWithFirstFlipMask(
                    (struct ContactSwarmSprite *)&p->sprite, &p->sequence, 1)) {
                Anim_RestartSequence(&p->sequence, p->resource, p->idleSequence, p->facing, 8);
                p->state = Ooze_UpdateAttackDelay;
                p->phase = 0;
                p->timer = 0;
                p->repeatCount = 0;
            }
            break;
    }

    p->sprite.flags &= ~1;
    p->timer++;
    return 0;
}
