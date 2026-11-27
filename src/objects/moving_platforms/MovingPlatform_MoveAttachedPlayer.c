#define Player_SetAttachmentPosition Player_SetPositionAndSave
#include "global.h"
#include "player/player.h"
#include "engine/ram.h"
#include "objects/moving_platforms/named_object_flags.h"
#include "objects/moving_platforms/named_object.h"
void MovingPlatform_ResolveAttachmentPosition(struct SceneVec *, struct SceneVec *,
                                           struct NamedObject *);
void Player_SetPositionAndSave(struct Player *, struct SceneVec *);
void MovingPlatform_MoveAttachedPlayer(struct NamedObject *object)
{
    struct SceneVec target;
    struct NamedObjectManager *manager = gNamedActorManager;
    if (gPlayers[0] && object->name == manager->followName) {
        if (!NamedObjectFlags_Has(0x100, &object->flags)) {
            target.x = gPlayers[0]->position.x + object->motion.x;
            target.y = gPlayers[0]->position.y + object->motion.y;
            target.z = gPlayers[0]->position.z + object->motion.z;
            MovingPlatform_ResolveAttachmentPosition(&target, &gPlayers[0]->position, object);
            Player_SetPositionAndSave(gPlayers[0], &target);
        }
        {
            u32 mask = ~0x100;
            object->flags = mask & object->flags;
        }
    }
}
