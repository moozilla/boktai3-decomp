#ifndef GUARD_HUD_H
#define GUARD_HUD_H
#include "hud_stat_display.h"

void Hud_DrawHealthMeterCells(struct SceneHudState *hud, s32 mode, s32 x, s32 y, s32 cap, s32 value,
                              s32 maximum);
void Hud_DrawHealthMeter(struct SceneHudState *hud, u32 mode, u32 x, u32 y, u32 cap);
void Hud_DrawDefaultHealthMeter(struct SceneHudState *hud, u32 mode, u32 x, u32 y);
void Hud_DrawEnergyMeter(struct SceneHudState *hud, s32 mode, s32 x, s32 y, s32 value, s32 maximum,
                         s32 cap);
void Hud_DrawDefaultEnergyMeter(struct SceneHudState *hud, s32 mode, s32 x, s32 y, s32 value,
                                s32 maximum);
void Hud_DrawHealthLabel(s32 unused, s32 x, s32 y);
void Hud_DrawEnergyLabel(s32 unused, s32 x, s32 y);
void Hud_DrawAlternateMeterLabel(s32 unused, s32 x, s32 y);
void Hud_DrawPlayerGauge(struct SceneHudState *hud, s32 x, s32 y);
void Hud_DrawWarningMeter(struct SceneHudState *hud, s32 mode, u32 x, u32 y);
void Hud_DrawTwoDigitCounter(s32 value);
void Hud_ClearCounterTiles(void);
void Hud_DrawStoredEnergyGauge(struct SceneHudState *hud, s32 mode);
void Hud_UpdateTransientStatGauge(struct SceneHudState *hud);
void Hud_UpdateTimedStatusIcon(struct SceneHudState *hud);
void Hud_PlayActionChangeCue(struct SceneHudState *hud, u32 group, u32 mode, u32 selection);
void Hud_UpdateCountdownDigits(struct SceneHudState *hud);
void Hud_DrawDecimalDigits(s32 value, s32 divisor, s32 x, s32 y, s32 alternateStyle);
void Hud_FillDigitCells(s32 count, u32 x, u32 y, s32 sign);
void Hud_DrawActiveStatValue(s32 x1, s32 y1, s32 x2, s32 y2, s32 style, s32 mode);
void Hud_DrawDefaultResourceCounts(s32 x1, s32 y1, s32 x2, s32 y2, s32 style);
void Hud_FillTileRun(s32 count, u32 x, u32 y);
void Hud_DrawCounter7B0(s32 x, s32 y);
void Hud_DrawCounter50C(s32 x, s32 y);
void Hud_DrawHeaderCounter(void);
void Hud_DrawSelectedCounter(struct SceneHudState *hud);
void Hud_DrawSunClockTimeB(s32 x, s32 y);
void Hud_DrawSunClockTimeA(s32 x, s32 y);
void Hud_DrawClockLabelB(u32 x, u32 y);
void Hud_DrawClockLabelA(u32 x, u32 y);
void Hud_DrawSelectedClock(struct SceneHudState *hud);
void Hud_DrawStatusMetersAndCounts(struct SceneHudState *hud, s32 x, s32 y);
#endif
