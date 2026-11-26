#ifndef GUARD_MINIGAMES_DARK_GENERATOR_H
#define GUARD_MINIGAMES_DARK_GENERATOR_H
#include "ui/widgets/scene_input.h"
#include "effects/projectiles/contact_swarm_sprite.h"
#include "player/player.h"
#include "gba/io_reg.h"
#include "script/script_utility.h"

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
    struct AnimSpriteResource resource;
    struct AnimSprite widgets[5];
    u16 start, cooldown, soundTicks, selection;
    u8 unknown580[8];
    u16 sustainedInput, thresholds[3], multiplier, unknown592;
    s32 ticks, tickLimit, total, totalLimit;
    u32 multiplierScript, limitScript, endScript;
    u16 accumulator, duration, unknown5B4, timer;
    void (*update)(struct ControlPanel *);
};

void DarkGenerator_ClearActivePanel(void);
void DarkGenerator_SetUpdate(struct ControlPanel *panel, void (*v)(struct ControlPanel *));
void DarkGenerator_ClearPlayerInputHistory(void);
void DarkGenerator_ResetPlayerForInteractionPrompt(struct ControlPanel *panel);
void DarkGenerator_UpdateAmbientIndicator(struct ControlPanel *panel);
void DarkGenerator_UpdateGaugeIndicator(struct ControlPanel *gauge);
void DarkGenerator_ShowRewardBalanceDigits(struct ControlPanel *panel);
void DarkGenerator_ShowValueDisplay(struct ControlPanel *panel);
void DarkGenerator_HideValueDisplay(struct ControlPanel *panel);
void DarkGenerator_UpdateSustainedInputTimer(struct ControlPanel *panel);
u32 DarkGenerator_GetInputMultiplier(struct ControlPanel *panel);
void DarkGenerator_ApplyMultiplierMode(struct ControlPanel *panel);
void DarkGenerator_TickMultiplierTimer(struct ControlPanel *panel);
void DarkGenerator_HideMultiplierSprites(struct ControlPanel *panel);
void DarkGenerator_UpdatePlayerSegments(struct ControlPanel *panel);
void DarkGenerator_ShowInputGauge(struct ControlPanel *panel);
void DarkGenerator_HideInputGauge(struct ControlPanel *panel);
void DarkGenerator_ShowInteractionDisplay(struct ControlPanel *panel);
void DarkGenerator_HideInteractionDisplay(struct ControlPanel *panel);
void DarkGenerator_OnStartPromptComplete(struct ControlPanel *panel);
void DarkGenerator_CallEndScript(struct ControlPanel *panel);
void DarkGenerator_CallLimitScript(struct ControlPanel *panel);
void DarkGenerator_BeginInteraction(void);
s32 DarkGenerator_AccumulateInput(struct ControlPanel *panel);
void DarkGenerator_UpdateIdle(struct ControlPanel *panel);
void DarkGenerator_BeginIdle(struct ControlPanel *panel, u32 flag);
void DarkGenerator_UpdateInteraction(struct ControlPanel *panel);
void DarkGenerator_UpdateScreenBrightness(struct ControlPanel *unused);
u32 DarkGenerator_OnTaskUpdate(struct ControlPanel *panel);
s32 DarkGenerator_Destroy(struct ControlPanel *panel);
void DarkGenerator_InitializeValueWidgets(struct ControlPanel *panel);
void DarkGenerator_InitializeMultiplierIcons(struct ControlPanel *panel);
void DarkGenerator_InitializeSegments(struct ControlPanel *panel);
s32 DarkGenerator_InitializeFromScript(struct ControlPanel *panel);
struct ControlPanel *DarkGenerator_CreateFromScript(void);
#endif
