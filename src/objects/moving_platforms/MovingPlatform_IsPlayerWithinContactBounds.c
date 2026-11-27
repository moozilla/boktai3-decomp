#define gNamedObjectContactBounds gUnk_086064B4
#include "objects/moving_platforms/named_object.h"
#include "player/player.h"
extern const struct SceneVec gUnk_086064B4;
static inline void Difference(struct SceneVec *d, const struct ContactPosition *b,
                              const struct SceneVec *a)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}
u8 MovingPlatform_IsPlayerWithinContactBounds(struct NamedObject *object)
{
    struct Player *player = gPlayers[0];
    struct SceneVec delta;
    if (player) {
        Difference(&delta, &object->body.position, &player->position);
        if (delta.x <= gUnk_086064B4.x && delta.x >= -gUnk_086064B4.x &&
            delta.y <= gUnk_086064B4.y && delta.y >= -gUnk_086064B4.y &&
            delta.z <= gUnk_086064B4.z && delta.z >= -gUnk_086064B4.z)
            return 1;
    }
    return 0;
}
