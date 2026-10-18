#define SceneEffect_CreateEntryTask     sub_0819AA8C
#define SceneEffect_UpdateEntryState    sub_0819AA08
#define SceneEffect_InitializeEntryTask SceneBurstEmitter_Init
#include "global.h"
struct SceneEffectLink {
    u8 unknown[0x20];
    const void *origin;
    u8 unknown24[0x12];
    u16 resourceId;
    u8 unknown38[8];
};
void *Task_Create(u32, u32);
void Task_Kill(void *);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void sub_0819AA08(void);
void SceneBurstEmitter_OnDestroy(void);
s32 SceneBurstEmitter_Init(struct SceneEffectLink *);
struct SceneEffectLink *sub_0819AA8C(u32 resourceId, const void *origin)
{
    struct SceneEffectLink *p = Task_Create(8, 0x40);
    if (p) {
        Task_SetCallbacks(p, sub_0819AA08, SceneBurstEmitter_OnDestroy);
        p->resourceId = 30;
        p->origin = 0;
        if (SceneBurstEmitter_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
        p->resourceId = resourceId;
        p->origin = origin;
    }
    return p;
}
