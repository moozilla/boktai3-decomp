#include "global.h"
#ifdef NONMATCHING
// fuzzy: 96.5%
// fix.py best (sweep after the inlined-insn fix)
#include "scene_hud_display.h"
#include "player.h"
#include "engine.h"

void PauseMenu_LoadAttributeIconFrame(void *, void *, s32);
void PauseMenu_LoadEquipmentIconFrame(void *, void *, s32);
void PauseMenu_LoadCategoryIconFrame(void *, void *, s32);
void PauseMenu_LoadIndexedIconKeyframe(void *, void *, s32);
s32 PauseMenu_GetSelectedListEntryType(void *);

static inline void show(u8 *p)
{
    *(u32 *)(p + 8) &= ~1;
}

static inline void hide(u8 *p)
{
    *(u32 *)(p + 8) |= 1;
}

void sub_08168214(struct SceneHudState *p)
{
    s32 wide = 0;
    s32 i;
    s32 k;
    s32 n;
    if ((((u8)(p->actionOwner->actionMode - 1)) > 1) || (gEngineFlags & 0x1000))
        wide = 1;
    for (i = 0; i < 3; i++) {
        n = p->primaryVisible;
        if (i < n) {
            k = i + p->primarySelection;
            if (k >= n)
                k -= n;
            {
                s32 off1 = k * 4;
                k = *((s8 *)((((u8 *)p) + off1) + 0xC79));
            }
            if (((u32)k) <= 11) {
                u8 *row = ((u8 *)p) + (i * 0x60);
                show(row + 0xC88);
                PauseMenu_LoadAttributeIconFrame(p, &p->primaryWidgets[i], k);
            } else {
                hide((u8 *)(&p->primaryWidgets[i]));
            }
        } else {
            hide((u8 *)(&p->primaryWidgets[i]));
        }
        n = p->secondaryCount;
        if ((i < n) && wide) {
            u8 *e;
            k = i + p->selection;
            if (k >= n)
                k -= n;
            show((u8 *)(&p->secondaryWidgets[i]));
            e = ((u8 *)p) + (k * 8);
            if (e[0xDAA])
                PauseMenu_LoadEquipmentIconFrame(p, &p->secondaryWidgets[i], e[0xDAB]);
            else
                PauseMenu_LoadCategoryIconFrame(p, &p->secondaryWidgets[i], e[0xDAB]);
        } else {
            hide((u8 *)(&p->secondaryWidgets[i]));
        }
        n = p->tertiaryVisible;
        if ((i < n) && wide) {
            k = i + p->tertiarySelection;
            if (k >= n)
                k -= n;
            if (PauseMenu_GetSelectedListEntryType(p))
                show((u8 *)(&p->tertiaryWidgets[i]));
            else
                hide((u8 *)(&p->tertiaryWidgets[i]));
            Anim_LoadKeyframe(&p->tertiaryWidgets[i], ((u8 *)p) + 0xA4,
                              (u16)(p->tertiaryOptions[k].icon + 0xE7), 0);
        } else {
            hide((u8 *)(&p->tertiaryWidgets[i]));
        }
    }

    if (p->primaryVisible) {
        show((u8 *)(&p->extraWidgetsA[0]));
        PauseMenu_LoadIndexedIconKeyframe(p, &p->extraWidgetsA[0], p->primarySelection);
        show((u8 *)(&p->extraWidgetsB[0]));
        PauseMenu_LoadIndexedIconKeyframe(p, &p->extraWidgetsB[0], p->primaryVisible - 1);
        show((u8 *)(&p->extraWidgetsC[0]));
    } else {
        hide((u8 *)(&p->extraWidgetsA[0]));
        hide((u8 *)(&p->extraWidgetsB[0]));
        hide((u8 *)(&p->extraWidgetsC[0]));
    }
    show((u8 *)(&p->extraWidgetsA[1]));
    PauseMenu_LoadIndexedIconKeyframe(p, &p->extraWidgetsA[1], p->tertiarySelection);
    show((u8 *)(&p->extraWidgetsB[1]));
    PauseMenu_LoadIndexedIconKeyframe(p, &p->extraWidgetsB[1], p->tertiaryVisible - 1);
    show((u8 *)(&p->extraWidgetsC[1]));
    show((u8 *)(&p->extraWidgetsA[2]));
    PauseMenu_LoadIndexedIconKeyframe(p, &p->extraWidgetsA[2], p->selection);
    show((u8 *)(&p->extraWidgetsB[2]));
    PauseMenu_LoadIndexedIconKeyframe(p, &p->extraWidgetsB[2], p->secondaryCount - 1);
    show((u8 *)(&p->extraWidgetsC[2]));
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08168214);
#endif
