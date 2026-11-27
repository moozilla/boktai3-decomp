#include "global.h"
/* Tentative ParticleEffect/ParticleBurst family names (low confidence). */
struct ParticleBurst {
    u8 pad[0x98];
    u8 spriteActive;
    u8 pad99[5];
    u8 slotsActive;
    u8 pad9F[0x1E5];
    u8 mode;
};
void sub_08236F48(struct ParticleBurst *);
void SpiralTrail_UpdateSlots(struct ParticleBurst *);
void Task_Kill(void *);
u32 SpiralTrail_Update(struct ParticleBurst *p)
{
    sub_08236F48(p);
    if (p->mode == 1)
        SpiralTrail_UpdateSlots(p);
    if (!p->spriteActive && !p->slotsActive)
        Task_Kill(p);
}
