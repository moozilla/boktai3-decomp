#ifndef GUARD_HUD_H
#define GUARD_HUD_H
#include "hud_stat_display.h"
void Hud_DrawHealthMeterCells(struct SceneHudState *p, s32 mode, s32 row, s32 col, s32 cap,
                              s32 value, s32 max);
void Hud_DrawHealthMeter(struct SceneHudState *p, u32 a, u32 b, u32 c, u32 d);
void Hud_DrawDefaultHealthMeter(struct SceneHudState *a, u32 b, u32 c, u32 d);
void Hud_DrawEnergyMeter(struct SceneHudState *hud, s32 mode, s32 row, s32 col, s32 value, s32 max,
                         s32 cap);
void Hud_DrawDefaultEnergyMeter(struct SceneHudState *a, s32 b, s32 c, s32 d, s32 e, s32 f);
void Hud_DrawHealthLabel(s32 unused, s32 x, s32 y);
void Hud_DrawEnergyLabel(s32 unused, s32 x, s32 y);
void Hud_DrawAlternateMeterLabel(s32 unused, s32 x, s32 y);
void Hud_DrawPlayerGauge(struct SceneHudState *p, s32 x, s32 y);
void Hud_DrawTwoDigitCounter(s32 value);
void Hud_ClearCounterTiles(void);
void Hud_DrawStoredEnergyGauge(struct SceneHudState *hud, s32 mode);
void Hud_UpdateTransientStatGauge(struct SceneHudState *p);
void Hud_UpdateTimedStatusIcon(struct SceneHudState *p);
void Hud_PlayActionChangeCue(struct SceneHudState *p, u32 group, u32 mode, u32 selection);
void Hud_UpdateCountdownDigits(struct SceneHudState *p);
void Hud_DrawDecimalDigits(s32 value, s32 div, s32 x, s32 y, s32 flag);
void Hud_FillDigitCells(s32 count, u32 x, u32 y, s32 sign);
void Hud_DrawActiveStatValue(s32 x1, s32 y1, s32 x2, s32 y2, s32 style, s32 mode);
void Hud_DrawDefaultResourceCounts(s32 a, s32 b, s32 c, s32 d, s32 e);
void Hud_FillTileRun(s32 count, u32 x, u32 y);
void Hud_DrawCounter7B0(s32 a, s32 b);
void Hud_DrawCounter50C(s32 a, s32 b);
void Hud_DrawHeaderCounter(void);
void Hud_DrawSelectedCounter(struct SceneHudState *self);
void Hud_DrawSunClockTimeB(s32 x, s32 y);
void Hud_DrawSunClockTimeA(s32 x, s32 y);
void Hud_DrawClockLabelB(u32 x, u32 y);
void Hud_DrawClockLabelA(u32 x, u32 y);
void Hud_DrawSelectedClock(struct SceneHudState *hud);
void Hud_DrawStatusMetersAndCounts(struct SceneHudState *p, s32 b, s32 c);
#endif
