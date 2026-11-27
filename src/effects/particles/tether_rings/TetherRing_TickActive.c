#include "effects/particles/proximity_emitter.h"
void TetherRing_TickActive(struct ProximityEmitter *emitter)
{
    u16 *nearby = &emitter->nearby;
    if (*nearby == 0) {
        if (TetherRing_IsTetherNearby(emitter) && gTetherActor->activation == 1) {
            *nearby = 1;
            if (emitter->enterScriptId)
                Script_Call(emitter->enterScriptId, 0);
            TetherActor_RequestPaletteBlend(1);
            Sound_PlaySong(0x198);
        }
        if ((++emitter->particleFrame & 7) == 0)
            TetherRing_EmitParticle(emitter, emitter->particleResource);
    } else {
        u32 close = TetherRing_IsTetherNearby(emitter);
        if (!close) {
            if (emitter->leaveScriptId)
                Script_Call(emitter->leaveScriptId, 0);
            TetherActor_RequestPaletteBlend(0);
            *nearby = close;
            Sound_PlaySong(0x199);
        }
        if ((++emitter->particleFrame & 3) == 0)
            TetherRing_EmitParticle(emitter, emitter->particleResource);
    }
}
