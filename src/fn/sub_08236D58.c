#include "global.h"
/* Tentative ParticleEffect/ParticleBurst family names (low confidence). */
struct ParticleBurstSlot {
    u8 pad[0x28];
    u8 active;
    u8 pad29[0xF];
    void *callback;
};
struct ParticleBurst {
    u8 pad[0x9E];
    u8 active;
    u8 pad9F;
    void *context;
    struct ParticleBurstSlot slots[8];
};
s32 sub_08249240(void *, void *, void *);
void ParticleBurst_UpdateSlots(struct ParticleBurst *p)
{
    if (p->active) {
        s32 count = 0;
        u8 *slot = (u8 *)&p->slots[0].active;
        struct ParticleBurstSlot *effect = p->slots;
        s32 i = 7;
        do {
            if (*slot) {
                sub_08249240(effect, p->context, *(void **)(slot + 0x10));
                count++;
            }
            slot += 0x3C;
            effect++;
            i--;
        } while (i >= 0);
        if (!count)
            p->active = count;
    }
}
