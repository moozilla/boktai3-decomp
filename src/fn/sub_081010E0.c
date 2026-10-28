#include "global.h"
#include "contact_actor.h"
#include "player.h"
struct NamedObjectLift {
    u8 pad0[0x58];
    struct ContactPosition motion;
    u8 pad60[0x56];
    u16 state, flags;
    u8 entered, padBB;
    u16 timer, speed;
    u8 padC0[0x14];
    s32 lowerHeight, upperHeight;
    u8 padDC[4];
    u32 soundFlags;
    u8 padE4[0x28];
    struct ContactPosition position;
};
struct LiftPlayer {
    u8 pad[0x30];
    struct ContactPosition position;
};
void NamedActor_PlayMovementSound(struct NamedObjectLift *);
u32 Player_TestAttachmentFlags(struct LiftPlayer *, u32);
void Player_SetPositionAndSave(struct LiftPlayer *, struct ContactPosition *);
static inline void Delta(struct LiftPlayer **player, struct ContactPosition *out,
                         struct ContactPosition *from)
{
    struct ContactPosition *to = &(*player)->position;
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline void Or(u32 mask, u16 *p)
{
    *p = mask | *p;
}
static inline u8 Test(u32 mask, u16 *flags)
{
    if (*flags & mask)
        return 1;
    return 0;
}
static inline void StopMovementSound(struct NamedObjectLift *object)
{
    if (!(object->soundFlags & 4)) {
        Sound_StopSong(0x158);
        Sound_StopSong(0x261);
        Sound_StopSong(0x289);
        Sound_StopSong(0x28A);
        Sound_StopSong(0x288);
        Sound_PlaySong(0x159);
    }
}
// confidence=low: moves vertically to the selected height limit and snaps an attached player on arrival.
void NamedObject_UpdateVerticalTravel(struct NamedObjectLift *object)
{
    struct ContactPosition delta;
    Memory_Clear(&object->motion, 8);
    Delta(&(*(struct LiftPlayer **)gPlayers), &delta, &object->position);
    if (object->entered) {
        NamedActor_PlayMovementSound(object);
        object->entered = 0;
    }
    if (Test(4, &object->flags))
        return;
    if (Test(2, &object->flags)) {
        object->position.y += object->speed;
        object->motion.y = object->speed;
        if (object->upperHeight > object->position.y)
            return;
        object->position.y = object->upperHeight;
        if (Player_TestAttachmentFlags((*(struct LiftPlayer **)gPlayers), 1)) {
            delta = (*(struct LiftPlayer **)gPlayers)->position;
            ((struct ContactPosition *)&delta)->y = object->position.y;
            Player_SetPositionAndSave((*(struct LiftPlayer **)gPlayers), &delta);
            Or(0x100, &object->flags);
            object->motion.y = 0;
        }
        object->state = 4;
        object->entered = 1;
        StopMovementSound(object);
    } else {
        object->position.y -= object->speed;
        object->motion.y = -object->speed;
        if (object->lowerHeight < object->position.y)
            return;
        object->position.y = object->lowerHeight;
        if (Player_TestAttachmentFlags((*(struct LiftPlayer **)gPlayers), 1)) {
            delta = (*(struct LiftPlayer **)gPlayers)->position;
            ((struct ContactPosition *)&delta)->y = object->position.y;
            Player_SetPositionAndSave((*(struct LiftPlayer **)gPlayers), &delta);
            Or(0x100, &object->flags);
            object->motion.y = 0;
        }
        object->state = 4;
        object->entered = 1;
        StopMovementSound(object);
    }
}
