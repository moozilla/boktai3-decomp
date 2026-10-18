#define NamedObject_MoveAttachedPlayer        sub_08102EE8
#define NamedObject_ResolveAttachmentPosition sub_08102D20
#define Player_SetAttachmentPosition Player_SetPositionAndSave
#define gNamedObjectManager                   gUnk_020001AC
#include "global.h"
#include "player.h"
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
extern struct NamedObjectManager *gUnk_020001AC;
void sub_08102D20(struct SceneVec *, struct SceneVec *, struct NamedObject *);
void Player_SetPositionAndSave(struct Player *, struct SceneVec *);
static inline u8 testFlag(u32 mask, u16 *flags)
{
    if (*flags & mask)
        return 1;
    return 0;
}
void sub_08102EE8(struct NamedObject *object)
{
    struct SceneVec target;
    struct NamedObjectManager *manager = gUnk_020001AC;
    if (gUnk_02000580[0] && object->name == manager->playerName) {
        if (!testFlag(0x100, &object->flags)) {
            target.x = gUnk_02000580[0]->position.x + object->offset.x;
            target.y = gUnk_02000580[0]->position.y + object->offset.y;
            target.z = gUnk_02000580[0]->position.z + object->offset.z;
            sub_08102D20(&target, &gUnk_02000580[0]->position, object);
            Player_SetPositionAndSave(gUnk_02000580[0], &target);
        }
        {
            u32 mask = ~0x100;
            object->flags = mask & object->flags;
        }
    }
}
