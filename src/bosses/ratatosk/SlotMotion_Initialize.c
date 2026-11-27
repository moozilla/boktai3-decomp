#include "bosses/ratatosk/particle_config_layout.h"
#define SlotMotion_SetCallback Ratatosk_SetParticleUpdateCallback
#include "global.h"
void sub_081FB808(void);
void Memory_Clear(void *, u32);
void Ratatosk_SetParticleUpdateCallback(struct ParticleConfig *, u32);
void SlotMotion_Initialize(struct ParticleConfig *motion)
{
    Memory_Clear(motion, 0x18);
    Ratatosk_SetParticleUpdateCallback(motion, (u32)sub_081FB808);
    motion->duration = 0x14;
    motion->mode = 5;
    motion->angle = 0;
    motion->kind = 0;
    motion->scale = 0x1000;
}
