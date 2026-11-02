#include "scene_callback_pool.h"

void NativeEffect_SetState(struct ScenePoolSlot *slot,
                           void (*callback)(struct ScenePoolSlot *, u32))
{
    slot->callback = callback;
    slot->entered = 1;
}
