#ifndef GUARD_CONTROL_PANEL_H
#define GUARD_CONTROL_PANEL_H
#include "scene_input.h"
#include "contact_swarm_sprite.h"
#include "player.h"
#include "gba/io_reg.h"
#include "script_utility.h"

struct ControlPanelDisplay {
    struct ContactSwarmSprite sprite;
    u8 graphics[0x1C];
    u16 phase, phaseStep;
};
struct ControlPanel {
    struct Task task;
    u32 unknown18[2];
    struct ControlPanelDisplay display[2];
    struct ContactSwarmSprite thirdSprite;
    u8 thirdGraphics[0x1C];
    struct ContactSwarmSprite segments[7];
    u8 segmentGraphics[0x1C];
    struct ContactSwarmSprite markers[6];
    u8 markerGraphics[0x1C];
    u16 multiplierTimer, unknown376;
    struct SceneInputResource resource;
    struct SceneInputWidget widgets[5];
    u16 start, cooldown, soundTicks, selection;
    u8 unknown580[8];
    u16 sustainedInput, thresholds[3], multiplier, unknown592;
    s32 ticks, tickLimit, total, totalLimit;
    u32 multiplierScript, limitScript, endScript;
    u16 accumulator, duration, unknown5B4, timer;
    void (*update)(struct ControlPanel *);
};

void ControlPanel_ClearActivePanel(void);
void ControlPanel_SetUpdate(struct ControlPanel *panel, void (*v)(struct ControlPanel *));
void ControlPanel_ClearPlayerInputHistory(void);
void ControlPanel_ResetPlayerForInteractionPrompt(struct ControlPanel *panel);
void ControlPanel_UpdateAmbientIndicator(struct ControlPanel *panel);
void ControlPanel_UpdateGaugeIndicator(struct ControlPanel *gauge);
void ControlPanel_ShowRewardBalanceDigits(struct ControlPanel *panel);
void ControlPanel_ShowValueDisplay(struct ControlPanel *panel);
void ControlPanel_HideValueDisplay(struct ControlPanel *panel);
void ControlPanel_UpdateSustainedInputTimer(struct ControlPanel *panel);
u32 ControlPanel_GetInputMultiplier(struct ControlPanel *panel);
void ControlPanel_ApplyMultiplierMode(struct ControlPanel *panel);
void ControlPanel_TickMultiplierTimer(struct ControlPanel *panel);
void ControlPanel_HideMultiplierSprites(struct ControlPanel *panel);
void ControlPanel_UpdatePlayerSegments(struct ControlPanel *panel);
void ControlPanel_ShowInputGauge(struct ControlPanel *panel);
void ControlPanel_HideInputGauge(struct ControlPanel *panel);
void ControlPanel_ShowInteractionDisplay(struct ControlPanel *panel);
void ControlPanel_HideInteractionDisplay(struct ControlPanel *panel);
void ControlPanel_OnStartPromptComplete(struct ControlPanel *panel);
void ControlPanel_CallEndScript(struct ControlPanel *panel);
void ControlPanel_CallLimitScript(struct ControlPanel *panel);
void ControlPanel_BeginInteraction(void);
s32 ControlPanel_AccumulateInput(struct ControlPanel *panel);
void ControlPanel_UpdateIdle(struct ControlPanel *panel);
void ControlPanel_BeginIdle(struct ControlPanel *panel, u32 flag);
void ControlPanel_UpdateInteraction(struct ControlPanel *panel);
void ControlPanel_UpdateScreenBrightness(struct ControlPanel *unused);
u32 ControlPanel_OnTaskUpdate(struct ControlPanel *panel);
s32 ControlPanel_Destroy(struct ControlPanel *panel);
void ControlPanel_InitializeValueWidgets(struct ControlPanel *panel);
void ControlPanel_InitializeMultiplierIcons(struct ControlPanel *panel);
void ControlPanel_InitializeSegments(struct ControlPanel *panel);
s32 ControlPanel_InitializeFromScript(struct ControlPanel *panel);
struct ControlPanel *ControlPanel_CreateFromScript(void);
#endif
