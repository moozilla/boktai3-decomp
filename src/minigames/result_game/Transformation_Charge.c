#include "minigames/result_game/result_tutorial.h"
#include "player/player.h"
#include "scene/collision/contact_position.h"
int ResultMenu_IsPhaseStart(void *);
void ResultMenu_SendCommand(void *, int, int, void *);
void PlayerCue_BeginModeSeven(void *, int);
void ImpactBurst_EmitChargeSparkle(int, int, int, int, void *);
void ResultMenu_SetPhase(void *, void (*)(void));
void Transformation_Reveal(void);
void Transformation_Charge(struct ResultMenu *p)
{
    struct Player *actor = gPlayers[0];
    struct Player *positionActor = actor;
    u16 param;
    struct SceneVec pos;
    if (ResultMenu_IsPhaseStart(p)) {
        param = 2;
        ResultMenu_SendCommand(p, 1, 1, &param);
        PlayerCue_BeginModeSeven(actor, 3);
        Sound_PlaySong(0x2e1);
    }
    if (p->phaseFrame - 30 <= 69 && !(p->phaseFrame & 3)) {
        {
            struct SceneVec *out = &pos;
            pos = positionActor->position;
            out->x += 200;
            ImpactBurst_EmitChargeSparkle(2, 0, 0x2000, 64, out);
        }
        Sound_PlaySong(0x1c5);
    }
    if (p->phaseFrame > 99)
        ResultMenu_SetPhase(p, Transformation_Reveal);
    else
        p->phaseFrame++;
}
