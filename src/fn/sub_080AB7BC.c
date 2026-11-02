#include "global.h"
#include "player.h"
#include "contact_position.h"
#include "contact_collider.h"

// confidence=low: inferred from segment state and collider updates.

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetPosition(void *, void *, s32);
void Collider_Refresh(void *);

struct TentacleActor {
    u8 pad0[0x9c];
    s16 health;
    u8 pad1[0x2d0 - 0x9e];
    u32 flags2D0;
    u16 motionFlags;
    u8 pad2[0x3d0 - 0x2d6];
    u8 *context;
};

static inline u32 HasMotionFlag(struct TentacleActor *a, u32 m)
{
    return a->motionFlags & m;
}
static inline u32 HasStatusFlag(u32 m, u32 *a)
{
    return *a & m;
}

static inline u8 SegmentFlagSet(u32 m, u8 *a)
{
    if (*a & m)
        return TRUE;
    return FALSE;
}

static inline u8 ActorUnavailable(struct TentacleActor *p)
{
    if (p != NULL) {
        if (HasMotionFlag(p, 0x40) || HasStatusFlag(0x20000, &p->flags2D0))
            return TRUE;
        if (p->health > 0)
            return FALSE;
        return TRUE;
    }
    return FALSE;
}

static inline void SetAttack(void *collider, u8 *p, s32 variant)
{
    u8 *q = p + 0xb4 + variant;
    u32 a = *(u16 *)q;
    u32 b;
    u32 c;
    u32 d;
    q += 8;
    b = *(u16 *)q;
    q += variant ? 0x12 : 0x10;
    c = *(u32 *)q;
    q -= variant ? 0xa : 8;
    d = *(u16 *)q;
    Collider_SetAttackProperties(collider, a, b, 0, c, d);
}

void TentacleActor_UpdateSegmentColliders(struct TentacleActor *actor)
{
    u8 *p = (u8 *)actor;
    u8 *trail = actor->context;
    s32 index;
    struct ContactPosition delta;
    struct ContactPosition *distance;
    u8 *position, *attackCollider, *contactCollider, *flags;
    s32 offset;

    if (ActorUnavailable(actor))
        return;
    index = 0;
    if (index >= *(s16 *)(trail + 0x8da))
        return;
    distance = &delta;
    attackCollider = trail + 0x41c;
    position = trail + 0x3f0;
    contactCollider = trail + 0x470;
    offset = 0;
    flags = trail + 0x4d9;
    do {
        if (!SegmentFlagSet(1, flags)) {
            struct Player *player;
            const struct ContactPosition *origin;
            const struct SceneVec *source;
            s32 xSquare, zSquare, range;
            Collider_SetPosition(contactCollider, position, 0);
            Collider_Refresh(contactCollider);
            player = gPlayers[0];
            origin = (void *)position;
            source = &player->position;
            distance->x = (u16)source->x - (u16)origin->x;
            distance->y = (u16)source->y - (u16)origin->y;
            distance->z = (u16)source->z - (u16)origin->z;
            xSquare = distance->x * distance->x;
            zSquare = distance->z * distance->z;
            range = xSquare + zSquare;
            if (range <= 0xffff) {
                Collider_SetPosition(attackCollider, position, 0);
                Collider_Refresh(attackCollider);
                if (HasMotionFlag((struct TentacleActor *)p, 0x100)) {
                    {
                        u32 mask = 0x100;
                        ((struct ContactCollider *)(position + 0x2C))->flags &= ~mask;
                    }
                    SetAttack(attackCollider, p, 2);
                } else {
                    {
                        struct ContactCollider *f = (void *)(trail + offset + 0x41c);
                        f->flags |= 0x100;
                    }
                    SetAttack(attackCollider, p, 0);
                }
            }
        }
        attackCollider += 0x128;
        position += 0x128;
        contactCollider += 0x128;
        offset += 0x128;
        flags += 0x128;
        index++;
    } while (index < *(s16 *)(trail + 0x8da));
}
