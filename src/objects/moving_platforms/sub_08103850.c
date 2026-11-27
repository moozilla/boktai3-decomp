#define MovingPlatform_UpdateAttachments MovingPlatform_UpdateFollowers
#define gNamedObjectStateCallbacks    gUnk_086064CC
#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/engine_pause.h"
#include "objects/moving_platforms/named_object.h"
#include "player/player.h"
extern void *gUnk_086064CC[];
void MovingPlatform_RegisterFollowCells(struct NamedObject *);
void MovingPlatform_UpdateFollowers(struct NamedObject *);
void sub_0810103C(struct NamedObject *);
void MovingPlatform_ApplyVerticalImpulseAndJitter(struct NamedObject *);
void MovingPlatform_UpdateMovementSound(struct NamedObjectManager *);
static inline u8 isActive(u32 mask, u32 bit)
{
    if (mask & bit)
        return 1;
    return 0;
}
static inline u32 testMask(void)
{
    s16 mask = 4;
    return gPlayerSlotStatus[0] & mask;
}
u32 MovingPlatform_UpdatePool(struct NamedObjectManager *manager)
{
    struct NamedObject *objects = manager->slots;
    s32 i;
    if (!manager->used)
        return 0;
    {
        if (Engine_PauseBits(0x400) || testMask()) {
            Sound_StopSong(0x158);
            Sound_StopSong(0x261);
            Sound_StopSong(0x289);
            Sound_StopSong(0x28A);
            Sound_StopSong(0x288);
            return 0;
        }
    }
    if (!(gEnginePauseFlags & 8)) {
        for (i = 0; i < 12; i++) {
            u32 *active = &manager->used;
            if (isActive(*active, 1 << i)) {
                struct NamedObject *object = (struct NamedObject *)(i * 0x19C + (u32)objects);
                if (object->state != 6) {
                    Callback_InvokeActor(object, object->handlers[0]);
                    Callback_InvokeActor(object, gUnk_086064CC[object->state]);
                    Callback_InvokeActor(object, object->handlers[1]);
                    Callback_InvokeActor(object, object->handlers[2]);
                    MovingPlatform_RegisterFollowCells(object);
                    MovingPlatform_UpdateFollowers(object);
                    sub_0810103C(object);
                    object->sprite.position = object->body.position;
                    object->sprite.position.x -= 0x100;
                    MovingPlatform_ApplyVerticalImpulseAndJitter(object);
                }
            }
        }
        MovingPlatform_UpdateMovementSound(manager);
    }
    return 0;
}
