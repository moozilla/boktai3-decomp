#include "scene/collision/scene_actor_contact_list.h"
#include "player/player.h"
#include "engine/ram.h"
struct SceneActorBehaviorContactObject {
    u8 unknown0[12];
    struct ContactPosition position;
};
u8 SceneActorBehavior_IsNearParticipant(struct ChaseActor *actor)
{
    struct ContactPackedVector difference;
    s32 i;
    struct Player *player;
    struct SceneActorContactNode *node;
    for (i = 0; i <= gPlayerCount; i++) {
        player = gPlayers[i];
        if (player) {
            difference.x = (u16)actor->base.body.position.x - (u16)player->position.x;
            if (abs(difference.x) <= 255) {
                difference.z = (u16)actor->base.body.position.z - (u16)player->position.z;
                if (abs(difference.z) <= 255)
                    return 1;
            }
        }
    }
    node = SceneActorContacts_FindNodeByKind(4);
    if (node) {
        difference.x = (u16)actor->base.body.position.x -
                       (u16)((struct SceneActorBehaviorContactObject *)node->object)->position.x;
        if (abs(difference.x) <= 255) {
            difference.z =
                (u16)actor->base.body.position.z -
                (u16)((struct SceneActorBehaviorContactObject *)node->object)->position.z;
            if (abs(difference.z) <= 255)
                return 1;
        }
    }
    return 0;
}
