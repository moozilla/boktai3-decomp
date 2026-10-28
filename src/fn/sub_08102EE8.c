#define Player_SetAttachmentPosition Player_SetPositionAndSave
#include "global.h"
#include "player.h"
#include "ram.h"
struct NamedObject {
    u8 pad0[0x58];
    struct SceneVec offset;
    u8 pad60[0x58];
    u16 flags;
    u8 padBA[0x22];
    u32 name;
};
struct NamedObjectManager {
    u8 pad0[0x1370];
    u32 playerName;
};
void NamedObject_ResolveAttachmentPosition(struct SceneVec *, struct SceneVec *,
                                           struct NamedObject *);
void Player_SetPositionAndSave(struct Player *, struct SceneVec *);
static inline u8 testFlag(u32 mask, u16 *flags)
{
    if (*flags & mask)
        return 1;
    return 0;
}
void NamedObject_MoveAttachedPlayer(struct NamedObject *object)
{
    struct SceneVec target;
    struct NamedObjectManager *manager = gNamedActorManager;
    if (gPlayers[0] && object->name == manager->playerName) {
        if (!testFlag(0x100, &object->flags)) {
            target.x = gPlayers[0]->position.x + object->offset.x;
            target.y = gPlayers[0]->position.y + object->offset.y;
            target.z = gPlayers[0]->position.z + object->offset.z;
            NamedObject_ResolveAttachmentPosition(&target, &gPlayers[0]->position, object);
            Player_SetPositionAndSave(gPlayers[0], &target);
        }
        {
            u32 mask = ~0x100;
            object->flags = mask & object->flags;
        }
    }
}
