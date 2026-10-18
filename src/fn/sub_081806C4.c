#define SceneMotion_InitState sub_081806C4
#define SceneMotion_CreateTask FrostEmitter_Create
#include "global.h"
struct SceneMotion {
    u8 unknown00[8];
    u32 position[2];
    void *task;
    u8 unknown14[0x24];
    s16 active;
};
void *FrostEmitter_Create(void);
static inline void ClearPosition(void *destination, u32 *source)
{
    *source = 0;
    CpuSet(source, destination, 0x05000002);
}
void *sub_081806C4(struct SceneMotion *motion)
{
    u32 *position = motion->position;
    u32 zero;
    void *task;
    ClearPosition(position, &zero);
    task = FrostEmitter_Create();
    motion->task = task;
    motion->active = 0;
    /* Original ABI leaves the task result in r0; callers ignore it. */
}
