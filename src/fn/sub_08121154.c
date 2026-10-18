#include "global.h"
#define gActorManager gUnk_0200020C
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
extern void *gUnk_0200020C;
extern u32 gUnk_030053F4, gUnk_030053F0;
void Anim_Remove(void *);
void sub_0811B394(void *);
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
            sub_0811B394(actor->contact);
            if (!((gUnk_030053F4 | gUnk_030053F0) & mask))
                SceneMessages_UnregisterReceiver(actor->drawable);
        }
    } while (++i <= 0);
}

static inline void ClearManager(u32 zero)
{
    gUnk_0200020C = (void *)zero;
}
u8 ActorManager_Destroy(struct ActorManager *manager)
{
    s32 i = 0;
    DestroyActors(manager, i, 0x800);
    ClearManager(0);
    return 0;
}
