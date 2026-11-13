#ifndef GUARD_REGISTRY_CHOICE_MENU_H
#define GUARD_REGISTRY_CHOICE_MENU_H
#include "menu_cursor.h"

/* Registry script 3F74: eleven choices, with a separate script for choice one. */
struct RegistryChoiceMenu {
    struct Task task;
    void *background;
    u16 *palettes;
    struct SceneInputWidget animations[5];
    struct SceneInputResource resource;
    u32 unknown220;
    s32 initialSelection, selection;
    struct MenuCursor *cursor;
    s32 entryScripts[11];
    s32 alternateScript;
    void (*update)(struct RegistryChoiceMenu *);
    s32 timer;
};
void Registry3F74_SetState(struct RegistryChoiceMenu *, void (*)(struct RegistryChoiceMenu *));
void Registry3F74_ApplyPaletteStep(struct RegistryChoiceMenu *, u32);
void Registry3F74_UpdateAnimations(struct RegistryChoiceMenu *);
void Registry3F74_ConfirmSelection(struct RegistryChoiceMenu *);
void Registry3F74_InitAnimations(struct RegistryChoiceMenu *);
void Registry3F74_InitBackground(struct RegistryChoiceMenu *);
void Registry3F74_ConfigureDisplay(void);
void Registry3F74_UpdateSelection(struct RegistryChoiceMenu *);
void Registry3F74_UpdateOpeningDelay(struct RegistryChoiceMenu *);
void Registry3F74_UpdateScriptDelay(struct RegistryChoiceMenu *);
void Registry3F74_StartSelection(struct RegistryChoiceMenu *);
void Registry3F74_StartOpeningFade(struct RegistryChoiceMenu *);
void Registry3F74_StartClosingFade(struct RegistryChoiceMenu *);
void Registry3F74_OnUpdate(struct RegistryChoiceMenu *);
void Registry3F74_OnDestroy(struct RegistryChoiceMenu *);
void Registry3F74_CreateSelectionCursor(struct RegistryChoiceMenu *, s32);
s32 Registry3F74_InitFromScript(struct RegistryChoiceMenu *, void *);
struct RegistryChoiceMenu *Registry3F74_Create(void *);
#endif
