#ifndef GUARD_INTERRUPTS_VIDEO_H
#define GUARD_INTERRUPTS_VIDEO_H
#include "global.h"
#include "runtime.h"
#include "gba/gba.h"

/* The three hardware words in each DMA channel. */
struct VideoDmaRegisters {
    vu32 source, destination, control;
};

void InterruptsAndVideo_Initialize(void);
void InterruptsAndVideo_InitRuntime(void);
void InterruptsAndVideo_WaitForVBlank(void);
void InterruptsAndVideo_WaitFrames(u16 frames);
u32 InterruptsAndVideo_OnVideoUploadTask(void);
u32 InterruptsAndVideo_OnGraphicsUpdateTask(void);
#endif
