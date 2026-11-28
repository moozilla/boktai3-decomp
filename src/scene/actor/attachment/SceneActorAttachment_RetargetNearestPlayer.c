#include "scene/actor/scene_actor_status.h"
#include "scene/collision/contact_actor.h"
#include "player/player.h"
#include "scene/collision/scene_actor_contact_list.h"
#include "engine/ram.h"
void SceneActorAttachment_RetargetNearestPlayer(struct ChaseActor *actor,
                                                const struct ContactPosition *origin)
{
    u32 nearest = 0;
    s32 bestDistance = 0x7fffffff;
    /* Only the three named halfwords are read. Padding remains unspecified. */
    struct ContactPosition difference;
    s32 count;
    s32 i;

    count = gPlayerCount;
    for (i = 0; i < count; i++) {
        struct Player *player = gPlayers[i];

        if (player) {
            const struct SceneVec *position = &player->position;
            struct ContactPosition *delta = &difference;
            s32 distance;

            delta->x = player->position.x - origin->x;
            delta->y = position->y - origin->y;
            delta->z = position->z - origin->z;
            distance = delta->x * delta->x + delta->z * delta->z;
            if (bestDistance > distance) {
                bestDistance = distance;
                nearest = (u32)gPlayers[i] + 0x24;
            }
        }
    }
    if (nearest) {
        u32 mask = ROOM_ACTOR_MOTION_HOLDS_CONTACT;

        if (actor->base.motionFlags & mask) {
            SceneActorContacts_ReleaseNode(actor->group.contactNode);
            actor->base.motionFlags &= ~mask;
            actor->group.contactNode = 0;
        }
        actor->base.tracking.target = (void *)nearest;
    }
}
