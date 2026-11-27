#include "objects/bouncing_bombs/bouncing_object.h"
s32 BouncingBomb_InitializePool(struct BouncingPool *pool)
{
    AnimData_Load(pool->graphics, 0xE74B);
    pool->sequenceResource = ScriptProc_Start(0x922E, 0x871C);
    pool->used = 0;
    pool->stopping = 0;
    gBouncingPool = pool;
    return 0;
}
