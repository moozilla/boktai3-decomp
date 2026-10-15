// MATCHING_CONCESSION: register-pinning
#include "proximity_emitter.h"
#define ProximityEmitter_TickActive sub_08051BD8
static inline u32 advanceParticleFrame(struct ProximityEmitter *emitter)
{
    u16 *address = &emitter->particleFrame;
    u32 frame = *address + 1;
    register u32 mask asm("r2");
    *address = frame;
    mask = 0xFFFF;
    asm("" : "+r"(mask));
    {
        register u32 applied asm("r1") = mask;
        asm("" : "+r"(applied));
        frame &= applied;
    }
    return frame;
}
void ProximityEmitter_TickActive(struct ProximityEmitter *emitter)
{
    u16 *nearby = &emitter->nearby;
    if (*nearby == 0) {
        if (sub_08051B70(emitter) && gUnk_02000488->activation == 1) {
            *nearby = 1;
            {
                register u32 offset asm("r2") = 0x260;
                u32 script;
                register u32 *scriptAddress asm("r0");
                asm("" : "+r"(offset));
                scriptAddress = (u32 *)((u8 *)emitter + offset);
                script = *scriptAddress;
                if (script)
                    Script_Call(script, 0);
            }
            TetherActor_RequestPaletteBlend(1);
            Sound_PlaySong(0x198);
        }
        {
            u32 frame = advanceParticleFrame(emitter);
            frame &= 7;
            asm("" : "+r"(frame));
            if (frame == 0)
                sub_08051A8C(emitter, emitter->particleResource);
        }
    } else {
        u32 close = sub_08051B70(emitter);
        if (!close) {
            if (emitter->leaveScriptId)
                Script_Call(emitter->leaveScriptId, 0);
            TetherActor_RequestPaletteBlend(0);
            *nearby = close;
            Sound_PlaySong(0x199);
        }
        {
            u32 frame = advanceParticleFrame(emitter);
            frame &= 3;
            asm("" : "+r"(frame));
            if (frame == 0)
                sub_08051A8C(emitter, emitter->particleResource);
        }
    }
}
