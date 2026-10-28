// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
#include "area_effect_link.h"
extern void (*const gUnk_086058EC[])(struct ModifierMenu *);
void ModifierMenu_CloseScript(struct ModifierMenu *);
void ModifierMenu_ClearDisplay(struct ModifierMenu *);
void ModifierMenu_UpdatePreviewAppearance(struct ModifierMenu *);
void ModifierMenu_StopListTextPlayback(struct ModifierMenu *);
void ModifierMenu_ClearBackground(struct ModifierMenu *);
void ModifierMenu_DrawBackground(struct ModifierMenu *);
void ModifierMenu_UpdateHeaderObjects(struct ModifierMenu *);
void ModifierMenu_DrawStateContents(struct ModifierMenu *);
void ModifierMenu_RefreshTextLabels(struct ModifierMenu *);
void ModifierMenu_UpdateDescription(struct ModifierMenu *);
void ModifierMenu_RefreshPreviewModel(struct ModifierMenu *);
void ModifierMenu_UpdateCapacityFlash(struct ModifierMenu *);
void ModifierMenu_DrawClockAndValue(void);
static inline u32 actorInactive(void)
{
    struct ModifierMenuActor *actor = gLinkSession;
    register u32 value asm("r0");
    if (!actor || actor->active)
        goto available;
    value = 1;
    return value;
available:
    value = 0;
    return value;
}
s32 ModifierMenu_UpdateTask(struct ModifierMenu *menu)
{
    u32 inactive = actorInactive();
    asm("" : "+r"(inactive));
    if (inactive) {
        Task_Kill(menu);
        asm("" : : "r"(menu) : "memory");
        goto done;
    }
    if (menu->state == 3) {
        if (menu->stateFrame > 33) {
            ModifierMenu_CloseScript(menu);
            ModifierMenu_ClearDisplay(menu);
            Task_Kill(menu);
            goto done;
        }
        ModifierMenu_UpdatePreviewAppearance(menu);
        goto incrementFrame;
    }
    Callback_InvokeActor(menu, gUnk_086058EC[menu->state]);
    if (menu->dirtyFlags) {
        if (menu->dirtyFlags & 1)
            ModifierMenu_StopListTextPlayback(menu);
        if (menu->dirtyFlags & 2) {
            ModifierMenu_ClearBackground(menu);
            if (menu->dirtyFlags & 2)
                ModifierMenu_DrawBackground(menu);
        }
        if (menu->dirtyFlags & 4)
            ModifierMenu_UpdateHeaderObjects(menu);
        if (menu->dirtyFlags & 8)
            ModifierMenu_DrawStateContents(menu);
        if (menu->dirtyFlags & 1)
            ModifierMenu_RefreshTextLabels(menu);
        if (menu->dirtyFlags & 16)
            ModifierMenu_UpdateDescription(menu);
        if (menu->dirtyFlags & 32)
            ModifierMenu_RefreshPreviewModel(menu);
    }
    ModifierMenu_UpdateCapacityFlash(menu);
    ModifierMenu_UpdatePreviewAppearance(menu);
    ModifierMenu_DrawClockAndValue();
    menu->dirtyFlags = 0;
incrementFrame:
    menu->stateFrame++;
done:
    return 0;
}
