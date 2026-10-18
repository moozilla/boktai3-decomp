#define gSineTable gUnk_086149C4
#include "global.h"
struct DirectionalActor {
    u8 pad0[0xCC];
    s16 x, y, z;
    u8 padD2[0x10];
    u8 angle;
};
struct ForwardPosition {
    s16 x, y, z;
};
extern const s16 gSineTable[];
static inline s32 scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
void DirectionalActor_GetForwardPosition(struct DirectionalActor *p, struct ForwardPosition *out)
{
    const s16 *table = gSineTable;
    u32 angle = p->angle;
    s32 cosine = table[(angle + 64) & 255];
    s32 sine = table[angle];
    out->x = p->x + scale(170 * cosine);
    out->y = p->y + 80;
    out->z = p->z + scale(170 * sine);
}
