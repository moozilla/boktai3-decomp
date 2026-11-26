#include "engine/shared/global.h"
#include "engine/shared/engine.h"
#include "engine/shared/ram.h"
#include "engine/shared/engine_pause.h"
#include "engine/shared/word_flags.h"
struct SceneDispatchTask {
    u8 unknown0[0x20];
    u32 flags;
    u8 unknown24[0xE0];
    void *callback;
    u8 unknown108[0x3A];
    u16 ticks;
};
extern u32 gUnk_02000560;
s32 Callback_InvokeActor(void *, void *);
s32 SceneTask_DispatchUnlessPaused(struct SceneDispatchTask *p)
{
    s32 result;
    p->flags |= 1;
    if (Engine_PauseBits(0x400) || Flags_MaskedWord(4, &gUnk_02000560))
        return 0;
    result = Callback_InvokeActor(p, p->callback);
    p->ticks++;
    return result;
}
