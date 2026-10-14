// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
#define ModifierMenu_UpdateTask              sub_08041F3C
#define ModifierMenu_UpdateCapacityFlash     sub_08041D10
#define ModifierMenu_UpdatePreviewAppearance sub_08041DA8
struct ModifierMenuActor {
    u8 unknown0[0x1C];
    u8 active;
};
extern struct ModifierMenuActor *gUnk_020004B4;
extern void (*const gUnk_086058EC[])(struct ModifierMenu *);
void ModifierMenu_CloseScript(struct ModifierMenu *);
void sub_08041F10(struct ModifierMenu *);
void ModifierMenu_UpdatePreviewAppearance(struct ModifierMenu *);
void sub_08041480(struct ModifierMenu *);
void ModifierMenu_ClearBackground(struct ModifierMenu *);
void ModifierMenu_DrawBackground(struct ModifierMenu *);
void ModifierMenu_UpdateHeaderObjects(struct ModifierMenu *);
void sub_08040FC0(struct ModifierMenu *);
void sub_08041370(struct ModifierMenu *);
void ModifierMenu_UpdateDescription(struct ModifierMenu *);
void sub_08041908(struct ModifierMenu *);
void ModifierMenu_UpdateCapacityFlash(struct ModifierMenu *);
void sub_0811DCAC(void);
static inline u32 actorInactive(void)
{
    struct ModifierMenuActor *actor = gUnk_020004B4;
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
            sub_08041F10(menu);
            Task_Kill(menu);
            goto done;
        }
        ModifierMenu_UpdatePreviewAppearance(menu);
        goto incrementFrame;
    }
    sub_0824923C(menu, gUnk_086058EC[menu->state]);
    if (menu->dirtyFlags) {
        if (menu->dirtyFlags & 1)
            sub_08041480(menu);
        if (menu->dirtyFlags & 2) {
            ModifierMenu_ClearBackground(menu);
            if (menu->dirtyFlags & 2)
                ModifierMenu_DrawBackground(menu);
        }
        if (menu->dirtyFlags & 4)
            ModifierMenu_UpdateHeaderObjects(menu);
        if (menu->dirtyFlags & 8)
            sub_08040FC0(menu);
        if (menu->dirtyFlags & 1)
            sub_08041370(menu);
        if (menu->dirtyFlags & 16)
            ModifierMenu_UpdateDescription(menu);
        if (menu->dirtyFlags & 32)
            sub_08041908(menu);
    }
    ModifierMenu_UpdateCapacityFlash(menu);
    ModifierMenu_UpdatePreviewAppearance(menu);
    sub_0811DCAC();
    menu->dirtyFlags = 0;
incrementFrame:
    menu->stateFrame++;
done:
    return 0;
}
