#include "global.h"
#include "minigames/result_game/result_tutorial.h"
int Solar_GetLevel(void);
void sub_0801C24C(void *);
void Camera_SetPosition(void *);
int ResultMenu_Update(struct ResultMenu *menu)
{
    menu->level = Solar_GetLevel();
    Callback_InvokeActor(menu, menu->update);
    sub_0801C24C(menu);
    Camera_SetPosition(&menu->cameraPosition);
    return 0;
}
