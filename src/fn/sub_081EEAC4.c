#include "global.h"

void ScreenLayers_HidePair(void *);
void Task_Kill(void *);

void SceneActors_DestroyTask(void *p)
{
    ScreenLayers_HidePair(p);
    Task_Kill(p);
}
