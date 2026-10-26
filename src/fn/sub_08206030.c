#include "global.h"
#include "engine.h"
struct SceneDispatchTask {
    u8 unknown0[0x20];
    u32 flags;
    u8 unknown24[0xE0];
    void *callback;
    u8 unknown108[0x3A];
    u16 ticks;
};
extern u32 gUnk_030053F0, gUnk_02000560;
s32 Callback_InvokeActor(void *, void *);
static inline u32 TestPair(u32 mask, u32 *a, u32 *b)
{
    return (*a | *b) & mask;
}
static inline u32 TestFlag(u32 mask, u32 *p)
{
    return *p & mask;
}
s32 SceneTask_DispatchUnlessPaused(struct SceneDispatchTask *p)
{
    s32 result;
    p->flags |= 1;
    if (TestPair(0x400, &gEngineFlags, &gUnk_030053F0) || TestFlag(4, &gUnk_02000560))
        return 0;
    result = Callback_InvokeActor(p, p->callback);
    p->ticks++;
    return result;
}
