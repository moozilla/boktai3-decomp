#include "global.h"
#include "background_controller.h"

s32 SceneTilemap_IsActive(void)
{
    if (gSceneCamera == NULL) {
        return 0;
    }
    return 1;
}
