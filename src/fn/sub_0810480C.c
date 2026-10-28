#include "global.h"
#include "ram.h"
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
        s32 z;
        p->frame = 0;
        p->flags = (p->flags & ~1) | 2;
        p->started = 0;
        Clear(&p->radiusX);
        p->radiusX = 41;
        p->radiusZ = 148;
        x = ScaleTemp(p->radiusX * gUnk_086149C4[(p->angle + 64) & 255]);
        x += Scale(p->radiusZ * gUnk_086149C4[p->angle]);
        z = ScaleTemp(-(p->radiusX * gUnk_086149C4[p->angle]));
        {
            s32 t = Scale(p->radiusZ * gUnk_086149C4[(p->angle + 64) & 255]);
            p->x = p->originX + x;
            p->z = (z + t) + p->originZ;
            asm("" : : "r"(t), "r"(z));
        }
    }
    if (p->frame & 1) {
        if (ProximityActor_IsPlayerInsideOriginRange(p)) {
            u32 m8 = 8;
            u8 *b8 = gUnk_02000580[0];
            u32 fl;
            asm("" : "+r"(m8));
            fl = *(u32 *)(b8 + 0x20);
            if (!(fl & m8)) {
                s32 active;
                if (gPlayerPassageState && !gPlayerInteractionMode)
                    active = 1;
                else
                    active = 0;
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
