#define SceneHud_DispatchSelectedAction sub_0816FB0C
#define SceneHud_UpdateActionSelection  sub_0816FF4C
#define gSceneHudActionDefinitions      gUnk_08E61280
#include "global.h"
#include "scene_hud_display.h"
#include "scene_item_values.h"
extern const struct SceneValueItemDefinition gUnk_08E61280[];
void Sound_PlaySong(u32);
s32 Inventory_GetQuantityByStorage(u32, u32);
void sub_0816DF48(void *);
void sub_0816E2C8(void *);
void sub_0816E570(void *);
void PauseMenu_BeginCharacterItemUse(void *, u32);
void sub_0816ECF0(void *, u32);
void sub_0816F240(void *);
void PauseMenu_BeginTypeThirteenItemUse(void *);
void PauseMenu_BeginTypeFourteenItemUse(void *);
void sub_0816DBAC(void *);
void sub_0816D634(void *, u32);
void sub_0816D98C(void *);
void sub_0816FF4C(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_0816CDB8(void *, int, int);
void sub_0816D920(void *, int);
void sub_0816D868(void *);
void Hud_SaveGridSelection(void *);
void sub_081672FC(void *);
void sub_08166B7C(void *, int);
void sub_0816701C(void *, int, int, int, int, int);
void sub_081663A0(void *, int, int, int, int);
void sub_0816FB0C(struct SceneHudState *p)
{
    int index = Inventory_GetQuantityByStorage(p->actionCategory, p->actionSelection);
    void *q;
    switch (gUnk_08E61280[index].actionKind) {
        case 1:
        case 2:
        case 3:
        case 5:
        case 9:
            sub_0816DF48(p);
            break;
        case 4:
            Sound_PlaySong(0x38A);
            sub_0816E2C8(p);
            break;
        case 7:
            Sound_PlaySong(0x10D);
            if (index == 26)
                Sound_PlaySong(0x182);
            goto apply;
        case 8:
            if (gUnk_08E61280[index].unknown6 == 2)
                sub_0816E570(p);
            else
                PauseMenu_BeginCharacterItemUse(p, gUnk_08E61280[index].unknown6);
            break;
        case 10:
            sub_0816ECF0(p, gUnk_08E61280[index].unknown6);
            break;
        case 11:
            sub_0816F240(p);
            break;
        case 6:
        case 12:
            Sound_PlaySong(0x10D);
        apply:
            sub_0816D634(p, p->actionSelection);
            sub_0816D98C(p);
            Hud_SetInteractionCallback(p, sub_0816FF4C, 1);
            q = p->actionInput;
            sub_0816CDB8(q, 1, 0);
            sub_0816D920((u8 *)p + 0x1620, p->actionCursor);
            sub_0816D868(p);
            Hud_SaveGridSelection(q);
            sub_081672FC(p);
            sub_08166B7C(p, 4);
            sub_0816701C(p, 0, 0, 0, 1, 0);
            sub_081663A0(p, 1, 0, 0, 0);
            break;
        case 13:
            Sound_PlaySong(0x38A);
            PauseMenu_BeginTypeThirteenItemUse(p);
            break;
        case 14:
            Sound_PlaySong(0x38A);
            PauseMenu_BeginTypeFourteenItemUse(p);
            break;
        case 16:
            sub_0816DBAC(p);
            break;
    }
}
