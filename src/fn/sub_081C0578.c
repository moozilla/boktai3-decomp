#include "global.h"
void ScenePaletteFade_SetState(void *, void (*)(void *));
void ScenePaletteFade_UpdateHold(void *);
void ScenePaletteFade_BeginHold(void *p)
{
    ScenePaletteFade_SetState(p, ScenePaletteFade_UpdateHold);
}
