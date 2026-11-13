#ifndef GUARD_SCANLINE_WAVE_H
#define GUARD_SCANLINE_WAVE_H
#include "engine.h"

/* 128-line periodic wave with 160 extra samples for a contiguous visible window. */
struct ScanlineWave {
    struct Task task;
    u16 id, target, mode;
    u8 start, end;
    u16 step, phase, period, timer;
    u32 enabled;
    u16 bank;
    s16 samples[1][288];
    u8 unknown26E[6]; /* allocation extends six bytes past the generated samples */
};
s32 ScanlineWave_Update(struct ScanlineWave *);
u32 ScanlineWave_Destroy(void);
s32 ScanlineWave_Initialize(struct ScanlineWave *, s32);
struct ScanlineWave *ScanlineWave_Create(s32);
void ScanlineEffect_SetSource(s32, s32, void *);
void sub_082194D4(void);
#endif
