#define SceneEffect_InitializeMovingTask SceneOrbitHazard_Init
#include "global.h"
struct SceneEffectMoving {
    u8 unknown[0xC8];
    const void *origin;
    u8 unknownCC[0x10];
    u16 resourceId;
    u16 frame;
    u8 state;
    u8 phase;
    u8 angle;
    u8 unknownE3;
};
void *Task_Create(u32, u32);
void Task_Kill(void *);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void SceneEffect_UpdateMovingState(void);
void SceneOrbitHazard_OnDestroy(void);
s32 SceneOrbitHazard_Init(struct SceneEffectMoving *);
struct SceneEffectMoving *SceneEffect_CreateMovingTask(u32 resourceId, const void *origin)
{
    struct SceneEffectMoving *p = Task_Create(8, 0xE4);
    if (p) {
        Task_SetCallbacks(p, SceneEffect_UpdateMovingState, SceneOrbitHazard_OnDestroy);
        p->resourceId = 30;
        p->origin = 0;
        if (SceneOrbitHazard_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
        p->resourceId = resourceId;
        p->origin = origin;
    }
    return p;
}
