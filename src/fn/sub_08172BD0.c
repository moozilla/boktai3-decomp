#define SceneHud_UpdateActionGridInput      sub_08172BD0
#define SceneHud_ShowSelectionDescription   sub_081723D8
#define SceneHud_RefreshEquippedActionIcons sub_081724D4
#define SceneHud_SetActionCursorPosition    sub_081727AC
#include "global.h"
#include "scene_hud_display.h"
void sub_08166B7C(void *, int);
void sub_08166E14(void *, u32, u32, u32, u32, u32);
u32 sub_08166ED4(void *, int, int, int, int);
void sub_0816701C(void *, int, int, int, int, int);
int sub_0816B59C(void *, int);
void Hud_SaveGridSelection(void *);
void sub_081724D4(void *);
void sub_08172638(void *);
void sub_081727AC(void *, u32);
void PauseInventory_ReturnToEquipmentSelection(void *);
void sub_08172100(void *);
u8 sub_08172270(void *);
void sub_081723D8(void *);
void sub_08172BD0(struct SceneHudState *p)
{
    u32 initialGroup = p->actionGridGroup;
    s32 unlockGroup = 1;
    s32 moved;
    void *grid;
    u8 *cursor;
    if (initialGroup <= 1)
        unlockGroup = 0;
    grid = p->actionInput;
    moved = sub_0816B59C(grid, 0xF0);
    if (moved) {
        sub_081727AC(&p->categoryWidgets[5].position, *(cursor = &p->actionCursor));
        sub_081723D8(p);
        Hud_SaveGridSelection(grid);
        {
            u32 selected = *cursor;
            s32 group = 1;
            if (selected <= 7)
                group = 0;
            sub_08166B7C(p, group);
        }
        if (*cursor > 7)
            sub_0816701C(p, 1, 0, 0, 0, 0);
        else
            sub_08166ED4(p, 3, 14, 0, 0);
    } else if (gUnk_03005260[1] & 1) {
        if (!unlockGroup)
            sub_08172100(grid);
        else if (sub_08172270(grid)) {
            u32 result = sub_08166ED4(p, 0, 0, 1, moved);
            sub_08166E14(p, 3, 14, result, moved, 0x47);
        }
        sub_08172638(p);
        {
            u32 selected = p->actionCursor;
            s32 group = 1;
            if (selected <= 7)
                group = 0;
            sub_08166B7C(p, group);
        }
        sub_081724D4(p);
    } else if (gUnk_03005260[1] & 2) {
        Sound_PlaySong(0xDE);
        PauseInventory_ReturnToEquipmentSelection(p);
    }
}
