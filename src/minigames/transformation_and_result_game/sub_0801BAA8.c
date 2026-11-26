#include "minigames/result_game_tutorial/result_tutorial.h"
#include "engine/shared/contact_actor.h"
#include "engine/shared/player.h"
int ResultMenu_IsPhaseStart(void *);
void sub_0801B700(void *);
void PlayerCue_Stand(void *, int);
void Vec16_Lerp(void *, void *, void *, u32, int);
void OrbitingParticles_Create(void *, int, int, int, int);
void FloatingItemEffect_Spawn(int, int, int, int, void *, void *, int, void *);
void PlayerCue_Motion7Front(void *, int, int);
void ScreenTransition_SendCommand(int, int, int, int, int, int, int);
void sub_080148CC(int, int, void *, int, int, int, int, int, int, int, int);
void ResultMenu_SetPhase(void *, void (*)(void));
void Transformation_Finish(void);
void Transformation_Reveal(struct ResultMenu *p)
{
    struct Player *actor = gPlayers[0];
    struct SceneVec orbit, spawn;
    struct ContactPackedVector burst;
    int i, angle;
    if (ResultMenu_IsPhaseStart(p)) {
        sub_0801B700(p);
        p->revealStart = p->cameraPosition;
        p->revealEnd = actor->position;
        p->revealPose = 4;
        PlayerCue_Stand(actor, 4);
    }
    if (p->phaseFrame <= 32)
        Vec16_Lerp(&p->cameraPosition, &p->revealStart, &p->revealEnd, p->phaseFrame, 5);
    if (p->phaseFrame == 30) {
        orbit = actor->position;
        OrbitingParticles_Create(&orbit, 64, 60, 8, 0);
        Sound_PlaySong(0x29c);
    }
    if (p->phaseFrame == 90) {
        {
            struct SceneVec *out = &spawn;
            struct SceneVec *origin = &actor->position;
            spawn = actor->position;
            out->x -= 32;
            out->z += 32;
            out->y += 128;
            FloatingItemEffect_Spawn(1, p->transformationResult.kind, 0, 0, out, origin, 60, p);
        }
        PlayerCue_Motion7Front(actor, 0, 0);
        Sound_PlaySong(0x110);
    }
    if (p->phaseFrame == 96 && actor) {
        burst.x = actor->position.x;
        burst.y = actor->position.y + 378;
        burst.z = actor->position.z;
        ScreenTransition_SendCommand(0, 3, 31, 31, 31, 0xfff, 64);
        {
            struct ContactPackedVector *origin = &burst;
            enum { REVEAL_BURST_SPACING = 88 };

            for (i = 0; i < 3; i++) {
                angle = i * REVEAL_BURST_SPACING;
                sub_080148CC(4, 0x218, origin, angle, REVEAL_BURST_SPACING, 8, 8, 8, 1, 12, 8);
            }
        }
    }
    if (p->phaseFrame > 163) {
        ResultMenu_SetPhase(p, Transformation_Finish);
        PlayerCue_Stand(actor, p->revealPose);
    } else
        (p->phaseFrame)++;
}
