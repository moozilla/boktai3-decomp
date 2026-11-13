#include "fragment_group.h"

static inline int Fragment_ScaleVelocity(int a, int b)
{
    int v = a * b;
    if (v >= 0)
        return v >> 12;
    else
        return -((-v) >> 12);
}

void FragmentGroup_Update(struct FragmentManager *manager, struct FragmentGroup *group)
{
    int i;
    group->count = 0;
    for (i = 0; i <= 5; i++) {
        struct DisplaySequenceSprite *sprite = &group->sprites[i];
        if (group->enabled[i]) {
            sprite->position.x += Fragment_ScaleVelocity(group->velocity[i].x, group->speed[i]);
            sprite->position.y += group->velocity[i].y;
            sprite->position.z += Fragment_ScaleVelocity(group->velocity[i].z, group->speed[i]);
            group->velocity[i].y -= 2;
            if (--group->lifetime[i] == 255) {
                group->enabled[i] = 0;
                Sprite_Remove(sprite);
            } else
                group->count++;
        }
    }
    if (!group->count)
        group->active = 0;
}

u32 FragmentManager_Update(struct FragmentManager *manager)
{
    struct FragmentGroup *group = manager->groups;
    s32 i = 3;
    do {
        if (group->active != 0)
            FragmentGroup_Update(manager, group);
        group++;
        i--;
    } while (i >= 0);
    manager->frame++;
    return 0;
}

u32 FragmentManager_Destroy(struct FragmentManager *manager)
{
    s32 i = 0;
    do {
        struct FragmentGroup *group = &manager->groups[i];
        u32 active = group->active;
        s32 next = i + 1;
        if (active != 0) {
            s32 j = 0;
            u8 *enabled = group->enabled;
            do {
                if (enabled[j] != 0)
                    Sprite_Remove(&group->sprites[j]);
                j++;
            } while (j <= 5);
        }
        i = next;
    } while (i <= 3);
    return gFragmentManager = 0;
}

s32 FragmentManager_Initialize(struct FragmentManager *manager, void *unused)
{
    s32 i = 0;
    do {
        struct FragmentGroup *group = &manager->groups[i];
        s32 next;
        s32 j;
        u8 *enabled;
        group->active = 0;
        next = i + 1;
        for (j = 5, enabled = &group->enabled[5]; j >= 0; j--)
            *enabled-- = 0;
        i = next;
    } while (i <= 3);
    manager->frame = 0;
    gFragmentManager = (u32)manager;
    return 0;
}

struct FragmentManager *FragmentManager_Create(void *argument, u32 unused)
{
    struct FragmentManager *manager = Task_Create(10, sizeof(*manager));
    if (manager != 0) {
        Task_SetCallbacks(manager, (void *)FragmentManager_Update, (void *)FragmentManager_Destroy);
        if (FragmentManager_Initialize(manager, argument) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

void AnimatedParticleManager_ClearSingleton(void)
{
    gAnimatedParticleManager = 0;
}
