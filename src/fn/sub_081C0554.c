#include "global.h"
void ScenePaletteFade_SetState(void *, void (*)(void *));
void ScenePaletteFade_UpdateDelay(void *);
void ScenePaletteFade_BeginDelay(void *p)
{
    ScenePaletteFade_SetState(p, ScenePaletteFade_UpdateDelay);
}
