#ifndef GUARD_INCLUDE_EFFECTS_SCENE_ACTOR_EFFECTS_ACTIVE_LIST_LAYOUT_H
#define GUARD_INCLUDE_EFFECTS_SCENE_ACTOR_EFFECTS_ACTIVE_LIST_LAYOUT_H

#include "global.h"

struct ActiveList {
    u8 task[0x18]; /* confidence=low */
    void *animData; /* confidence=low */
    void *sequence; /* confidence=low */
    void *head;
};

typedef char ActiveListSizeCheck[sizeof(struct ActiveList) == 0x24 ? 1 : -1];

#endif
