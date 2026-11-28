#include "global.h"
#include "minigames/result_game/result_tutorial.h"
int Script_GetKeywordValue(int, int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
void Camera_SetPosition(void *);
int ResultMenu_ReadScript(struct ResultMenu *menu)
{
    struct SceneVec *camera;
    menu->receiverId = Script_GetKeywordValue('n', 0x6740);
    menu->receiverKind = 5;
    if (Script_SeekToKeyword('p')) {
        menu->cameraPosition.x = Script_GetValue();
        menu->cameraPosition.y = Script_GetValue();
        menu->cameraPosition.z = Script_GetValue();
    } else {
        menu->cameraPosition.x = 0;
        menu->cameraPosition.y = 0;
        menu->cameraPosition.z = 0;
    }
    camera = &menu->cameraPosition;
    menu->initialCameraPosition = menu->cameraPosition;
    Camera_SetPosition(camera);
    menu->completionScript = Script_GetKeywordValue('e', 0);
    menu->difficulty = Script_GetKeywordValue('l', 0);
    if (Script_SeekToKeyword('s')) {
        menu->equipmentSlot = Script_GetValue();
        menu->consumableSlot = Script_GetValue();
        menu->wideTimingWindow = Script_GetKeywordValue('b', 0);
        return 0;
    }
    return -1;
}
