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
struct Manager *ActorPool_GetManager(void);
struct Manager *ActorPool_CreateManager(void);
void *Heap_Alloc(int);
void Memory_Clear(void *, int);
int ActorPool_LinkObject(struct Manager *, struct Object *);
void *ActorPool_AllocObject(int size)
{
    struct Manager *m = ActorPool_GetManager();
    struct Object *p;
    if (!m) {
        m = ActorPool_CreateManager();
        if (!m)
            return 0;
    }
    p = Heap_Alloc(size);
    if (!p)
        return 0;
    Memory_Clear(p, size);
    ActorPool_LinkObject(m, p);
    return p;
}
