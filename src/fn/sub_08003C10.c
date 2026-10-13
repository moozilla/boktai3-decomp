#include "global.h"

u32 TerrainOverlay_QueueCommand(u32 a, u32 b);

u32 sub_08003C10(u32 *p)
{
    return TerrainOverlay_QueueCommand(1, *p);
}
