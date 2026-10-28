#ifndef GUARD_RUNTIME_H
#define GUARD_RUNTIME_H

#include "global.h"
#include "engine_globals.h"

/* Static scheduler tasks used by the video runtime (0x18-byte task header). */
struct RuntimeTask {
    struct RuntimeTask *prev, *next;
    void (*update)(struct RuntimeTask *);
    void (*destroy)(struct RuntimeTask *);
    u16 id, flags;
    u8 priority;
    u8 unk15, unk16, unk17;
};

/* 0x03001118 */
extern struct RuntimeTask gVideoUploadTask;
/* 0x03001130 */
extern struct RuntimeTask gGraphicsUpdateTask;
/* 0x03001668 */
extern struct RuntimeTask gRootTask;

struct SceneRuntimeTask {
    struct RuntimeTask task;
    s32 state, counter;
};

/* 0x030025A0 */
extern struct SceneRuntimeTask gSceneLifecycleTask;

/* RAM objects; each row of symbols/ram.csv with header include/runtime.h. */
/* 0x03000000 */
extern u8 gIwramCode
    []; /* IWRAM instruction buffer populated from Palette_CacheObjectBankArm through AgbMain__addr at startup; extent is not inferred from the partial RFU read */
/* 0x03001110 */
extern u32
    gVideoIrqPhase; /* Two-phase video/sound IRQ handler latch; toggled by sub_08214144 (tentative name) */
/* 0x03001680 */
extern u32
    gMessageQueueBufferIndex; /* Active double-buffered message queue index, toggled by MessageQueue_SwapBuffers */
/* 0x03001684 */
extern u32
    gTrapDispatchStarted; /* Set by Trap_UpdateProbeContacts and reset when boxes are loaded */
/* 0x03002608 */
extern u32
    gTransferSavedRuntimeFlag; /* Saves 0300546C while save transfer suspends the runtime; semantic name tentative */
/* 0x030035B0 */
extern s32 gSolarMeasurementPhase; /* Solar timer-interrupt measurement phase */
/* 0x030035B4 */
extern s32 gSolarInterruptCounter; /* Counter incremented during active sensor interrupts */
/* 0x030035B8 */
extern u32
    gSolarLastInput; /* Last GPIO light-input bit observed by the timer handler; tentative name */
/* 0x030035BC */
extern u32 gSolarGpioShadow; /* GPIO output/control shadow used by solar timer phases */
/* 0x030035C0 */
extern u32
    gSolarEnabled; /* Software sensor-enable flag, set by Solar_Enable and cleared by Solar_Disable */
/* 0x030035C6 */
extern u8
    gRtcProtected; /* RTC GPIO protection flag set by Rtc_Protect and cleared by Rtc_Initialize */
/* 0x03003A04 */
extern u32 gVideoUploadFlags; /* Video upload dirty flags; bit 0 requests OAM upload */
/* 0x03003A08 */
extern u32
    gSceneEntryActive; /* Set while scene-entry script runs; suppresses part of video upload task */
/* 0x03003A44 */
extern vu32 gVBlankOccurred; /* VBlank wait latch, volatile for interrupt polling */
/* 0x03003A48 */
extern u32
    gDialogueScrollOffset; /* Dialogue scroll/tick offset copied into the video IRQ shadow; tentative name */
/* 0x03003A4C */
extern u32
    gDialogueModeLatched; /* VBlank-latched dialogue state controlling the two-phase video IRQ; tentative name */
/* 0x03003A60 */
extern u8
    gInterruptCode[0x800]; /* IWRAM interrupt dispatcher copy, 512 words copied from IntrMain */
/* 0x03004260 */
extern u32
    gVBlankUploadPending; /* Set by VBlank and cleared when the video upload task finishes; tentative name */
/* 0x03004264 */
extern u32 gVBlankFrameCounter; /* Counter incremented after VBlank clock/link updates */
/* 0x03004268 */
extern u32
    gDialogueScrollLatched; /* Dialogue scroll offset latched in VBlank and read as a byte by the secondary video IRQ */
/* 0x0300426C */
extern u32 gVideoInterruptsEnabled; /* Gates video register updates and link/clock work in VBlank */
/* 0x0300427C */
extern u16
    gVideoTransferControl; /* Runtime transfer-control flag initialized to 1 and cleared around save/wireless transfer; tentative name */
/* 0x03005270 */
extern u16
    gHardwareKeys; /* Raw active-low KEYINPUT sampled before per-player input records are updated */
/* 0x03005278 */
extern u32
    gLcgState; /* Independent 32-bit linear-congruential state, initialized to 123456 and advanced by Random_Next15 */
/* 0x030052F0 */
extern s32 gResourceRegistryCount; /* High-water count of resource-registry entries, capped at 32 */
/* 0x030053D8 */
extern u32
    gPlayTimeCounterEnabled; /* Enables the persistent play-time counter at game-state +0x614 */
/* 0x030053DC */
extern u32
    gSceneControlWord0; /* Cleared by scene transitions and Engine_ClearControlFlags; finer meaning unresolved */
/* 0x030053E0 */
extern u32
    gSceneInitWord0; /* Initialized to zero by the scene lifecycle manager; semantic name provisional */
/* 0x030053E4 */
extern u32 gRuntimeFrameCounter; /* Incremented on every scene lifecycle task update */
/* 0x030053E8 */
extern u32
    gSceneTransitionFlags; /* Pending scene transition flags, including active-region backup/restore bits */
/* 0x030053EC */
extern u32 gSceneControlWord1; /* Cleared by Engine_ClearControlFlags; finer meaning unresolved */
/* 0x030053F8 */
extern u8 *
    gSessionState; /* Persistent/session state pointer; byte base preserves existing subsystem-specific pointee views */
/* 0x030053FC */
extern u32
    gSceneTransitionActive; /* Set during transition cleanup and cleared when the scene starts */
/* 0x03005400 */
extern u32
    gSceneInitWord1; /* Initialized to zero by the scene lifecycle manager; semantic name provisional */
/* 0x03005404 */
extern u32 gSleepResumePending; /* Set after BIOS Stop and consumed by scene runtime refresh */
/* 0x03005408 */
extern s32
    gSceneDialogActive; /* Shared scene-dialog activity/control flag, retaining existing provisional aliases */
/* 0x0300540C */
extern u32
    gSceneUnpausedFrames; /* Counter incremented while the scene is not paused; provisional semantic name */
/* 0x03005410 */
extern u32 gSceneControlWord2; /* Cleared by Engine_ClearControlFlags; finer meaning unresolved */
/* 0x03005414 */
extern u32
    gSceneControlWord3; /* Cleared during transitions and by Engine_ClearControlFlags; finer meaning unresolved */
/* 0x030054B4 */
extern u16
    gSolarControlFlag; /* solar control flag set by sub_0822C180 and cleared by sub_0822C18C; purpose tentative */
/* 0x030054B8 */
extern u16
    gSolarSimulatedIntensity; /* simulated solar intensity ramping 0..140 (0x8C), returned by sub_0822C8D8 */
/* 0x030054BC */
extern u16
    gSolarPaused; /* solar task paused flag: set by Solar_PauseMeasurement, cleared by Solar_ResumeMeasurement */
/* 0x030054C0 */
extern u16
    gSolarSavedLevel; /* copy of the current solar level (game state +0x874), restored by sub_0822CA5C */
/* 0x030054C4 */
extern u16
    gSolarSavedCachedLevel; /* copy of the cached solar level (game state +0x876), restored by sub_0822CA5C */
/* 0x03006A30 */
extern u32
    gSolarSensorParam0; /* solar sensor timer parameter, set to 255 by Solar_InitTimer (meaning tentative) */
/* 0x03006A34 */
extern u32
    gSolarGpioSaved; /* cartridge GPIO data read by Solar_InitTimer; cleared by Solar_Enable */
/* 0x03006A38 */
extern u32
    gSolarMeasuring; /* solar measurement active flag (Solar_StartMeasurement sets, Solar_InitTimer clears) */
/* 0x03006A3C */
extern u32 gSolarLightBit; /* light-detect bit (GPIO bit 3) sampled by Solar_OnTimerInterrupt */
/* 0x03006A40 */
extern u32
    gSolarSensorParam1; /* solar sensor parameter, set to 1 by Solar_InitTimer (meaning tentative) */
/* 0x03006A44 */
extern u32
    gSolarSensorParam2; /* solar sensor parameter, set to 4 by Solar_InitTimer (meaning tentative) */
/* 0x03006A48 */
extern s32 gSolarMeasurement; /* measured solar value (counter >> 1), -1 while none */
/* 0x03006A4C */
extern u32 gSolarTimerPeriod; /* timer 3 period in cycles (0x3128); reload is its negation */

#endif
