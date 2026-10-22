#include "global.h"
#ifdef NONMATCHING
// fuzzy: 91.9%
// Register allocation still differs in a few places.
struct ActorGroupMember {
    u32 flags;
    u8 pad4[2];
    u8 angle;
    u8 pad7[0x15];
    s16 x, y, z, pad22;
    u8 pad24[0xF4];
    s16 originX, originY, originZ, pad11E;
    s16 turnStep;
    u8 pad122[6];
    s16 radiusX;
    u8 pad12A[2];
    s16 radiusZ;
};
extern const s16 gUnk_086149C4[];
static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
static inline s32 Half(s32 x)
{
    if (x >= 0)
        return x >> 1;
    return -((-x) >> 1);
}
static inline s32 WrapAngle(s32 x)
{
    if (x > 128)
        x -= 256;
    else if (x < -128)
        x += 256;
    return x;
}
// confidence=low: rotates the member's offset about its origin and damps its turn step.
void ActorGroup_UpdateMemberSway(struct ActorGroupMember *p)
{
    s32 angle = WrapAngle(p->angle);
    s32 x;
    s32 z;

    if (angle == 0 && p->turnStep == 0)
        return;
    x = Scale(p->radiusX * gUnk_086149C4[(p->angle + 64) & 255]) +
        Scale(p->radiusZ * gUnk_086149C4[p->angle]);
    z = Scale(-(p->radiusX * gUnk_086149C4[p->angle])) +
        Scale(p->radiusZ * gUnk_086149C4[(p->angle + 64) & 255]);
    p->x = p->originX + x;
    p->z = z + p->originZ;
    if (p->turnStep == 0) {
        if (angle > 0)
            p->angle--;
        else
            p->angle++;
    } else {
        p->angle += p->turnStep;
        if (p->turnStep > 0) {
            if (angle > p->turnStep)
                p->turnStep = -p->turnStep;
        } else {
            if (angle < p->turnStep) {
                p->turnStep = -p->turnStep;
                p->turnStep = Half(p->turnStep);
            }
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", ActorGroup_UpdateMemberSway);
#endif
