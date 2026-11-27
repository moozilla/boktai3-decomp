#ifndef GUARD_INCLUDE_UI_BOOKS_COLLECTION_DISPLAY_RESOURCES_H
#define GUARD_INCLUDE_UI_BOOKS_COLLECTION_DISPLAY_RESOURCES_H

#include "global.h"

struct CollectionDisplayResources {
    u32 w[8];
};

typedef char CollectionDisplayResourcesSizeCheck[sizeof(struct CollectionDisplayResources) == 0x20 ? 1 : -1];

#endif
