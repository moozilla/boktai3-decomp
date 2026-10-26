#include "global.h"
#include "render.h"
struct Actor {
    u8 pad0[0x3b9];
    u8 command;
    u8 pad3ba[2];
    int argument;
    u8 pad3c0[0x61c];
    void *handle;
};
struct Actor *SceneActors_GetBySlot(int);
int Script_GetKeywordValue(int, int);
void SceneActors_StopBackgroundFlash(void *);
void BackgroundFlash_BeginAlternating(void *);
void BackgroundFlash_BeginAppearance(void *);
void ScaledShadow_Destroy(void *);
void Collider_Remove(void *);
void Anim_Remove(void *);
void SceneActors_Destroy(void *);
int Vampire_Destroy(u8 *p)
{
    ScaledShadow_Destroy(p + 0x2a0);
    Collider_Remove(p + 0x1cc);
    Anim_Remove(p + 0x240);
    SceneActors_Destroy(p);
    return 0;
}
