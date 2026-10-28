#define gNamedObjectContactBounds gUnk_086064B4
#include "global.h"
#include "player.h"
struct NamedObjectContactBody {
    u8 unknown00[0x10C];
    struct SceneVec position;
};
extern const struct SceneVec gUnk_086064B4;
static inline void Difference(struct SceneVec *d, const struct SceneVec *b,
                              const struct SceneVec *a)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}
u32 NamedObject_IsPlayerWithinContactBounds(struct NamedObjectContactBody *object)
{
    struct Player *player = gPlayers[0];
    struct SceneVec delta;
    if (player) {
        Difference(&delta, &object->position, &player->position);
        if (delta.x <= gUnk_086064B4.x && delta.x >= -gUnk_086064B4.x &&
            delta.y <= gUnk_086064B4.y && delta.y >= -gUnk_086064B4.y &&
            delta.z <= gUnk_086064B4.z && delta.z >= -gUnk_086064B4.z)
            return 1;
    }
    return 0;
}
