#include "global.h"
#include "ram.h"

struct CloneVec {
    s16 x, y, z, unk_06;
};
struct CloneCollider {
    u8 unk_00[6];
    u16 flags;
    u8 unk_08[0x44 - 8];
    u16 timer;
    u8 unk_46[0x54 - 0x46];
};
struct CloneParams {
    u16 unk_00, lifetime, unk_04;
    s8 direction;
    u8 unk_07;
    s32 speed;
    void *callback;
    u8 unk_10[0x10];
};
struct CloneAnim {
    u8 unk_00[0x20];
    struct CloneVec position;
    u8 unk_28[0x12];
    u16 paletteFlags;
    u8 unk_3C[0xc];
    void *palette;
    u8 unk_4C[0xc0 - 0x4c];
};
struct CloneState {
    u32 flags;
    u32 phase;
    u8 unk_30C[0x10];
    s32 animation;
    u8 foldedFacing, facing, mirrored, unk_323;
    s32 speed;
    u32 index;
    u8 unk_32C[4];
    s16 paletteTimer;
    u16 unk_332;
    s16 attackPending, attack, element, power;
    u32 debris;
    u16 blinkTimer, unk_342;
};
struct PlayerClone {
    u32 frame;
    u8 unk_04[8];
    struct CloneVec velocity;
    u8 unk_14[0x2c - 0x14];
    struct CloneCollider contact;
    struct CloneCollider body;
    struct CloneCollider area;
    struct CloneCollider playerAttack;
    u8 unk_17C[0x198 - 0x17c];
    struct CloneParams params;
    struct CloneAnim anim;
    u8 resource[0x20];
    u8 shadow[0x6c];
    struct CloneState state;
};
struct PlayerClonePool {
    u8 task[0x18];
    u32 activeMask;
    struct PlayerClone entries[3];
    s32 activeCount;
    s16 palette1[16];
    s16 palette2[16];
};
extern u32 gUnk_02000560;
extern const s16 gUnk_086149C4[];
s32 sub_08249240(void *, s32, void *);
void PlayerClone_ReleaseSlot(struct PlayerClonePool *, struct PlayerClone *, s32);
void DirectionalSprite_FoldFacing(struct PlayerClone *);
s32 PlayerClone_SetAnimation(struct PlayerClone *, s32, u32, u32);
void Collider_SetPosition(struct CloneCollider *, struct CloneVec *, s32);
void Collider_Refresh(struct CloneCollider *);
void Collider_SetAttackProperties(struct CloneCollider *, s32, s32, u32, u32, s32);
struct CloneVec *Player_GetEffectivePosition(s32);
void Anim_SelectPaletteBank(struct CloneAnim *, u32);

static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}
static inline void Vec_FromAngle(struct CloneVec *v, s32 angle, s32 speed)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * speed);
}
static inline void Vec_Sum(struct CloneVec *d, struct CloneVec *a, struct CloneVec *b)
{
    d->x = b->x + a->x;
    d->y = b->y + a->y;
    d->z = b->z + a->z;
}
static inline u32 IsPaused(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}
static inline u32 TestGlobal(u32 mask)
{
    return gUnk_02000560 & mask;
}
static inline void ClearFlags(u32 *flags, u32 mask)
{
    *flags &= ~mask;
}
static inline void SetFacing(struct PlayerClone *slot, u16 angle)
{
    s32 facing = (angle + 0x70) >> 5;
    u32 mask = 7;
    mask &= facing;
    slot->state.facing = mask;
}
static inline void UpdateVelocity(u32 *flags, struct CloneVec *velocity, struct CloneParams *params)
{
    if (*flags & 1) {
        Vec_FromAngle(velocity, params->direction, params->speed);
        ClearFlags(flags, 1);
    }
}
/* Advance each active clone, its animation and colliders, and its lifetime. */
#ifdef NONMATCHING
// fuzzy: 70.1%
// scope shape 86 (differences once low registers are set aside)
s32 PlayerClone_UpdatePool(struct PlayerClonePool *pool)
{
    struct PlayerClone *slot = pool->entries;
    s32 i = 0;
    struct CloneVec position;
    struct CloneVec delta;
    do {
        struct CloneCollider *area;
        u32 active = pool->activeMask & (1 << i);
        area = &slot->area;
        if (active) {
            struct CloneAnim *anim = &slot->anim;
            u8 *facing = &slot->state.facing;
            struct CloneParams *params = &slot->params;
            if (IsPaused(0x400))
                goto next;
            if (TestGlobal(4))
                goto next;
            slot->area.flags |= 4;
            if (sub_08249240(slot, i, params->callback) == 1) {
                PlayerClone_ReleaseSlot(pool, slot, i);
                goto next;
            }
            SetFacing(slot, (u16)params->direction);
            DirectionalSprite_FoldFacing(slot);
            {
                UpdateVelocity(&slot->state.flags, &slot->velocity, params);
                Vec_Sum(&anim->position, &anim->position, &slot->velocity);
            }
            if (anim->position.x > 0xcda)
                anim->position.x = 0xcda;
            else if (anim->position.x < 0x424)
                anim->position.x = 0x424;
            if (anim->position.z > 0xbd6)
                anim->position.z = 0xbd6;
            else if (anim->position.z < 0x528)
                anim->position.z = 0x528;
            if (slot->state.phase == 4)
                PlayerClone_SetAnimation(slot, slot->state.animation, 0x40, 2);
            else
                PlayerClone_SetAnimation(slot, slot->state.animation, 0x40, 1);
            {
                struct CloneCollider *collider = &slot->contact;
                struct CloneVec *pos = &slot->anim.position;
                Collider_SetPosition(collider, pos, 0);
                if (!(slot->body.flags & 4))
                    Collider_Refresh(collider);
                collider = &slot->body;
                Collider_SetPosition(collider, pos, 0);
            }
            if (!(slot->area.flags & 4)) {
                struct CloneCollider *collider = area;
                position = slot->anim.position;
                Vec_FromAngle(&delta, *facing, 0x80);
                Vec_Sum(&position, &position, &delta);
                Collider_SetPosition(collider, &position, 0);
                Collider_Refresh(collider);
            }
            if (slot->state.attackPending) {
                struct CloneCollider *collider;
                slot->state.attackPending = 0;
                collider = &slot->playerAttack;
                Collider_SetAttackProperties(collider, slot->state.attack, slot->state.element, 0,
                                             2, slot->state.power);
                Collider_SetPosition(collider, Player_GetEffectivePosition(0), 0);
                Collider_Refresh(collider);
            }
            if (slot->state.paletteTimer > 0) {
                slot->state.paletteTimer--;
                Anim_SelectPaletteBank(&slot->anim, 0x192);
                slot->anim.paletteFlags = 0xe013;
                slot->anim.palette = pool->palette1;
            } else {
                Anim_SelectPaletteBank(&slot->anim, 0x1f4);
                slot->anim.paletteFlags = 0xe000;
                slot->anim.palette = pool->palette2;
            }
            if (slot->frame >= (u32)(i * 30 + 0x2b2))
                PlayerClone_ReleaseSlot(pool, slot, i);
            if (slot->body.timer)
                slot->body.timer--;
            slot->frame++;
        }
    next:
        i++;
        slot++;
    } while (i <= 2);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", PlayerClone_UpdatePool);
#endif
