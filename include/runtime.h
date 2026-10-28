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

struct SceneRuntimeTask {
    struct RuntimeTask task;
    s32 state, counter;
};

extern struct SceneRuntimeTask gSceneLifecycleTask;

/* RAM objects; each row of symbols/ram.csv with header include/runtime.h. */
extern u8 gIwramCode
    []; /* IWRAM instruction buffer populated from Palette_CacheObjectBankArm through AgbMain__addr at startup; extent is not inferred from the partial RFU read */
extern u32
    gVideoIrqPhase; /* Two-phase video/sound IRQ handler latch; toggled by sub_08214144 (tentative name) */
extern u32
    gMessageQueueBufferIndex; /* Active double-buffered message queue index, toggled by MessageQueue_SwapBuffers */
extern u32
    gTrapDispatchStarted; /* Set by Trap_UpdateProbeContacts and reset when boxes are loaded */
extern u32
    gTransferSavedRuntimeFlag; /* Saves 0300546C while save transfer suspends the runtime; semantic name tentative */
extern s32 gSolarMeasurementPhase; /* Solar timer-interrupt measurement phase */
extern s32 gSolarInterruptCounter; /* Counter incremented during active sensor interrupts */
extern u32
    gSolarLastInput; /* Last GPIO light-input bit observed by the timer handler; tentative name */
extern u32 gSolarGpioShadow; /* GPIO output/control shadow used by solar timer phases */
extern u32
    gSolarEnabled; /* Software sensor-enable flag, set by Solar_Enable and cleared by Solar_Disable */
extern u8
    gRtcProtected; /* RTC GPIO protection flag set by Rtc_Protect and cleared by Rtc_Initialize */
extern u16
    gScriptEntryProcedure; /* Script entry procedure selected by Script_SetEntryProcedure; separate from the preceding MP2K buffer */
extern u32 gVideoUploadFlags; /* Video upload dirty flags; bit 0 requests OAM upload */
extern u32
    gSceneEntryActive; /* Set while scene-entry script runs; suppresses part of video upload task */
extern vu32 gVBlankOccurred; /* VBlank wait latch, volatile for interrupt polling */
extern u32
    gDialogueScrollOffset; /* Dialogue scroll/tick offset copied into the video IRQ shadow; tentative name */
extern u32
    gDialogueModeLatched; /* VBlank-latched dialogue state controlling the two-phase video IRQ; tentative name */
extern u8
    gInterruptCode[0x800]; /* IWRAM interrupt dispatcher copy, 512 words copied from IntrMain */
extern u32
    gVBlankUploadPending; /* Set by VBlank and cleared when the video upload task finishes; tentative name */
extern u32 gVBlankFrameCounter; /* Counter incremented after VBlank clock/link updates */
extern u32
    gDialogueScrollLatched; /* Dialogue scroll offset latched in VBlank and read as a byte by the secondary video IRQ */
extern u32 gVideoInterruptsEnabled; /* Gates video register updates and link/clock work in VBlank */
extern u16
    gVideoTransferControl; /* Runtime transfer-control flag initialized to 1 and cleared around save/wireless transfer; tentative name */
extern u16
    gHardwareKeys; /* Raw active-low KEYINPUT sampled before per-player input records are updated */
extern u32
    gLcgState; /* Independent 32-bit linear-congruential state, initialized to 123456 and advanced by Random_Next15 */
extern s32 gResourceRegistryCount; /* High-water count of resource-registry entries, capped at 32 */

#endif
