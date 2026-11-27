#include "global.h"
#include "scene/collision/contact_actor.h"
#include "player/player.h"
#include "objects/moving_platforms/named_object_flags.h"
#include "objects/moving_platforms/named_object.h"
u32 Player_TestAttachmentFlags(struct Player *, u32);
void Player_SetPositionAndSave(struct Player *, struct ContactPosition *);
static inline void Delta(struct Player **player, struct ContactPosition *out,
                         struct ContactPosition *from)
{
    struct SceneVec *to = &(*player)->position;
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline void StopMovementSound(struct NamedObject *object)
{
    if (!(object->field_E0 & 4)) {
        Sound_StopSong(0x158);
        Sound_StopSong(0x261);
        Sound_StopSong(0x289);
        Sound_StopSong(0x28A);
        Sound_StopSong(0x288);
        Sound_PlaySong(0x159);
    }
}
// confidence=low: moves vertically to the selected height limit and snaps an attached player on arrival.
void MovingPlatform_UpdateVerticalTravel(struct NamedObject *object)
{
    struct ContactPosition delta;
    Memory_Clear(&object->motion, 8);
    Delta(gPlayers, &delta, &object->body.position);
    if (object->stage) {
        MovingPlatformActor_PlayMovementSound(object);
        object->stage = 0;
    }
    if (NamedObjectFlags_Has(4, &object->flags))
        return;
    if (NamedObjectFlags_Has(2, &object->flags)) {
        object->body.position.y += object->speed;
        object->motion.y = object->speed;
        if (object->upperHeight > object->body.position.y)
            return;
        object->body.position.y = object->upperHeight;
        if (Player_TestAttachmentFlags(gPlayers[0], 1)) {
            delta = *(const struct ContactPosition *)&gPlayers[0]->position;
            delta.y = object->body.position.y;
            Player_SetPositionAndSave(gPlayers[0], &delta);
            MovingPlatform_EnableFlags(0x100, object);
            object->motion.y = 0;
        }
        object->state = 4;
        object->stage = 1;
        StopMovementSound(object);
    } else {
        object->body.position.y -= object->speed;
        object->motion.y = -object->speed;
        if (object->lowerHeight < object->body.position.y)
            return;
        object->body.position.y = object->lowerHeight;
        if (Player_TestAttachmentFlags(gPlayers[0], 1)) {
            delta = *(const struct ContactPosition *)&gPlayers[0]->position;
            delta.y = object->body.position.y;
            Player_SetPositionAndSave(gPlayers[0], &delta);
            MovingPlatform_EnableFlags(0x100, object);
            object->motion.y = 0;
        }
        object->state = 4;
        object->stage = 1;
        StopMovementSound(object);
    }
}
