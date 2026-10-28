#include "global.h"
// fix.py best (sweep after the inlined-insn fix)
#include "scene_hud_display.h"
#include "player.h"
#include "engine.h"

void PauseMenu_LoadAttributeIconFrame(void *, void *, s32);
void PauseMenu_LoadEquipmentIconFrame(void *, void *, s32);
void PauseMenu_LoadCategoryIconFrame(void *, void *, s32);
void PauseMenu_LoadIndexedIconKeyframe(void *, void *, s32);
s32 PauseMenu_GetSelectedListEntryType(void *);

#define PAUSE_ROW_FIELD(base, member)                                                              \
    (*(typeof(((struct SceneHudState *)0)->member) *)((u8 *)(base) + (u32) &                       \
                                                      ((struct SceneHudState *)0)->member))

static inline void PauseMenu_ShowWidget(u8 *p)
{
    ((struct SceneInputWidget *)p)->flags &= ~1;
}

static inline void PauseMenu_HideWidgetWithMask(u8 *p, u32 bit)
{
    ((struct SceneInputWidget *)p)->flags |= bit;
}

static inline void PauseMenu_HideWidget(u8 *p)
{
    ((struct SceneInputWidget *)p)->flags |= 1;
}

void sub_08168214(struct SceneHudState *p)
{
    s32 wide = 0;
    s32 i;
    s32 k;
    s32 n;
    u32 one;
    u8 *row;
    struct SceneInputWidget *third;
    if ((((u8)(p->actionOwner->actionMode - 1)) > 1) || (gEngineFlags & 0x1000))
        wide = 1;
    for (i = 0, one = 1, row = (u8 *)p, third = p->tertiaryWidgets; i < 3; i++) {
        n = p->primaryVisible;
        if (i < n) {
            k = i + p->primarySelection;
            if (k >= n)
                k -= n;
            {
                s32 off1 = k * 4;
                k = PAUSE_ROW_FIELD((u8 *)p + off1, primaryOptions[0].icon);
            }
            if (((u32)k) <= 11) {
                PauseMenu_ShowWidget((u8 *)&PAUSE_ROW_FIELD(row, primaryWidgets[0]));
                PauseMenu_LoadAttributeIconFrame(p, &p->primaryWidgets[i], k);
            } else {
                PauseMenu_HideWidgetWithMask((u8 *)(&p->primaryWidgets[i]), one);
            }
        } else {
            PauseMenu_HideWidgetWithMask((u8 *)(&p->primaryWidgets[i]), one);
        }
        n = p->secondaryCount;
        if ((i < n) && wide) {
            u8 *e;
            k = i + p->selection;
            if (k >= n)
                k -= n;
            PauseMenu_ShowWidget((u8 *)(&p->secondaryWidgets[i]));
            e = ((u8 *)p) + (k * 8);
            if (PAUSE_ROW_FIELD(e, secondaryEntryEnabled))
                PauseMenu_LoadEquipmentIconFrame(p, &p->secondaryWidgets[i],
                                                 PAUSE_ROW_FIELD(e, secondaryEntryIcon));
            else
                PauseMenu_LoadCategoryIconFrame(p, &p->secondaryWidgets[i],
                                                PAUSE_ROW_FIELD(e, secondaryEntryIcon));
        } else {
            PauseMenu_HideWidgetWithMask((u8 *)(&p->secondaryWidgets[i]), one);
        }
        n = p->tertiaryVisible;
        if ((i < n) && wide) {
            k = i + p->tertiarySelection;
            if (k >= n)
                k -= n;
            if (PauseMenu_GetSelectedListEntryType(p))
                PauseMenu_ShowWidget((u8 *)(third));
            else
                PauseMenu_HideWidgetWithMask((u8 *)(third), one);
            Anim_LoadKeyframe(third, p->motionResource, (u16)(p->tertiaryOptions[k].icon + 0xE7),
                              0);
        } else {
            PauseMenu_HideWidgetWithMask((u8 *)(third), one);
        }
        row += 0x60;
        third++;
    }

    if (p->primaryVisible) {
        PauseMenu_ShowWidget((u8 *)(&p->extraWidgetsA[0]));
        PauseMenu_LoadIndexedIconKeyframe(p, &p->extraWidgetsA[0], p->primarySelection);
        PauseMenu_ShowWidget((u8 *)(&p->extraWidgetsB[0]));
        PauseMenu_LoadIndexedIconKeyframe(p, &p->extraWidgetsB[0], p->primaryVisible - 1);
        PauseMenu_ShowWidget((u8 *)(&p->extraWidgetsC[0]));
    } else {
        PauseMenu_HideWidget((u8 *)(&p->extraWidgetsA[0]));
        PauseMenu_HideWidget((u8 *)(&p->extraWidgetsB[0]));
        PauseMenu_HideWidget((u8 *)(&p->extraWidgetsC[0]));
    }
    PauseMenu_ShowWidget((u8 *)(&p->extraWidgetsA[1]));
    PauseMenu_LoadIndexedIconKeyframe(p, &p->extraWidgetsA[1], p->tertiarySelection);
    PauseMenu_ShowWidget((u8 *)(&p->extraWidgetsB[1]));
    PauseMenu_LoadIndexedIconKeyframe(p, &p->extraWidgetsB[1], p->tertiaryVisible - 1);
    PauseMenu_ShowWidget((u8 *)(&p->extraWidgetsC[1]));
    PauseMenu_ShowWidget((u8 *)(&p->extraWidgetsA[2]));
    PauseMenu_LoadIndexedIconKeyframe(p, &p->extraWidgetsA[2], p->selection);
    PauseMenu_ShowWidget((u8 *)(&p->extraWidgetsB[2]));
    PauseMenu_LoadIndexedIconKeyframe(p, &p->extraWidgetsB[2], p->secondaryCount - 1);
    PauseMenu_ShowWidget((u8 *)(&p->extraWidgetsC[2]));
}
