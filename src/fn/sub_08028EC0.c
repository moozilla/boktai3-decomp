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
void Vampire_QueueDefaultCommand(void)
{
    struct Actor *p = SceneActors_GetBySlot(0);
    if (p && p->handle) {
        int a = Script_GetKeywordValue(115, 0);
        int b = Script_GetKeywordValue(104, 0);
        if (!p->command) {
            if (a)
                p->command = 3;
            else if (b)
                p->command = 1;
            else
                p->command = 2;
            p->argument = 0;
        }
    }
}
