#include "global.h"
#include "player.h"

// confidence=low: requests player contact damage and adds the returned amount to boss health.

struct DrainPosition {
    s16 x, y, z, pad;
};
struct PlayerDrainTarget {
    u8 pad0[0x20];
    u32 flags;
    u8 pad24[0x418 - 0x24];
    u8 mode;
    u8 pad419[0x4F4 - 0x419];
    u8 frame;
};
struct DrainActor {
    u8 pad0[0x54];
    struct DrainPosition position;
    u8 pad5C[0x94 - 0x5C];
    s16 health;
    u8 pad96[6];
    u32 flags;
    u8 padA0[3];
    u8 action;
    u8 padA4[0xAF - 0xA4];
    u8 finished, entered, pendingAnimation, ready;
    u8 padB3[0x10C - 0xB3];
    struct PlayerDrainTarget *player;
    u8 pad110[0x18C - 0x110];
    u8 animation[0x60];
    u8 drawable[0x58A - 0x1EC];
    u16 damage;
    u8 pad58C[0x5B2 - 0x58C];
    s16 startFrame;
    u8 pad5B4[0x7B8 - 0x5B4];
    u8 collision[3][0x20];
};
extern u32 gUnk_02000560;
void Vanargand_SetPendingCue(void *, u16);
s32 Player_BeginPositionRequest(void *, void *, s32, s32, s32, s32, s32, s32, s32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void *Player_GetCollisionData(void);
void SceneActors_SetHealth(void *, s32);
void DamageNumberDisplay_ShowWithDefaultScale(void *, s32, s32, void *, s32);
void sub_081A6398(void *);
void Sound_PlaySongChecked(u32);
void CpuSet(const void *, void *, u32);

static inline u32 TestFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline void OffsetPosition(struct DrainPosition *out, struct DrainPosition *in)
{
    out->x = in->x + 128;
    out->y = in->y;
    out->z = in->z + 128;
}
static inline void SetPosition(struct DrainPosition *out, s32 x, s32 y, s32 z)
{
    out->x = x;
    out->y = y;
    out->z = z;
}
static inline void AddPosition(struct DrainPosition *out, struct DrainPosition *a,
                               struct DrainPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

void Vanargand_UpdatePlayerDrain(struct DrainActor *p)
{
    struct DrainActor *self = p;
    struct DrainPosition initial;
    struct DrainPosition position;
    struct DrainPosition *initialBase;
    {
        u32 mask = 12;
        p->flags |= mask;
    }
    Vanargand_SetPendingCue(p, 1);
    if (p->pendingAnimation) {
        u32 one;
        struct PlayerDrainTarget *target;
        p->pendingAnimation = 0;
        SetPosition(&initial, 128, 0, 128);
        initialBase = &initial;
        AddPosition(&initial, &p->position, initialBase);
        target = gPlayers[0];
        one = 1;
        if (!TestFlags(&target->flags, 0x40000)) {
            void *animation;
            void *drawable;
            Player_BeginPositionRequest(gPlayers[0], &initial, 0, 0, 0, 0, one, 0, 2);
            animation = p->animation;
            drawable = p->drawable;
            switch ((s32)p->player->mode) {
                case 0:
                    SceneActors_StartUnmirroredAnimation(p, animation, drawable, 12, one, 0);
                    CpuSet(Player_GetCollisionData(), p->collision[0], 0x4000008);
                    break;

                case 1:
                    SceneActors_StartUnmirroredAnimation(p, animation, drawable, 13, one, 0);
                    CpuSet(Player_GetCollisionData(), p->collision[1], 0x4000008);
                    break;

                case 2:
                    SceneActors_StartUnmirroredAnimation(p, animation, drawable, 14, one, 0);
                    CpuSet(Player_GetCollisionData(), p->collision[2], 0x4000008);
                    break;
            }

            self->startFrame = p->player->frame;
        } else {
            p->finished = one;
            return;
        }
    }
    {
        struct PlayerDrainTarget *player = p->player;
        s32 start = self->startFrame;
        s32 current = player->frame;
        s32 bias = -256;
        start += bias;
        if (((current - start) & 255) <= 19) {
            if (p->ready) {
                s32 amount;
                s32 health;
                OffsetPosition(&position, &p->position);
                if (!TestFlags(&p->player->flags, 0x40000)) {
                    amount = Player_BeginPositionRequest(p->player, &position, 0, 0, 0, 0, 1,
                                                         self->damage, 2);
                    health = self->health;
                    SceneActors_SetHealth(self, health + amount);
                    if (health != self->health)
                        DamageNumberDisplay_ShowWithDefaultScale(((u8 *)self) + 0x6C4, amount, 1,
                                                                 &self->position, 1);
                    sub_081A6398(self);
                    Sound_PlaySongChecked(0x225);
                    {
                        u32 mask = 4;
                        if ((!TestFlags(&gUnk_02000560, mask)) && (p->action <= 4))
                            return;
                    }
                }
                p->finished = 1;
            }
        } else {
            p->finished = 1;
        }
    }
}
