#include "global.h"
#include "contact_swarm.h"
struct RegionSprite {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x90 - 0x2C];
    struct DisplaySequenceState sequence;
    struct ContactPosition origin;
    u8 unknownA8[0xEB - 0xA8];
    u8 hit;
    u8 unknownEC[0xFC - 0xEC];
    s16 health, maxHealth;
    s32 threshold;
    s16 resourceId, kind;
    u32 behaviorFlags;
    u8 entered, mode;
    u16 frames;
    u8 overlayActive, variant, unknown112[2];
    s32 region;
    u8 terrainCell[0x10];
};
void TerrainModifiers_UnregisterCell(void *);
static inline u8 TestV(u32 flags, u32 bit)
{
    if (flags & bit)
        return 1;
    return 0;
}
// confidence=low: in kinds 3-4, releases the registered terrain cell once (behavior flag 0x20).
void sub_0810638C(struct RegionSprite *p)
{
    if ((u16)(p->kind - 3) <= 1) {
        u32 m = 0x20;
        u32 *fp = &p->behaviorFlags;
        u32 v = *fp & m;
        u32 *flags = fp;
        if (TestV(v, ~0)) {
            TerrainModifiers_UnregisterCell(p->terrainCell);
            {
                u32 c = ~0x20;
                *flags &= c;
            }
        }
    }
}
