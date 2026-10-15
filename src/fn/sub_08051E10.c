// MATCHING_CONCESSION: register-pinning
#include "proximity_emitter.h"
#define ProximityEmitter_InitializeParticles sub_08051E10
static inline void *resourceAt(void **resource)
{
    void **address = resource;
    return *address;
}
void ProximityEmitter_InitializeParticles(struct ProximityEmitter *input)
{
    register struct ProximityEmitter *emitter asm("r6") = input;
    void **resource;
    register s32 scale asm("r10");
    register u32 one asm("r9");
    s32 i;
    {
        void *found = AnimData_FindEntry(0x1C1E);
        void **address = &emitter->particleResource;
        *address = found;
        i = 0;
        resource = address;
    }
    {
        s32 value = -4;
        scale = value;
    }
    {
        u32 value = 1;
        one = value;
    }
    do {
        u32 offset = i * sizeof(struct ProximityEmitterParticle);
        u32 particleOffset = offset + 0xC0;
        register struct ProximityEmitterParticle *particle asm("r4");
        register u16 *active asm("r5");
        particle = (void *)((u8 *)emitter + particleOffset);
        sub_08217DE0(particle, resourceAt(resource), 0);
        sub_08217EC4(particle, scale, scale);
        sub_08217EEC(particle, resourceAt(resource), 2);
        sub_08217ECC(particle, 1);
        {
            register u32 value asm("r1") = one;
            asm("" : "+r"(value));
            ((u8 *)particle)[0xF] = value;
        }
        ((u8 *)particle)[0x10] = 20;
        particle->flags |= one;
        active = (u16 *)((u8 *)emitter + offset + 0xF0);
        {
            register u32 zero asm("r0") = 0;
            *active = zero;
        }
    } while (++i <= 7);
    {
        u8 *address = &emitter->nextParticle;
        u32 zero = 0;
        *address = zero;
        {
            register u16 *frameAddress asm("r0") = (u16 *)(address + 1);
            *frameAddress = zero;
        }
    }
}
