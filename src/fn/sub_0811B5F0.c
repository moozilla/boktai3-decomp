#include "global.h"
struct CollisionBody;
struct CollisionBodyLink {
    struct CollisionBody *body;
    void *previous;
    struct CollisionBodyLink *next;
};
struct CollisionGroup {
    struct CollisionBodyLink *pending, *targets;
    s32 unknown, count;
};
struct CollisionManager {
    u8 pad0[0x18];
    struct CollisionGroup groups[10];
};
void sub_0811B094(struct CollisionManager *, struct CollisionGroup *, s32);
void Collision_DispatchOverlaps(struct CollisionBody *, struct CollisionBodyLink *);
void Heap_Free(void *);
s32 Collision_ProcessGroups(struct CollisionManager *manager)
{
    s32 i;
    for (i = 0; i < 10; i++) {
        struct CollisionBodyLink *node;
        sub_0811B094(manager, &manager->groups[i], i);
        node = manager->groups[i].pending;
        while (node) {
            struct CollisionBodyLink *old;
            if (manager->groups[i].count > 0)
                Collision_DispatchOverlaps(node->body, manager->groups[i].targets);
            old = node;
            node = node->next;
            Heap_Free(old);
        }
        manager->groups[i].pending = 0;
    }
    return 0;
}
