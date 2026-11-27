#include "monsters/ooze.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/engine_pause.h"
#include "engine/word_flags.h"
#include "player/player.h"
s32 Callback_InvokeActor(void *, void *);
s32 SceneTask_DispatchUnlessPaused(struct Ooze *p)
{
    s32 result;
    p->sprite.flags |= 1;
    if (Engine_PauseBits(0x400) || Flags_MaskedWord(4, &gPlayerSlotStatus[0]))
        return 0;
    result = Callback_InvokeActor(p, p->state);
    p->dispatchTicks++;
    return result;
}
