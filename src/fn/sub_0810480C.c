#include "global.h"
#include "ram.h"
#include "player.h"
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
u8 ProximityActor_IsPlayerInsideOriginRange(void *);
void ActorGroup_UpdateMemberSway(struct ActorGroupMember *);
void Sound_PlaySong(u32);
static inline void Clear(void *p)
{
    u32 zero = 0;
    CpuSet(&zero, p, 0x05000002);
}
static inline s32 ActorGroup_ProjectComponent(s32 x)
{
    s32 result;
    if (x >= 0)
        result = x >> 12;
    else
        result = -((-x) >> 12);
    return result;
}

static inline u32 ActorGroup_TestPlayerFlag(const struct Player *player, u8 mask)
{
    return mask & player->statusFlags;
}
// confidence=low: starts a sway and advances the group member when the nearby player moves.
void ActorGroup_UpdateMemberProximity(void *unused, struct ActorGroupMember *p)
{
    if (p->started) {
        s32 x;
        s32 z;
        p->frame = 0;
        p->flags = (p->flags & ~1) | 2;
        p->started = 0;
        Clear(&p->radiusX);
        p->radiusX = 41;
        p->radiusZ = 148;
        x = ActorGroup_ProjectComponent(p->radiusX * gUnk_086149C4[(p->angle + 64) & 255]) +
            Scale(p->radiusZ * gUnk_086149C4[p->angle]);
        z = ActorGroup_ProjectComponent(-(p->radiusX * gUnk_086149C4[p->angle])) +
            Scale(p->radiusZ * gUnk_086149C4[(p->angle + 64) & 255]);
        p->x = p->originX + x;
        p->z = z + p->originZ;
    }
    if (p->frame & 1) {
        if (ProximityActor_IsPlayerInsideOriginRange(p)) {
            if (!ActorGroup_TestPlayerFlag(gPlayers[0], 8)) {
                s32 active;
                if (gPlayerPassageState && !gPlayerInteractionMode)
                    active = 1;
                else
                    active = 0;
                if (!active && gPlayers[0]->actionMode != 3) {
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
