#include "global.h"
#include "ram.h"
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
extern u8 *gUnk_02000580[4];
int ActorPool_UnlinkObject(struct Manager *, struct Object *);
void Heap_Free(void *);
void ActorPool_RefreshTargets(struct Manager *);
int ActorPool_LinkObject(struct Manager *m, struct Object *p)
{
    if (m->head)
        m->head->next = p;
    p->next = 0;
    p->prev = m->head;
    m->head = p;
    m->serial++;
    p->slot = m->serial & 3;
    return 0;
}
