#ifndef RESULT_TUTORIAL_H
#define RESULT_TUTORIAL_H

#include "engine.h"
#include "scene_input_widget.h"

/* The result game and its tutorial share the same task and display objects.
 * Position and velocity use five fractional bits. Identities are tentative. */
struct ResultTutorialMarker {
    u32 flags;
    u8 unknown04[0x14];
    s16 x, y, z;
    u8 unknown1E[0xA];
};

struct ResultMenu {
    struct Task task;
    u8 unknown18[0x2E];
    u16 lastPressScore;
    u8 unknown48[8];
    s32 level;
    u8 unknown54[0x20];
    void *messageTable, *owner;
    u8 unknown7C[0x10];
    s32 playerPosition, playerVelocity;
    s32 targetMinimum, targetMaximum, targetPosition;
    u8 unknownA0[8];
    s16 elapsed, timer;
    u16 countdown;
    u8 difficulty, phase;
    u8 pressState, pressed, accelerationDelay, unknownB3;
    u8 step, alpha, prompt, layer;
    u8 hideLevel, hideTimer;
    u8 unknownBA[6];
    u8 pressScore;
    u8 unknownC1[0x43];
    struct SceneInputResource primaryResource, secondaryResource;
    struct SceneInputWidget widgets[15];
    struct ResultTutorialMarker markers[4];
    u8 unknown784[0x1C];
    u16 completionScript;
    u8 unknown7A2[2];
    void *update;
    u8 unknown7A8[0x4C];
};

struct ResultTutorialStep {
    s16 prompt;
    u8 wait, hide;
    int (*run)(void *);
};

int ResultTutorial_FadeIn(struct ResultMenu *tutorial);
int ResultTutorial_FadeOut(struct ResultMenu *tutorial);
int ResultTutorial_ShowPrompt(struct ResultMenu *tutorial);
int ResultTutorial_HidePrompt(struct ResultMenu *tutorial);
int ResultTutorial_SlideMarkerIn(struct ResultMenu *tutorial);
int ResultTutorial_BlinkPlayer(struct ResultMenu *tutorial);
int ResultTutorial_BlinkTarget(struct ResultMenu *tutorial);
int ResultTutorial_MovePlayer(struct ResultMenu *tutorial, int input);
u32 ResultTutorial_DemonstrateMovement(struct ResultMenu *tutorial);
int ResultTutorial_RaiseMarker(struct ResultMenu *tutorial);
int ResultTutorial_LowerMarker(struct ResultMenu *tutorial);
int ResultTutorial_DemonstrateTracking(struct ResultMenu *tutorial);
int ResultTutorial_SlideMarkerOut(struct ResultMenu *tutorial);
int ResultTutorial_InitializePressDemo(struct ResultMenu *tutorial);
int ResultTutorial_BlinkPressMarker(struct ResultMenu *tutorial);
void ResultTutorial_Press(struct ResultMenu *tutorial, int pressed);
int ResultTutorial_DemonstrateEarlyPress(struct ResultMenu *tutorial);
int ResultTutorial_DemonstrateLatePress(struct ResultMenu *tutorial);
int ResultTutorial_DemonstrateAccuratePress(struct ResultMenu *tutorial);
int ResultTutorial_BlinkScore(struct ResultMenu *tutorial);
int ResultTutorial_BlinkTimer(struct ResultMenu *tutorial);
int ResultTutorial_BlinkLevel(struct ResultMenu *tutorial);
int ResultTutorial_AnimateLevel(struct ResultMenu *tutorial);
int ResultTutorial_Close(struct ResultMenu *tutorial);
void ResultTutorial_Open(struct ResultMenu *tutorial);
void ResultTutorial_UpdateSequence(struct ResultMenu *tutorial);
u32 ResultTutorial_Finish(struct ResultMenu *tutorial);
int ResultTutorial_ReadScript(struct ResultMenu *tutorial);
int ResultTutorial_Initialize(struct ResultMenu *tutorial, void *owner, void *unused);
void *ResultTutorial_CreateTask(void *a, void *b);

#endif
