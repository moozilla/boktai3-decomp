#include "global.h"
#include "contact_actor.h"

extern const s16 gUnk_086149C4[];
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);

struct AxeDirection {
    u8 unk_00[6];
    u8 angle;
};
struct AxeCollisionContext {
    u8 unk_00[0xcec];
    struct ContactCollider attack[2];
    struct ContactCollider body[2];
    u8 unk_E3C[0x10];
    u32 flags;
};
struct AxeCollisionActor {
    void *object;
    struct AxeDirection *drawable;
    u8 unk_08[0x54];
    struct ContactPosition position;
    u8 unk_64[0xd0];
    struct ContactCollider body;
    struct ContactCollider attack;
    u8 unk_1DC[0xe4];
    u16 flags;
    u8 unk_2C2[0x10e];
    struct AxeCollisionContext *context;
};
static inline u8 HasContextFlag(struct AxeCollisionContext *context, u32 mask)
{
    if (context->flags & mask)
        return 1;
    return 0;
}
static inline u8 HasActorFlag(struct AxeCollisionActor *actor, u32 mask)
{
    if (actor->flags & mask)
        return 1;
    return 0;
}
static inline void SetCollider(struct ContactCollider *collider, u32 mask)
{
    collider->flags |= mask;
}
static inline void ClearCollider(struct ContactCollider *collider, u32 mask)
{
    collider->flags &= ~mask;
}
static inline s32 Scale(s32 value)
{
    s32 result;
    if (value >= 0)
        result = value >> 12;
    else
        result = -(-value >> 12);
    return result;
}
static inline void ClearOffset(u32 control, struct ContactPosition *dest, u32 *zero)
{
    CpuSet(zero, dest, control);
}
void Axe_UpdateBodyColliders(struct AxeCollisionActor *actor)
{
    struct AxeCollisionContext *context = actor->context;
    struct AxeDirection *drawable = actor->drawable;
    s32 index = 0;
    struct ContactCollider *attack = &actor->attack;
    struct {
        u32 zero;
        u32 values[0];
    } buf;
    struct ContactPosition offset;
    union {
        struct ContactPosition p;
        struct ContactPackedVector v;
    } position;
    s32 i, x, z;

    for (index = 0; index < 3; index++) {
        if (HasContextFlag(context, 0x40) || HasActorFlag(actor, 0x800)) {
            SetCollider(attack, 4);
            SetCollider(&actor->body, 4);
            for (i = 0; i < 2; i++) {
                SetCollider(&context->body[i], 4);
                SetCollider(&context->attack[i], 4);
            }
            break;
        }
        ClearCollider(attack, 4);
        ClearCollider(&actor->body, 4);
        for (i = 0; i < 2; i++) {
            ClearCollider(&context->body[i], 4);
            ClearCollider(&context->attack[i], 4);
        }
        buf.zero = 0;
        ClearOffset(0x05000002, &offset, &buf.zero);
        position.p = actor->position;
        switch (index) {
            case 0:
                offset.x = -120;
                offset.z = -120;
                break;
            case 1:
                offset.x = -30;
                offset.z = -30;
                break;
            case 2:
                offset.x = 60;
                offset.z = 60;
                break;
        }
        x = Scale(offset.x * gUnk_086149C4[(drawable->angle + 64) & 255]);
        z = Scale(offset.z * gUnk_086149C4[drawable->angle]);
        position.v.x = x + z + position.p.x;
        x = Scale(-(offset.x * gUnk_086149C4[drawable->angle]));
        z = Scale(offset.z * gUnk_086149C4[(drawable->angle + 64) & 255]);
        position.v.z = x + z + position.p.z;
        if (index == 0) {
            Collider_SetPosition(&actor->body, &position, 0);
            Collider_SetPosition(attack, &position, 0);
            Collider_Refresh(attack);
        } else {
            Collider_SetPosition(&context->body[index - 1], &position, 0);
            Collider_SetPosition(&context->attack[index - 1], &position, 0);
            Collider_Refresh(&context->attack[index - 1]);
        }
    }
}
