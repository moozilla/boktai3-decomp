#include "scene_actor_contact_list.h"
#include "player.h"
#include "ram.h"
struct SceneActorBehaviorContactObject {
    u8 unknown0[12];
    struct ContactPosition position;
};
u8 SceneActorBehavior_IsNearParticipant(struct SceneActorRegistryActor *actor)
{
    struct ContactPackedVector difference;
    s32 index = 0;
    s32 last;
    s32 limit = gPlayerCount;
    struct Player **cursor;
    struct Player *player;
    struct SceneActorContactNode *node;
    s32 distance;
    if (index > limit)
        goto playersDone;
    last = limit;
    cursor = gPlayers;
    do {
        player = *cursor;
        if (player) {
            difference.x = (u16)actor->position.x - (u16)player->position.x;
            distance = difference.x;
            if (distance < 0)
                distance = -distance;
            if (distance <= 255) {
                difference.z = (u16)actor->position.z - (u16)player->position.z;
                distance = difference.z;
                if (distance < 0)
                    distance = -distance;
                if (distance <= 255)
                    return 1;
            }
        }
        cursor++;
        index++;
    } while (index <= last);
playersDone:
    node = SceneActorContacts_FindNodeByKind(4);
    if (node) {
        u16 *positionX = (void *)&actor->position.x;
        struct SceneActorBehaviorContactObject *object;
        object = node->object;
        difference.x = *positionX - (u16)object->position.x;
        distance = difference.x;
        if (distance < 0)
            distance = -distance;
        if (distance <= 255) {
            difference.z = (u16)actor->position.z - (u16)object->position.z;
            distance = difference.z;
            if (distance < 0)
                distance = -distance;
            if (distance <= 255)
                return 1;
        }
    }
    return 0;
}
