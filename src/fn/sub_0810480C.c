#include "global.h"
#ifdef NONMATCHING
// fuzzy: 94.5%
// Register allocation and a few load/order choices still differ.
// MATCHING_CONCESSION: register-pinning
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
    u8 pad12E[2];
    s32 previousDistanceSquared, distanceSquared;
    u8 pad138[0x12];
    s16 health;
    u8 pad14C[6];
    u16 state;
    u8 pad154[8];
    u16 frame;
    u8 started;
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
static inline s32 ScaleTemp(s32 x)
{
    register s32 result asm("r0") = Scale(x);
    asm("" : : "r"(result));
    return result;
}
static inline s32 WrapAngle(s32 x)
{
    if (x > 128)
        x -= 256;
    else if (x < -128)
        x += 256;
    return x;
}
extern u8 *gUnk_02000580[];
extern u16 gUnk_02000534, gUnk_02000500;
u8 ProximityActor_IsPlayerInsideOriginRange(void *);
void ActorGroup_UpdateMemberSway(struct ActorGroupMember *);
void Sound_PlaySong(u32);
static inline void Clear(void *p)
{
    u32 zero = 0;
    CpuSet(&zero, p, 0x05000002);
}
// confidence=low: starts a sway and advances the group member when the nearby player moves.
void ActorGroup_UpdateMemberProximity(void *unused, struct ActorGroupMember *p)
{
    if (p->started) {
        register s32 x asm("r6");
        register s32 z asm("r3");
        p->frame = 0;
        p->flags = (p->flags & ~1) | 2;
        p->started = 0;
        Clear(&p->radiusX);
        p->radiusX = 41;
        p->radiusZ = 148;
        x = ScaleTemp(p->radiusX * gUnk_086149C4[(p->angle + 64) & 255]);
        asm("" : : "r"(x));
        x += Scale(p->radiusZ * gUnk_086149C4[p->angle]);
        asm("" : : "r"(x));
        z = ScaleTemp(-(p->radiusX * gUnk_086149C4[p->angle]));
        asm("" : : "r"(z));
        z += Scale(p->radiusZ * gUnk_086149C4[(p->angle + 64) & 255]);
        p->x = p->originX + x;
        p->z = z + p->originZ;
    }
    if (p->frame & 1) {
        if (ProximityActor_IsPlayerInsideOriginRange(p)) {
            if (!(*(u32 *)(gUnk_02000580[0] + 0x20) & 8)) {
                s32 active = 0;
                if (gUnk_02000534 && !gUnk_02000500)
                    active = 1;
                if (!active && gUnk_02000580[0][0x418] != 3) {
                    if (p->previousDistanceSquared != p->distanceSquared && p->turnStep == 0) {
                        p->turnStep = 4;
                        p->health--;
                        if (p->health <= 0) {
                            p->state = 3;
                            p->started = 1;
                            return;
                        }
                        Sound_PlaySong(0x3C9);
                    }
                }
            }
        }
    }
    ActorGroup_UpdateMemberSway(p);
    p->frame++;
}

#else
INCLUDE_ASM("asm/nonmatching", ActorGroup_UpdateMemberProximity);
#endif
