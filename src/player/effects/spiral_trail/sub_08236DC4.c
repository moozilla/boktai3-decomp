#include "player/effects/spiral_trail.h"

void *AnimData_FindEntry(s32);
void ParticleSprite_Initialize(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void Effect_SetPalette(void *, u32);

void sub_08236DC4(struct SpiralTrailTask *p)
{
    struct SpiralTrailEmitter *emitter = &p->emitter;
    s32 i;
    s32 frame;
    emitter->graphics = AnimData_FindEntry(0x1C1E);
    emitter->next = 0;
    emitter->delay = 0;
    if (p->kind == 7)
        frame = 14;
    else
        frame = (p->kind + 1) * 2;
    for (i = 0; i < 8; i++) {
        struct SpiralTrailParticle *slot = &emitter->particles[i];
        slot->active = 0;
        slot->frame = frame;
        ParticleSprite_Initialize(slot, emitter->graphics, 1);
        Effect_SetOrigin(slot, -4, -4);
        Effect_SetPalette(slot, 1);
        slot->attribute = 0xF8;
        slot->priority = 2;
    }
}
