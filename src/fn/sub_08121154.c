#include "global.h"
#include "engine.h"
#include "ram.h"
struct ManagedActor {
    u8 unknown0[0x56C];
    u8 contact[0x8C];
    u8 animations[5][0x60];
    u8 unknown7D8[0x80];
    u8 drawable[0x5C];
    u32 active;
    u8 unknown8B8[0x50];
};
struct ActorManager {
    u8 task[0x18];
    struct ManagedActor actors[1];
};
void Anim_Remove(void *);
void SpatialIndex_RemoveEntry(void *);
void SceneMessages_UnregisterReceiver(void *);
static inline void DestroyActors(struct ActorManager *manager, s32 i, u32 mask)
{
    do {
        if (manager->actors[i].active == 1) {
            u8 *anim = (u8 *)manager + (i * sizeof(struct ManagedActor) + 0x610);
            s32 j = 4;
            struct ManagedActor *actor;
            do {
                Anim_Remove(anim);
                anim += 0x60;
            } while (--j >= 0);
            actor = &manager->actors[i];
            SpatialIndex_RemoveEntry(actor->contact);
            if (!((gEngineFlags | gScenePauseFlags) & mask))
                SceneMessages_UnregisterReceiver(actor->drawable);
        }
    } while (++i <= 0);
}

static inline void ClearManager(u32 zero)
{
    gActorManager = (void *)zero;
}
u8 ActorManager_Destroy(struct ActorManager *manager)
{
    s32 i = 0;
    DestroyActors(manager, i, 0x800);
    ClearManager(0);
    return 0;
}
