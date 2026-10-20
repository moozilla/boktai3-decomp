#include "global.h"
#include "engine.h"

struct HresvelgrAnimActor {
    u8 pad0[0xA3];
    u8 clearOnStart;
    u8 padA4[0xB6 - 0xA4];
    u8 reset;
    u8 mirrored;
    u16 index;
    u8 padBA[0x32B - 0xBA];
    u8 side;
};
struct HresvelgrAnim {
    u8 pad[8];
    u32 flags;
};

void Hresvelgr_SetAnimation(struct HresvelgrAnimActor *actor, struct HresvelgrAnim *anim,
                            void *data, u32 index, u32 row, u32 flags)
{
    u8 *resetp = &actor->reset;
    u32 zero = 0;
    *resetp = zero;
    actor->index = index;
    Anim_SetKeyframes(anim, data, actor->index, (u16)row, (u16)flags);
    if (!(flags & 1))
        actor->clearOnStart = zero;
    if (actor->side == 0) {
        actor->mirrored = 1;
        anim->flags |= 4;
    } else {
        actor->mirrored = zero;
        anim->flags &= ~4;
    }
}
