#include "global.h"
#include "ram.h"
struct NamedObject {
    u8 pad0[0xB6];
    u16 state;
    u16 flags;
    u8 padBA[0xDC - 0xBA];
    u32 name;
};
struct NamedObjectManager {
    u8 pad0[0x1370];
    u32 followName;
};
struct Follower {
    u8 pad0[0x40];
    void *target;
    u8 pad44[0x284 - 0x44];
    u8 slot[4];
};
extern struct Follower *gUnk_02000580[];
void sub_0810103C(struct NamedObject *object)
{
    struct NamedObjectManager *manager = gNamedActorManager;
    s32 i;
    for (i = 0; i < 1; i++) {
        struct Follower *f = gUnk_02000580[i];
        if (f && object->name == manager->followName) {
            if ((u16)(object->state - 3) <= 1)
                f->target = f->slot;
            else
                f->target = NULL;
        }
    }
}
