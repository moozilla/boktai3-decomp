#define Task_KillById   sub_0821A168
#define Task_TryDestroy sub_0821A0CC
#include "task_scheduler.h"
#include "engine_globals.h"
#include "render.h"

extern const u32 gUnk_08614114[];

u32 TaskSystem_InitLists(void)
{
    struct VideoDmaRegisters *d = (struct VideoDmaRegisters *)0x040000D4;
    d->source = (u32)gUnk_08614114;
    d->destination = (u32)&gTaskLists;
    d->control = 0x8400001C;
    return d->control;
}

void Task_LinkToPriorityList(struct RuntimeTask *task)
{
    struct TaskList *list = &gTaskLists[task->priority];
    struct RuntimeTask *head = list->head;
    if (head != 0)
        head->prev = task;
    task->next = head;
    list->head = task;
}

void Task_UnlinkFromPriorityList(struct RuntimeTask *task)
{
    struct TaskList *list = &gTaskLists[task->priority];
    struct RuntimeTask *prev = task->prev;
    struct RuntimeTask *next = task->next;
    if (prev != 0)
        prev->next = next;
    else
        list->head = next;
    if (next != 0)
        next->prev = prev;
}

struct RuntimeTask *Heap_Alloc(u32);

void Memory_Clear(struct RuntimeTask *, u32);

void Task_LinkToPriorityList(struct RuntimeTask *);

struct RuntimeTask *Task_Create(u32 priority, u32 bytes)
{
    struct RuntimeTask *task = Heap_Alloc(bytes);
    if (task == 0)
        return 0;
    Memory_Clear(task, bytes);
    task->priority = priority;
    if (gNextTaskId == 0)
        gNextTaskId++;
    task->id = gNextTaskId;
    gNextTaskId++;
    Task_LinkToPriorityList(task);
    return task;
}

struct RuntimeTask *Heap_AllocFromEnd(u32);

struct RuntimeTask *Task_CreateFromHeapEnd(u32 priority, u32 bytes)
{
    struct RuntimeTask *task = Heap_AllocFromEnd(bytes);
    if (task == 0)
        return 0;
    Memory_Clear(task, bytes);
    task->priority = priority;
    if (gNextTaskId == 0)
        gNextTaskId++;
    task->id = gNextTaskId;
    gNextTaskId++;
    Task_LinkToPriorityList(task);
    return task;
}

void Task_SetCallbacks(struct RuntimeTask *task, void (*a)(struct RuntimeTask *),
                       void (*b)(struct RuntimeTask *))
{
    task->update = a;
    task->destroy = b;
}

void Task_UnlinkFromPriorityList(struct RuntimeTask *);

void Heap_Free(struct RuntimeTask *);

void TaskSystem_RunFrame(void)
{
    struct TaskList *list = gTaskLists;
    s32 i;
    for (i = 0; i <= 13; i++, list++) {
        if (!(gEnginePauseFlags & list->pauseMask)) {
            struct RuntimeTask *task = list->head;
            while (task) {
                struct RuntimeTask *following = task->next;
                if (!(task->flags & 1)) {
                    if (task->update)
                        task->update(task);
                } else {
                    if (task->destroy)
                        task->destroy(task);
                    Task_UnlinkFromPriorityList(task);
                    Heap_Free(task);
                }
                task = following;
            }
        }
    }
}

s32 Task_Kill(struct RuntimeTask *task)
{
    task->flags |= 1;
    return 0;
}

s32 Callback_InvokeActor(void *arg, void *function);

s32 Task_TryDestroy(struct RuntimeTask *task)
{
    s32 result;
    if (task->destroy != NULL)
        result = Callback_InvokeActor(task, task->destroy);
    else
        result = 0;
    if (result >= 0) {
        task->destroy = NULL;
        task->flags |= 1;
    }
    return result;
}

s32 Task_Kill(struct RuntimeTask *);

void TaskSystem_KillBelowPersistenceLevel(s32 prio)
{
    struct TaskList *list = gTaskLists;
    s32 i = 0;
    do {
        struct RuntimeTask *task = list->head;
        s32 ni = i + 1;
        struct TaskList *nl = list + 1;
        for (; task != 0; task = task->next) {
            /* unk16 is the persistence level in the shared task header. */
            if (task->unk16 < prio)
                Task_Kill(task);
        }
        i = ni;
        list = nl;
    } while (i <= 13);
}

struct RuntimeTask *Task_FindById(u16 id)
{
    struct TaskList *list = gTaskLists;
    s32 i = 0;
    do {
        struct RuntimeTask *task = list->head;
        for (; task != 0; task = task->next) {
            if (task->id == id)
                return task;
        }
        i++;
        list++;
    } while (i <= 13);
    return 0;
}

/* Task_KillById: Finds a task by ID and marks it for destruction or returns -1 if absent. */

struct RuntimeTask *Task_FindById(u16);

s32 Task_KillById(u16 id)
{
    void *task = Task_FindById(id);
    s32 r;
    if (task)
        r = Task_Kill(task);
    else
        r = -1;
    return r;
}
