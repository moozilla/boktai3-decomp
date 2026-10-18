#include "global.h"

void SceneActors_RunStateCallback(void);

void SceneActors_TickStateCallback(void)
{
    SceneActors_RunStateCallback();
}
