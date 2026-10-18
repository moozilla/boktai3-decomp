#include "global.h"
/* Tentative ParticleEffect/ParticleBurst family names (low confidence). */
#define ParticleBurst_Update      sub_082370AC
#define ParticleBurst_UpdateSlots sub_08236D58
struct ParticleBurst {
    u8 pad[0x98];
    u8 spriteActive;
    u8 pad99[5];
    u8 slotsActive;
    u8 pad9F[0x1E5];
    u8 mode;
};
void sub_08236F48(struct ParticleBurst *);
void sub_08236D58(struct ParticleBurst *);
void Task_Kill(void *);
u32 sub_082370AC(struct ParticleBurst *p)
{
    sub_08236F48(p);
    if (p->mode == 1)
        sub_08236D58(p);
    if (!p->spriteActive && !p->slotsActive)
        Task_Kill(p);
}
