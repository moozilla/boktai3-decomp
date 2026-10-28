#define gEffectRandomIndex  gRandomIndex
#define gEffectRandomValues gUnk_0203B400
// Orange spine family and descriptive names are tentative (low confidence).
#include "global.h"
#include "ram.h"
struct OrangeSpinePosition {
    s16 x, y, z, pad;
};
struct OrangeSpine {
    u8 unk0[0x18];
    u32 state;
    u8 unk1c[0x40 - 0x1c];
    struct OrangeSpinePosition position;
    u8 unk48[0x14a - 0x48];
    s16 defeatDelay;
};
extern u16 gUnk_0203B400[];
void SpriteParticle_CreateDefaultVariant4384(void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                             u32, u32);
void OrangeSpine_RecordDefeat(void *);
u32 OrangeSpine_UpdateDefeatEffect(struct OrangeSpine *p)
{
    if (p->defeatDelay == 0) {
        if ((u32)(p->state - 13) <= 10) {
            struct OrangeSpinePosition position;
            struct OrangeSpinePosition *pos = &position;
            u32 random;
            *pos = p->position;
            pos->y += 128;
            gEffectRandomIndex = (gEffectRandomIndex + 1) & 0x3ff;
            random = *(u8 *)&gEffectRandomValues[gEffectRandomIndex];
            SpriteParticle_CreateDefaultVariant4384(&position, 128, 32, 12, 12, 24, random, 32, 32,
                                                    1, 2, 512);
        }
        if (p->state == 1)
            OrangeSpine_RecordDefeat(p);
    }
    return 0;
}
