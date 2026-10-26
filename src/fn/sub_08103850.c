#define NamedObject_UpdateAttachments NamedObject_UpdateFollowers
#define gNamedObjectStateCallbacks    gUnk_086064CC
#include "global.h"
#include "engine.h"
struct NamedObjectPosition {
    u32 xy, zp;
};
struct NamedObject {
    u8 pad0[0x1C];
    struct NamedObjectPosition spritePosition;
    u8 pad24[0x92];
    s16 state;
    u8 padB8[0x2C];
    void *callbackE4, *callbackE8, *callbackEC;
    u8 padF0[0x1C];
    struct NamedObjectPosition position;
    u8 pad114[0x88];
};
struct NamedObjectManager {
    u8 pad0[0x1C];
    struct NamedObject objects[12];
    u32 activeMask;
};
extern u32 gUnk_030053F0;
extern s32 gUnk_02000560;
extern void *gUnk_086064CC[];
void NamedObject_RegisterFollowCells(struct NamedObject *);
void NamedObject_UpdateFollowers(struct NamedObject *);
void sub_0810103C(struct NamedObject *);
void NamedObject_ApplyVerticalImpulseAndJitter(struct NamedObject *);
void NamedObject_UpdateMovementSound(struct NamedObjectManager *);
static inline u8 isActive(u32 mask, u32 bit)
{
    if (mask & bit)
        return 1;
    return 0;
}
static inline u32 testMask(void)
{
    s16 mask = 4;
    return gUnk_02000560 & mask;
}
u32 NamedObject_UpdatePool(struct NamedObjectManager *manager)
{
    struct NamedObject *objects = manager->objects;
    s32 i;
    if (!manager->activeMask)
        return 0;
    {
        u32 mask = 0x400;
        s32 mask2 = 4;
        if ((mask & (gUnk_030053F4 | gUnk_030053F0)) || testMask()) {
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
            u32 *active = &manager->activeMask;
            if (isActive(*active, 1 << i)) {
                struct NamedObject *object = (struct NamedObject *)(i * 0x19C + (u32)objects);
                if (object->state != 6) {
                    Callback_InvokeActor(object, object->callbackE4);
                    Callback_InvokeActor(object, gUnk_086064CC[object->state]);
                    Callback_InvokeActor(object, object->callbackE8);
                    Callback_InvokeActor(object, object->callbackEC);
                    NamedObject_RegisterFollowCells(object);
                    NamedObject_UpdateFollowers(object);
                    sub_0810103C(object);
                    object->spritePosition = object->position;
                    *(u16 *)&object->spritePosition -= 0x100;
                    NamedObject_ApplyVerticalImpulseAndJitter(object);
                }
            }
        }
        NamedObject_UpdateMovementSound(manager);
    }
    return 0;
}
