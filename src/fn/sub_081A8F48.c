#include "vanargand_pair_attack.h"
#include "player.h"

// confidence=low: requests player contact damage and adds the returned amount to boss health.

extern u32 gUnk_02000560;
void Vanargand_SetPendingCue(void *, u16);
s32 Player_BeginPositionRequest(struct Player *, struct SceneVec *, struct SceneVec *, s32, s32,
                                s32, s32, s32, u32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
u32 Player_GetCollisionData(void);
void SceneActors_SetHealth(void *, s32);
void DamageNumberDisplay_ShowWithDefaultScale(void *, s32, s32, void *, s32);
void sub_081A6398(void *);
void Sound_PlaySongChecked(u32);
void CpuSet(const void *, void *, u32);

static inline u32 TestFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline void OffsetPosition(struct SceneVec *out, struct SceneVec *in)
{
    out->x = in->x + 128;
    out->y = in->y;
    out->z = in->z + 128;
}
static inline void SetPosition(struct SceneVec *out, s32 x, s32 y, s32 z)
{
    out->x = x;
    out->y = y;
    out->z = z;
}
static inline void AddPosition(struct SceneVec *out, struct SceneVec *a,
                               struct SceneVec *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

void Vanargand_UpdatePlayerDrain(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *self = p;
    struct SceneVec initial;
    struct SceneVec position;
    struct SceneVec *initialBase;
    {
        u32 mask = 12;
        p->flags |= mask;
    }
    Vanargand_SetPendingCue(p, 1);
    if (p->pending) {
        u32 one;
        struct Player *target;
        p->pending = 0;
        SetPosition(&initial, 128, 0, 128);
        initialBase = &initial;
        AddPosition(&initial, &p->position, initialBase);
        target = gPlayers[0];
        one = 1;
        if (!TestFlags(&target->statusFlags, 0x40000)) {
            void *animation;
            void *drawable;
            Player_BeginPositionRequest(gPlayers[0], &initial, 0, 0, 0, 0, one, 0, 2);
            animation = &p->animation;
            drawable = p->drawable;
            switch ((s32)p->player->actionMode) {
                case 0:
                    SceneActors_StartUnmirroredAnimation(p, animation, drawable, 12, one, 0);
                    CpuSet((const void *)Player_GetCollisionData(), p->palettes[5], 0x4000008);
                    break;

                case 1:
                    SceneActors_StartUnmirroredAnimation(p, animation, drawable, 13, one, 0);
                    CpuSet((const void *)Player_GetCollisionData(), p->palettes[6], 0x4000008);
                    break;

                case 2:
                    SceneActors_StartUnmirroredAnimation(p, animation, drawable, 14, one, 0);
                    CpuSet((const void *)Player_GetCollisionData(), p->palettes[7], 0x4000008);
                    break;
            }

            self->drainStartFrame = p->player->facingChangeEffects;
        } else {
            p->finished = one;
            return;
        }
    }
    {
        struct Player *player = p->player;
        s32 start = self->drainStartFrame;
        s32 current = player->facingChangeEffects;
        s32 bias = -256;
        start += bias;
        if (((current - start) & 255) <= 19) {
            if (p->animationEvent) {
                s32 amount;
                s32 health;
                OffsetPosition(&position, &p->position);
                if (!TestFlags(&p->player->statusFlags, 0x40000)) {
                    amount = Player_BeginPositionRequest(p->player, &position, 0, 0, 0, 0, 1,
                                                         self->drainDamage, 2);
                    health = self->health;
                    SceneActors_SetHealth(self, health + amount);
                    if (health != self->health)
                        DamageNumberDisplay_ShowWithDefaultScale(&self->damageSource, amount, 1,
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
