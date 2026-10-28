#ifndef GUARD_RUNTIME_H
#define GUARD_RUNTIME_H

#include "global.h"

/* Static scheduler tasks used by the video runtime (0x18-byte task header). */
struct RuntimeTask {
    struct RuntimeTask *prev, *next;
    void (*update)(struct RuntimeTask *);
    void (*destroy)(struct RuntimeTask *);
    u16 id, flags;
    u8 priority;
    u8 unk15, unk16, unk17;
};

extern struct RuntimeTask gVideoUploadTask;
extern struct RuntimeTask gGraphicsUpdateTask;
extern struct RuntimeTask gRootTask;

/* RAM objects; each row of symbols/ram.csv with header include/runtime.h. */
extern u8 gIwramCode
    []; /* IWRAM instruction buffer populated from Palette_CacheObjectBankArm through AgbMain__addr at startup; extent is not inferred from the partial RFU read */
extern u32
    gVideoIrqPhase; /* Two-phase video/sound IRQ handler latch; toggled by sub_08214144 (tentative name) */
extern u32
    gMessageQueueBufferIndex; /* Active double-buffered message queue index, toggled by MessageQueue_SwapBuffers */
extern u32
    gTrapDispatchStarted; /* Set by Trap_UpdateProbeContacts and reset when boxes are loaded */

#endif
