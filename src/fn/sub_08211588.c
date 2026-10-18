#include "global.h"
void CollectionMenu_DestroyGraphics(u32);
u32 CollectionMenu_FinalizeState(u32);
u32 CollectionMenu_OnTaskDestroy(u32 a)
{
    CollectionMenu_DestroyGraphics(a);
    CollectionMenu_FinalizeState(a);
}
