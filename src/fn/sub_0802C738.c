#include "global.h"
#include "ram.h"
#include "player.h"
struct Object {
    u8 pad0[0x50];
    u32 flags;
    u8 pad54[0xc0];
    void (*update)(void *);
    void (*destroy)(void *);
    u8 pad11c[0x2b];
    u8 slot;
    u8 pad148[0xd0];
    struct Object *next, *prev;
};
struct Manager {
    u8 pad0[0x18];
    void *targets[4];
    struct Object *head;
    u32 ticks, slot, serial;
};
int ActorPool_UnlinkObject(struct Manager *, struct Object *);
void Heap_Free(void *);
void ActorPool_RefreshTargets(struct Manager *);
int ActorPool_UpdateCallback(struct Manager *m)
{
    struct Object *p = m->head;
    if (p) {
        u32 mask = 1;
        do {
            struct Object *next = p->prev;
            if (p->flags & mask) {
                if (p->destroy)
                    p->destroy(p);
                ActorPool_UnlinkObject(m, p);
                Heap_Free(p);
            } else if (p->update)
                p->update(p);
            p = next;
        } while (p);
    }
    m->ticks++;
    m->slot = m->ticks & 3;
    return 0;
}
