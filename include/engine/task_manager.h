#ifndef GUARD_TASK_MANAGER_H
#define GUARD_TASK_MANAGER_H

#include "global.h"

/* Effect managers register their task in a singleton slot. */
static inline void Task_SetManager(void *manager, void **slot)
{
    *slot = manager;
}

#endif
