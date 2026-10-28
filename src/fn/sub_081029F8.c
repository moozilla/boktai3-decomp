#include "global.h"
#include "player.h"
#include "ram.h"
struct NamedObjectVector {
    s16 x, y, z, pad;
};
struct NamedObject {
    u8 pad0[0x58];
    struct SceneVec offset;
    u8 pad60[0x58];
    s16 flags;
    u8 padBA[0x22];
    u32 name;
    u8 padE0[0x2C];
    struct SceneVec position;
};
struct NamedObjectManager {
    u8 pad0[0x1370];
    u32 playerName;
};
extern const struct NamedObjectVector gUnk_086064B4;
extern const struct NamedObjectVector gUnk_086064BC;
void NamedObject_ClearAttachmentFlag(u32, s32);
void NamedObject_UpdatePlayerFollowFlags(struct NamedObject *);
void Player_ClearAttachmentFlags(struct Player *, u32);
static inline void subtractVector(struct NamedObjectVector *d, struct NamedObjectVector *a,
                                  struct SceneVec *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}
static inline void addVector(struct NamedObjectVector *d, struct SceneVec *a, struct SceneVec *b)
{
    d->x = a->x + b->x;
    d->y = a->y + b->y;
    d->z = a->z + b->z;
}
#define IN_BOUNDS(b)                                                                               \
    (delta.x <= (b).x && delta.x >= -(b).x && delta.y <= (b).y && delta.y >= -(b).y &&             \
     delta.z <= (b).z && delta.z >= -(b).z)
static inline void clearFlag16(s32 mask, s16 *flags)
{
    *flags &= ~mask;
}
void NamedObject_UpdatePlayerAttachment(struct NamedObject *object)
{
    struct NamedObjectVector target, delta;
    struct NamedObjectManager *manager;
    struct Player *player = gPlayers[0];
    target.x = player->position.x + object->offset.x;
    target.y = player->position.y + object->offset.y;
    target.z = player->position.z + object->offset.z;
    subtractVector(&delta, &target, &object->position);
    manager = gNamedActorManager;
    if (delta.x <= gUnk_086064B4.x && delta.x >= -gUnk_086064B4.x && delta.y <= gUnk_086064B4.y &&
        delta.y >= -gUnk_086064B4.y && delta.z <= gUnk_086064B4.z && delta.z >= -gUnk_086064B4.z) {
        NamedObject_ClearAttachmentFlag(manager->playerName, 0);
        manager->playerName = object->name;
        NamedObject_UpdatePlayerFollowFlags(object);
        {
            s16 mask = 8;
            object->flags = mask | object->flags;
        }
        {
            u32 f;
            u32 v;
            u8 *p = (u8 *)gPlayers[0] + 0x701;
            f = *p;
            do {
                v = 1 | f;
            } while (0);
            *p = v;
        }
    } else if (manager->playerName == object->name) {
        if (*((u8 *)object + 0xF3) == 1 && !(*(u32 *)((u8 *)object + 0xE0) & 8)) {
            if (IN_BOUNDS(gUnk_086064BC)) {
                NamedObject_UpdatePlayerFollowFlags(object);
                return;
            }
        } else if (IN_BOUNDS(gUnk_086064B4)) {
            NamedObject_UpdatePlayerFollowFlags(object);
            return;
        }
        {
            manager->playerName = 0;
            *(u32 *)((u8 *)gPlayers[0] + 0x40) = (u32)gPlayers[0] + 0x284;
            Player_ClearAttachmentFlags(gPlayers[0], 5);
            clearFlag16(8, &object->flags);
            ((u8 *)gPlayers[0])[0x701] &= 0xFE;
        }
    }
}
