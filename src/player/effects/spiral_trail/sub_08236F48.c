#include "global.h"

struct BurstVector {
    s16 x, y, z, pad;
};
struct BurstParent {
    u8 pad0[0x30];
    struct BurstVector position;
    u8 pad38[0x457 - 0x38];
    u8 variant;
};
struct Burst {
    u8 pad0[0x18];
    struct BurstParent *parent;
    struct BurstVector offset;
    u8 kind, variant;
    u16 timer;
    u16 limit28, limit2A;
    u8 sprite[0x1C];
    struct BurstVector position;
    u8 pad50[0x98 - 0x50];
    u8 active;
    u8 pad99[0x284 - 0x99];
    u8 mode;
};
void SpiralTrail_AdvanceFrame(void *);
void SpiralTrail_RewindFrame(void *);
void SpiralTrail_EmitOrbitParticle(struct Burst *, struct BurstVector *);

void sub_08236F48(struct Burst *p)
{
    if (p->active) {
        u32 limit = p->limit2A;
        u32 frame = p->timer;
        u16 storedLimit = limit;
        if (frame < limit && p->variant != p->parent->variant)
            p->timer = storedLimit;
        frame = ++p->timer;
        if ((frame << 16) < ((u32)storedLimit << 16)) {
            SpiralTrail_AdvanceFrame(p->sprite);
            p->position = p->parent->position;
            p->position.x += p->offset.x;
            p->position.y += p->offset.y;
            p->position.z += p->offset.z;
            if (p->mode == 1 && p->timer < p->limit28)
                SpiralTrail_EmitOrbitParticle(p, &p->position);
        } else {
            SpiralTrail_RewindFrame(p->sprite);
        }
    }
}
