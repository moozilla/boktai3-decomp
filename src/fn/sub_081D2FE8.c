#include "global.h"

#define FrostEmitter_Step sub_081D2FE8

struct FrostParticle {
    u8 pad0[0x44];
    u16 active;
    u8 pad46[2];
};
struct FrostEmitter {
    u8 pad0[0x3c];
    u32 f3c;
    struct FrostParticle particles[12]; /* at 0x40 */
    u8 pad[0x3a0 - 0x40 - 12 * 0x48];
    u32 f3a0;
    u32 f3a4;
};

void sub_081D2EA4(struct FrostParticle *);
void sub_081D2D78(struct FrostEmitter *);

void FrostEmitter_Step(struct FrostEmitter *p)
{
    s32 idle = 0;
    s32 i;
    for (i = 0; i < 12; i++) {
        if (p->particles[i].active != 0)
            sub_081D2EA4(&p->particles[i]);
        else
            idle++;
    }
    if (p->f3c == 0) {
        if (idle == 12) {
            p->f3a0 = 0;
            p->f3a4 = 0;
        }
    } else if ((p->f3a4 & 3) == 0)
        sub_081D2D78(p);
    p->f3a4++;
}
