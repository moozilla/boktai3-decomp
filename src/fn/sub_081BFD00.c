#define gStorageCategoryKeyframes gUnk_0824F928
#include "global.h"
#include "scene_input.h"

/* Tentative Storage browse update; category selection uses L/R and B closes. */

struct StorageMenu {
    u8 unknown0[0x80];
    u32 primaryFlags;
    u8 unknown84[0x54];
    struct SceneInputWidget blinkWidget;
    u8 unknown138[0x60];
    u8 categoryAnimation[0x60];
    u8 groupAnimation[0x1320];
    u8 animationData[0x234];
    u8 active;
    u8 unknown174D[7];
    u8 categorySelection;
    u8 categoryCount;
    u8 unknown1756[2];
    u8 categories[6];
    u8 unknown175E;
    u8 blinkCounter;
    u8 unknown1760[0x68];
    u8 highlight[0x2C];
    void *callback;
};
struct StorageCategoryKeyframes {
    u16 entries[3][2];
};
extern const struct StorageCategoryKeyframes gStorageCategoryKeyframes;
s32 Storage_TickCategorySelection(struct StorageMenu *);
void Storage_RefreshCategoryWidgets(struct StorageMenu *);
void Storage_InitSlotMapping(struct StorageMenu *, s32);
void Storage_UpdateCloseDelay(void);
void Storage_SetUpdateCallback(struct StorageMenu *, void (*)(void));
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void SceneInputPanel_TickHighlight(void *);
void BgGfx_LoadTilemapAtPosition(u32, u32, u32);

static inline void ShowWidget(struct SceneInputWidget *widget)
{
    widget->flags |= 1;
}
static inline void HideWidget(struct SceneInputWidget *widget)
{
    widget->flags &= ~1;
}

void Storage_UpdateBrowse(struct StorageMenu *p)
{
    if (p->active) {
        p->blinkCounter++;
        if ((p->blinkCounter >> 5) & 1) {
            p->primaryFlags |= 1;
            ShowWidget(&p->blinkWidget);
        } else {
            p->primaryFlags &= ~1;
            HideWidget(&p->blinkWidget);
        }
        if (Storage_TickCategorySelection(p)) {
            s32 category;
            struct StorageCategoryKeyframes keyframes;
            Sound_PlaySong(0x10A);
            Storage_InitSlotMapping(p, (p->categories[p->categorySelection] & 1) << 4);
            Storage_RefreshCategoryWidgets(p);
            category = p->categories[p->categorySelection];
            Anim_LoadKeyframe(p->categoryAnimation, p->animationData, category + 0xD3, 0);
            category = p->categories[p->categorySelection];
            keyframes = gStorageCategoryKeyframes;
            if ((u32)category <= 5)
                Anim_LoadKeyframe(p->groupAnimation, p->animationData,
                                  keyframes.entries[category >> 1][0], 0);
        }
        if (gPadState[1] & 2) {
            Sound_PlaySong(0x119);
            ScreenTransition_SendCommand(1, 5, 4, 4, 4, 0xFFFF, 0);
            Storage_SetUpdateCallback(p, Storage_UpdateCloseDelay);
            return;
        }
    } else {
        p->primaryFlags |= 1;
        ShowWidget(&p->blinkWidget);
        p->blinkCounter = 0;
    }
    SceneInputPanel_TickHighlight(p->highlight);
    BgGfx_LoadTilemapAtPosition(0, 0, 0);
    Callback_InvokeActor(p, p->callback);
}
