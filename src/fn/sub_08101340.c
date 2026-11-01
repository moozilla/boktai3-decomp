#include "global.h"
#include "named_object.h"
#include "player.h"
static inline u8 Active(u32 mask, u32 bit)
{
    if (mask & bit)
        return 1;
    return 0;
}
static inline void Stop(void)
{
    Sound_StopSong(0x288);
    Sound_StopSong(0x28A);
    Sound_StopSong(0x289);
}
struct NamedObject *NamedObject_FindByName(u32);
static inline void Play(struct NamedObject *o)
{
    u32 song;
    switch (o->kind) {
        case 0:
            song = 0x28A;
            break;
        case 3:
            song = 0x288;
            break;
        default:
            song = 0x289;
            break;
    }
    Sound_PlaySong(song);
}
void NamedObject_UpdateMovementSound(struct NamedObjectManager *manager)
{
    u32 attached = 0;
    struct NamedObject *objects = manager->slots;
    s32 i;
    struct NamedObject *o;
    for (i = 0; i < 12; i++) {
        if (Active(*(u32 *)((u8 *)manager + 0x136C), 1 << i)) {
            o = (struct NamedObject *)(i * (s32)sizeof(*objects) + (u32)objects);
            if ((s16)o->state != 6 && (o->field_E0 & 0x10)) {
                if ((s16)o->state != 1) {
                    if (manager->movementSoundActive == 1) {
                        Stop();
                        manager->movementSoundActive = 0;
                    }
                } else {
                    if (!manager->movementSoundActive) {
                        Play(o);
                        manager->movementSoundActive = 1;
                    }
                }
                return;
            }
        }
    }
    if (Player_TestAttachmentFlags(gPlayers[0], 1))
        attached = 1;
    objects = NamedObject_FindByName(*(u16 *)((u8 *)gPlayers[0] + 0x476));
    if (!objects)
        return;
    if (objects->field_E0 & 4)
        return;
    if (attached) {
        if ((s16)objects->state != 1) {
            if (manager->movementSoundActive == 1) {
                Stop();
                manager->movementSoundActive = 0;
            }
        } else {
            if (!manager->movementSoundActive) {
                Play(objects);
                manager->movementSoundActive = 1;
            }
        }
    } else {
        if (manager->movementSoundActive == 1) {
            Stop();
            manager->movementSoundActive = attached;
        }
    }
}
