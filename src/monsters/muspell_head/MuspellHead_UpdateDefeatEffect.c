#include "monsters/muspell_head.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
struct V {
    u16 a;
    u16 b;
    u32 c;
};
void TrapSpellParticles_CreateDefaultBurst(void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                             u32, u32);
void MuspellHead_RecordDefeat(void *);
u32 MuspellHead_UpdateDefeatEffect(struct MuspellHead *p)
{
    if (p->initialDead == 0) {
        if (p->age <= 0x23) {
            struct V v;
            u16 *q = (u16 *)&v;
            u32 idx, t;
            v = *(struct V *)&p->sprite.position;
            q[1] += 0xc8;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            idx = gRandomIndex;
            t = *(u8 *)&gRandomTable[idx];
            TrapSpellParticles_CreateDefaultBurst(&v, 0x80, 0x20, 0xc, 0xc, 0x18, t, 0x20, 0x20,
                                                    1, 2, 0x200);
        }
        if (p->age == 1)
            MuspellHead_RecordDefeat(p);
    }
    return 0;
}
