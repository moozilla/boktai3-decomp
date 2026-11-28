#include "global.h"
#include "minigames/result_game/result_tutorial.h"
s32 ResultMenu_Initialize(struct ResultMenu *menu, void *owner, void *unused);
void ResultMenu_Update(void);
void ResultMenu_OnTaskExit(void);
void *ResultMenu_Open(void *a, void *b)
{
    struct ResultMenu *menu = Task_Create(11, sizeof(*menu));
    if (menu != 0) {
        Task_SetCallbacks(menu, ResultMenu_Update, ResultMenu_OnTaskExit);
        if (ResultMenu_Initialize(menu, a, b) < 0) {
            Task_Kill(menu);
            return 0;
        }
    }
    return menu;
}
