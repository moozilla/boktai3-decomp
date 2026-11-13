#ifndef GUARD_REGISTRY_SELECTION_MENU_H
#define GUARD_REGISTRY_SELECTION_MENU_H
#include "menu_cursor.h"

struct PositionMotion {
    struct ContactPosition *position;
    struct ContactPosition from, to;
    u8 unknown14[0x14];
    u8 mode, active;
    u8 unknown2A[2];
    s16 radius, radius2, radius3;
    u16 duration;
    u32 timer;
};
struct RegistrySelectionMenu {
    struct Task task;
    u32 unknown18;
    u8 *palettes;
    struct SceneInputWidget animations[13];
    struct SceneInputResource resource;
    s16 elapsed;
    u8 shift, mode;
    u32 unknown524;
    u16 availableEntries, inputDisabled;
    s32 selection;
    s16 initialSelection, cancelSelection;
    s32 entryValues[9];
    struct MenuCursor *selectionCursor;
    void (*update)(struct RegistrySelectionMenu *);
    s32 timer;
};

void PositionMotion_InitTimedInterpolation(struct PositionMotion *, struct ContactPosition *,
                                           struct ContactPosition *, struct ContactPosition *, u16);
void RegistrySelection_SetUpdate(struct RegistrySelectionMenu *,
                                 void (*)(struct RegistrySelectionMenu *));
void RegistrySelection_ShowIndicators(struct RegistrySelectionMenu *);
void RegistrySelection_UpdateIndicators(struct RegistrySelectionMenu *);
void RegistrySelection_BlendPaletteBanks(u8 *, u8 *, u8 *, int, int, int);
void RegistrySelection_ApplyPalettePhase(struct RegistrySelectionMenu *, u32);
void RegistrySelection_BlendTimedPalettes(struct RegistrySelectionMenu *);
void RegistrySelection_ProcessConfirm(struct RegistrySelectionMenu *);
void RegistrySelection_OnUnusedTransition(void);
void RegistrySelection_InitializeAnimations(struct RegistrySelectionMenu *);
void RegistrySelection_InitializeBackground(struct RegistrySelectionMenu *);
void RegistrySelection_ConfigureDisplay(void);
void RegistrySelection_BeginFadeIn(struct RegistrySelectionMenu *);
void RegistrySelection_BeginFadeOut(struct RegistrySelectionMenu *);
void RegistrySelection_BeginSelection(struct RegistrySelectionMenu *);
void RegistrySelection_StartTimedEffect(struct RegistrySelectionMenu *, s32, s32);
void RegistrySelection_UpdateTimedEffect(struct RegistrySelectionMenu *);
void RegistrySelection_UpdateSelection(struct RegistrySelectionMenu *);
void RegistrySelection_UpdateOpeningDelay(struct RegistrySelectionMenu *);
void RegistrySelection_UpdateScriptDelay(struct RegistrySelectionMenu *);
void RegistrySelection_OnUpdate(struct RegistrySelectionMenu *);
void RegistrySelection_Destroy(struct RegistrySelectionMenu *);
void RegistrySelection_CreateSelectionCursor(struct RegistrySelectionMenu *, s32);
s32 RegistrySelection_InitializeFromScript(struct RegistrySelectionMenu *, void *);
void RegistrySelection_StartTimedEffectFromScript(void);
void RegistrySelection_SetVisibleFromScript(void);
struct RegistrySelectionMenu *RegistrySelection_Create(void *);
#endif
