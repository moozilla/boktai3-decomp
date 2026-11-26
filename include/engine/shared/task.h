#ifndef GUARD_TASK_H
#define GUARD_TASK_H

#include "engine/shared/global.h"

/*
 * The header of every block the scheduler runs (docs/ENGINE.md section 3,
 * docs/OBJECT_MODEL.md). Task_Create(list, size) allocates a zeroed block of
 * size bytes that starts with this header; the rest of the block is the
 * object's own state, so an object struct begins with `struct Task task;`.
 * Its "class" is the pair of callbacks given to Task_SetCallbacks.
 */
struct Task {
    struct Task *prev, *next;
    void (*update)(struct Task *);  /* called every frame while alive */
    void (*destroy)(struct Task *); /* called once after Task_Kill */
    u16 id;                         /* unique, never 0 (gNextTaskId counter) */
    u16 flags;                      /* bit 0: marked for deletion */
    u8 list;                        /* index into gTaskLists (priority list, 0..13) */
};

#endif // GUARD_TASK_H
