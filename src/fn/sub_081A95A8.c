#include "paired_scene_actor.h"
#include "player.h"

struct Vanargand {
    u8 pad0[0x9C];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 phaseTimer;
    u8 padA8[7];
    u8 finished;
    u8 entered, pendingAnimation;
    u8 padB2[0x104 - 0xB2];
    u8 (*callback)(struct Vanargand *);
    u8 pad108[0x56C - 0x108];
    struct VanargandAttackPair *attachment;
    u8 pad570[8];
    u32 resetTimer;
};
s32 Player_RequestTimedReset(struct Player *, u32);
void Sound_PlaySong(u32);
void Vanargand_SetPendingCue(void *, u16);
void Vanargand_UpdatePairedAttack(struct Vanargand *p)
{
    s32 callbackResult;
    s32 phase;
    u32 *timer;

    u32 flags = 0x40;
    p->flags |= flags;
    Vanargand_SetPendingCue(p, 1);
    callbackResult = p->callback(p);
    if ((s16)callbackResult > 0)
        return;
    if (p->pendingAnimation) {
        p->pendingAnimation = callbackResult;
        p->phase = callbackResult;
        p->phaseTimer = callbackResult;
    }
    phase = p->phase;
    switch (phase) {
        case 0:
            timer = &p->phaseTimer;
            if (*timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(p->attachment, 3, 7);
                (*timer)++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(p->attachment)) {
                p->phase = 1;
                *timer = phase;
            }
            break;
        case 1:
            timer = &p->phaseTimer;
            if (*timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(p->attachment, 3, 16);
                (*timer)++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(p->attachment)) {
                s32 count = ScenePair_CountPositiveCounters(p->attachment);
                if (count > 0) {
                    struct Player *player = gPlayers[0];
                    u32 duration = count * 72;
                    Player_RequestTimedReset(player, duration);
                    p->resetTimer = duration;
                }
                Sound_PlaySong(0x1BA);
                p->phase = 2;
                *timer = callbackResult;
            }
            break;
        case 2:
            p->phaseTimer++;
            if (p->phaseTimer > 60) {
                p->phase = 3;
                p->phaseTimer = callbackResult;
            }
            break;
        case 3:
            p->finished = 1;
            break;
    }
}
