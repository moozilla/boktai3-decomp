#include "global.h"
#include "engine_globals.h"

struct Task {
    u8 pad[0x10];
    u16 f10;
    u8 pad12[2];
    u8 f14;
};
struct Task *Heap_AllocFromEnd(u32);
void Memory_Clear(struct Task *, u32);
void Task_LinkToPriorityList(struct Task *);

struct Task *Task_CreateFromHeapEnd(u32 a, u32 b)
{
    struct Task *t = Heap_AllocFromEnd(b);
    if (t == 0)
        return 0;
    Memory_Clear(t, b);
    t->f14 = a;
    if (gNextTaskId == 0)
        gNextTaskId++;
    t->f10 = gNextTaskId;
    gNextTaskId++;
    Task_LinkToPriorityList(t);
    return t;
}
