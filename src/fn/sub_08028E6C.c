#include "global.h"
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
extern u32 gUnk_0300523C;
int Vampire_TryQueueCommand(int value)
{
    struct Actor *p = SceneActors_GetBySlot(0);
    if (p && p->handle && !p->command && !(gUnk_0300523C & 7)) {
        p->command = 1;
        p->argument = value;
        return 1;
    }
    return 0;
}
