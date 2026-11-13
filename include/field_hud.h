#ifndef GUARD_FIELD_HUD_H
#define GUARD_FIELD_HUD_H
#include "scene_hud_display.h"
#include "scene_pool.h"
#include "scene_camera.h"
#include "link_session_state.h"

void FieldHud_OnUnusedUpdate(void);
void FieldHud_OnUnusedCleanup(void);
u16 *Background_GetWrappedTileEntry(u32 i, u32 x, u32 y);
void FieldHud_UpdateGaugePalette(struct FieldHudWidgets *p);
void FieldHud_RefreshGaugeIndex(struct FieldHudWidgets *p);
void FieldHud_UpdateTimeDigits(struct FieldHudWidgets *p);
s32 NumericDisplay_InitializeWidgets(struct FieldHudWidgets *s, s32 *timeSource);
void Palette_InitColorTransition(struct FieldHudWidgets *p, u32 x, u32 y, u32 z, u32 endX, u32 endY,
                                 u32 endZ, u32 duration);
void FieldHud_UpdateNumberDisplay(struct FieldHudWidgets *p);
s32 ViewportGauge_ComputeFill(struct FieldHudWidgets *gauge, s32 slot);
void MeterDisplay_InitializeWidgets(struct FieldHudWidgets *s);
void FieldHud_InitializeNumber(struct FieldHudWidgets *p, s32 *source);
void FieldHud_InitializeGauge(struct FieldHudWidgets *p, s32 *source);
void MeterDisplay_UpdateOwnerMeters(struct FieldHudWidgets *s);
void FieldHud_UpdateWidgets(struct SceneHudState *p);
void FieldHud_InitializeWidgets(s32 *value, s32 *gauge, void *a, u16 b, s32 *timeSource);
#endif
