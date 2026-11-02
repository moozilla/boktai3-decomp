#include "frost_emitter.h"

void sub_081D2EA4(struct FrostParticle *);
void sub_081D2D78(struct FrostEmitter *);

void FrostEmitter_Step(struct FrostEmitter *p)
{
    s32 idle = 0;
    s32 i;
    for (i = 0; i < 12; i++) {
        if (p->particles[i].lifetime != 0)
            sub_081D2EA4(&p->particles[i]);
        else
            idle++;
    }
    if (p->emitting == 0) {
        if (idle == 12) {
            p->runtime.enabled = 0;
            p->runtime.age = 0;
        }
    } else if ((p->runtime.age & 3) == 0)
        sub_081D2D78(p);
    p->runtime.age++;
}
