#include "burst_emitter.h"
#define BurstEmitter_InitPool sub_08069F34
void sub_08069F30(void);
s32 BurstEmitter_InitPool(struct BurstEmitterPool *pool, u32 unused)
{
    sub_08069F30();
    gUnk_02000160 = pool;
    return 0;
}
