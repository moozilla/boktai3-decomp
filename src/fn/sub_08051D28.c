#include "proximity_emitter.h"
#define ProximityEmitter_TickGrow sub_08051D28
void ProximityEmitter_TickGrow(struct ProximityEmitter *emitter)
{
    emitter->primary.sprite.flags |= 1;
    emitter->secondary.sprite.flags &= ~1;
    emitter->stateFrame++;
    if (emitter->stateFrame > 63) {
        emitter->secondary.sprite.flags &= ~2;
        emitter->secondary.sprite.unknown8[0] = 64;
        emitter->secondary.sprite.unknown8[1] = 64;
        emitter->stateFrame = 0;
        emitter->update = sub_08051CB4;
        Sound_PlaySong(0x32F);
    } else {
        emitter->secondary.sprite.flags |= 2;
        emitter->secondary.sprite.unknown8[0] = emitter->stateFrame;
        emitter->secondary.sprite.unknown8[1] = emitter->stateFrame;
        emitter->secondary.sprite.unknown4[2] = emitter->stateFrame << 2;
    }
}
