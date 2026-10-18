#include "global.h"
s32 SceneCatalog_MapSelectionMask(s32 selection)
{
    if (selection == 0)
        return 4;
    if (selection == 1)
        return 2;
    return 1;
}
