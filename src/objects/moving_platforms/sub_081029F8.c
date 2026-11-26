#include "global.h"
#include "player/player.h"
#include "engine/ram.h"
#include "objects/moving_platforms/named_object_flags.h"
#include "objects/moving_platforms/named_object.h"
extern const struct ContactPosition gUnk_086064B4;
extern const struct ContactPosition gUnk_086064BC;
void MovingPlatform_ClearAttachmentFlag(u32, s32);
void MovingPlatform_UpdatePlayerFollowFlags(struct NamedObject *);
void Player_ClearAttachmentFlags(struct Player *, u32);
static inline void subtractVector(struct ContactPosition *d, struct ContactPosition *a,
                                  struct SceneVec *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}
static inline void addVector(struct ContactPosition *d, struct SceneVec *a, struct SceneVec *b)
{
    d->x = a->x + b->x;
    d->y = a->y + b->y;
    d->z = a->z + b->z;
}
#define IN_BOUNDS(b)                                                                               \
    (delta.x <= (b).x && delta.x >= -(b).x && delta.y <= (b).y && delta.y >= -(b).y &&             \
     delta.z <= (b).z && delta.z >= -(b).z)
void MovingPlatform_UpdatePlayerAttachment(struct NamedObject *object)
{
    struct ContactPosition target, delta;
    struct NamedObjectManager *manager;
    struct Player *player = gPlayers[0];
    target.x = player->position.x + object->motion.x;
    target.y = player->position.y + object->motion.y;
    target.z = player->position.z + object->motion.z;
    subtractVector(&delta, &target, &object->body.position);
    manager = gNamedActorManager;
    if (delta.x <= gUnk_086064B4.x && delta.x >= -gUnk_086064B4.x && delta.y <= gUnk_086064B4.y &&
        delta.y >= -gUnk_086064B4.y && delta.z <= gUnk_086064B4.z && delta.z >= -gUnk_086064B4.z) {
        MovingPlatform_ClearAttachmentFlag(manager->followName, 0);
        manager->followName = object->name;
        MovingPlatform_UpdatePlayerFollowFlags(object);
        MovingPlatform_EnableFlags(8, object);
        gPlayers[0]->shadow[1] |= 1;
    } else if (manager->followName == object->name) {
        if (object->kind == 1 && !(object->field_E0 & 8)) {
            if (IN_BOUNDS(gUnk_086064BC)) {
                MovingPlatform_UpdatePlayerFollowFlags(object);
                return;
            }
        } else if (IN_BOUNDS(gUnk_086064B4)) {
            MovingPlatform_UpdatePlayerFollowFlags(object);
            return;
        }
        {
            manager->followName = 0;
            gPlayers[0]->followTarget = &gPlayers[0]->movementContactFlags;
            Player_ClearAttachmentFlags(gPlayers[0], 5);
            MovingPlatform_DisableFlags(8, object);
            gPlayers[0]->shadow[1] &= 0xFE;
        }
    }
}
