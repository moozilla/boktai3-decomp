#include "bosses/vanargand/vanargand_pair_attack.h"
#include "bosses/vanargand/paired_scene_actor.h"
#include "player/player.h"

s32 Player_RequestTimedReset(struct Player *, u32);
void Sound_PlaySong(u32);
void Vanargand_SetPendingCue(void *, u16);
void Vanargand_UpdatePairedAttack(struct VanargandPairAttack *p)
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
    if (p->pending) {
        p->pending = callbackResult;
        p->phase = callbackResult;
        p->timer = callbackResult;
    }
    phase = p->phase;
    switch (phase) {
        case 0:
            timer = &p->timer;
            if (*timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 7);
                (*timer)++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(p->pair)) {
                p->phase = 1;
                *timer = phase;
            }
            break;
        case 1:
            timer = &p->timer;
            if (*timer == 0) {
                PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 16);
                (*timer)++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(p->pair)) {
                s32 count = ScenePair_CountPositiveCounters(p->pair);
                if (count > 0) {
                    struct Player *player = gPlayers[0];
                    u32 duration = count * 72;
                    Player_RequestTimedReset(player, duration);
                    p->impactShake = duration;
                }
                Sound_PlaySong(0x1BA);
                p->phase = 2;
                *timer = callbackResult;
            }
            break;
        case 2:
            p->timer++;
            if (p->timer > 60) {
                p->phase = 3;
                p->timer = callbackResult;
            }
            break;
        case 3:
            p->finished = 1;
            break;
    }
}
