#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Particle {
    u8 pad[40];
    u8 sprite[40];
};
struct Effect {
    u8 active;
    u8 pad[7];
    struct Particle particles[4];
};
struct Actor {
    u8 pad[0x418];
    u8 mode;
};
struct Manager {
    u8 task[24];
    u32 mode, counter;
    void *graphics, *animation;
    struct Effect effects[6];
    struct Actor *actor;
};
extern struct Manager *gUnk_02000040;
extern struct Actor *gUnk_02000580;
void *sub_08215184(u32);
void *sub_0821A520(u32, u32);
void sub_08217E60(void *, void *, u32);
void sub_08217ECC(void *, u32);
s32 EffectManager_Init(struct Manager *manager)
{
    s32 i, j;
    u8 *p;
    gUnk_02000040 = manager;
    manager->graphics = sub_08215184(0x1c1a);
    manager->animation = sub_0821A520(0x922e, 0xd1b8);
    for (i = 0; i < 6; i++) {
        struct Effect *effect = &manager->effects[i];
        effect->active = 0;
        {
            register s32 next asm("r7") = i + 1;
        }
        p = effect->particles[0].sprite;
        for (j = 3; j >= 0; p += 80, j--) {
            sub_08217E60(p, manager->graphics, 1);
            sub_08217ECC(p, 1);
        }
    }
    manager->counter = 0;
    {
        struct Actor *actor = ({
            struct Actor *a = gUnk_02000580;
            a;
        });
        manager->actor = actor;
        if (!actor)
            return -1;
        if (actor->mode == 6)
            manager->mode = 1;
        else
            manager->mode = 0;
        return 0;
    }
}
