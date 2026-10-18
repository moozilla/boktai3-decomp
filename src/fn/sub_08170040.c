#include "global.h"
int SceneHud_UpdateBinaryChoice(void *);
void Sound_PlaySong(int);
void SceneHud_ReturnToActionGrid(void *);
void Inventory_SortConsumables(int);
void Inventory_SortValuables(int);
struct S {
    u8 pad[0x4388];
    u8 a, b, c;
    u8 pad2[0x4860 - 0x438b];
    u8 d;
};
void PauseMenu_OnSortConfirmationUpdate(struct S *p)
{
    int r = SceneHud_UpdateBinaryChoice(p);
    if (r == 0) {
        Sound_PlaySong(0xde);
        SceneHud_ReturnToActionGrid(p);
    } else if (r == 1) {
        Sound_PlaySong(0xdd);
        if (!p->d)
            Inventory_SortConsumables(0);
        else
            Inventory_SortValuables(0);
        p->a = 0;
        p->b = 0;
        p->c = 0;
        SceneHud_ReturnToActionGrid(p);
    }
}
