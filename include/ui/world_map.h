#ifndef GUARD_REGISTRY_SELECTION_MENU_H
#define GUARD_REGISTRY_SELECTION_MENU_H
#include "ui/widgets/menu_cursor.h"

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
    struct AnimSprite animations[13];
    struct AnimSpriteResource resource;
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
void WorldMap_SetUpdate(struct RegistrySelectionMenu *,
                                 void (*)(struct RegistrySelectionMenu *));
void WorldMap_ShowIndicators(struct RegistrySelectionMenu *);
void WorldMap_UpdateIndicators(struct RegistrySelectionMenu *);
void WorldMap_BlendPaletteBanks(u8 *, u8 *, u8 *, int, int, int);
void WorldMap_ApplyPalettePhase(struct RegistrySelectionMenu *, u32);
void WorldMap_BlendTimedPalettes(struct RegistrySelectionMenu *);
void WorldMap_ProcessConfirm(struct RegistrySelectionMenu *);
void WorldMap_OnUnusedTransition(void);
void WorldMap_InitializeAnimations(struct RegistrySelectionMenu *);
void WorldMap_InitializeBackground(struct RegistrySelectionMenu *);
void WorldMap_ConfigureDisplay(void);
void WorldMap_BeginFadeIn(struct RegistrySelectionMenu *);
void WorldMap_BeginFadeOut(struct RegistrySelectionMenu *);
void WorldMap_BeginSelection(struct RegistrySelectionMenu *);
void WorldMap_StartTimedEffect(struct RegistrySelectionMenu *, s32, s32);
void WorldMap_UpdateTimedEffect(struct RegistrySelectionMenu *);
void WorldMap_UpdateSelection(struct RegistrySelectionMenu *);
void WorldMap_UpdateOpeningDelay(struct RegistrySelectionMenu *);
void WorldMap_UpdateScriptDelay(struct RegistrySelectionMenu *);
void WorldMap_OnUpdate(struct RegistrySelectionMenu *);
void WorldMap_Destroy(struct RegistrySelectionMenu *);
void WorldMap_CreateSelectionCursor(struct RegistrySelectionMenu *, s32);
s32 WorldMap_InitializeFromScript(struct RegistrySelectionMenu *, void *);
void WorldMap_StartTimedEffectFromScript(void);
void WorldMap_SetVisibleFromScript(void);
struct RegistrySelectionMenu *WorldMap_Create(void *);
#endif
