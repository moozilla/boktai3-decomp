#include "minigames/result_game/result_tutorial.h"
#include "scene/collision/contact_actor.h"
#include "player/player.h"
int ResultMenu_IsPhaseStart(void *);
void sub_0801B700(void *);
void PlayerCue_Stand(void *, int);
void Vec16_Lerp(void *, void *, void *, u32, int);
void OrbitingParticles_Create(void *, int, int, int, int);
void FloatingItemEffect_Spawn(int, int, int, int, void *, void *, int, void *);
void PlayerCue_Motion7Front(void *, int, int);
void ScreenTransition_SendCommand(int, int, int, int, int, int, int);
void ImpactParticleGroup_SpawnRadial(int, int, void *, int, int, int, int, int, int, int, int);
void ResultMenu_SetPhase(void *, void (*)(void));
void Transformation_Finish(void);
enum { REVEAL_BURST_SPACING = 88 };

void Transformation_Reveal(struct ResultMenu *menu)
{
    struct Player *actor = gPlayers[0];
    struct SceneVec orbit, spawn;
    struct ContactPackedVector burst;
    int i;
    if (ResultMenu_IsPhaseStart(menu)) {
        sub_0801B700(menu);
        menu->revealStart = menu->cameraPosition;
        menu->revealEnd = actor->position;
        menu->revealPose = 4;
        PlayerCue_Stand(actor, 4);
    }
    if (menu->phaseFrame <= 32)
        Vec16_Lerp(&menu->cameraPosition, &menu->revealStart, &menu->revealEnd, menu->phaseFrame,
                   5);
    if (menu->phaseFrame == 30) {
        orbit = actor->position;
        OrbitingParticles_Create(&orbit, 64, 60, 8, 0);
        Sound_PlaySong(0x29c);
    }
    if (menu->phaseFrame == 90) {
        struct SceneVec *out = &spawn;
        struct SceneVec *origin = &actor->position;
        spawn = actor->position;
        out->x -= 32;
        out->z += 32;
        out->y += 128;
        FloatingItemEffect_Spawn(1, menu->transformationResult.kind, 0, 0, out, origin, 60, menu);
        PlayerCue_Motion7Front(actor, 0, 0);
        Sound_PlaySong(0x110);
    }
    if (menu->phaseFrame == 96 && actor) {
        burst.x = actor->position.x;
        burst.y = actor->position.y + 378;
        burst.z = actor->position.z;
        ScreenTransition_SendCommand(0, 3, 31, 31, 31, 0xfff, 64);
        for (i = 0; i < 3; i++)
            ImpactParticleGroup_SpawnRadial(4, 0x218, &burst, i * REVEAL_BURST_SPACING,
                                            REVEAL_BURST_SPACING, 8, 8, 8, 1, 12, 8);
    }
    if (menu->phaseFrame > 163) {
        ResultMenu_SetPhase(menu, Transformation_Finish);
        PlayerCue_Stand(actor, menu->revealPose);
    } else
        menu->phaseFrame++;
}
