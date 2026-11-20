#ifndef RESULT_TUTORIAL_H
#define RESULT_TUTORIAL_H

#include "engine.h"
#include "scene_input_widget.h"
#include "scene_value_equipment.h"
#include "scene_vec.h"
#include "sprite_oam.h"

/* The result game and its tutorial share the same task and display objects.
 * Position and velocity use five fractional bits. Identities are tentative. */
struct ResultMarkerFeedback {
    u16 phase, elapsed;
};

struct ResultMenu {
    struct Task task;
    struct SceneVec cameraPosition, initialCameraPosition;
    struct SceneVec revealStart, revealEnd;
    u8 phaseStart;
    u8 unknown39[3];
    u32 phaseFrame;
    s16 targetLeft, targetRight, targetCenter;
    u16 lastPressScore;
    u8 markerAnimationState, unknown49, nextFeedbackSlot;
    u8 unknown4B[5];
    s32 level;
    u8 unknown54[0x20];
    void *messageTable, *owner;
    struct SceneValueEquipment transformationResult;
    s32 playerPosition, playerVelocity;
    s32 targetMinimum, targetMaximum, targetPosition;
    u16 trackingScore, trackingSamples, pressScoreTotal, pressSamples;
    s16 elapsed, timer;
    u16 countdown;
    u8 difficulty, phase;
    u8 pressState, pressed, accelerationDelay;
    s8 gaugeBrightness;
    u8 step, alpha, prompt, layer;
    u8 hideLevel, hideTimer;
    u8 equipmentSlot, consumableSlot, wideTimingWindow;
    u8 unknownBD[2], lastTimingGrade;
    u8 pressScore;
    u8 revealPose, unknownC2[2], finishState;
    u8 unknownC5[0x13];
    s32 preciseCount, closeCount, missedCount;
    u16 meterPalette[16];
    struct SceneInputResource primaryResource, secondaryResource;
    struct SceneInputWidget widgets[15];
    struct SpriteDrawable markers[4];
    u8 callbackPending;
    u8 unknown785[3];
    u32 unknown788;
    struct ResultMarkerFeedback markerFeedback[5];
    u16 completionScript;
    u8 unknown7A2[2];
    void *update;
    void (*gameUpdate)(struct ResultMenu *);
    u8 unknown7AC[0x48];
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
