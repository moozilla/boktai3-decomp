#ifndef GUARD_RAM_H
#define GUARD_RAM_H

#include "global.h"

typedef void (*SerialIntrFunc)(void);

/* RAM objects; each row of symbols/ram.csv with header include/ram.h. */
extern u8 *gPanelSpritePool;
extern u8 *gDirectionalSpritePool;
extern void *gActorManager;
/* 0x0300531C */
extern s32 gSerialConnectionRole;
/* 0x03005324 */
extern s32 gSerialTransferStatus;
extern u8 *gScriptRecordTable;
extern u8 *gFieldHud;
extern u8 *gSlotController;
extern struct ScreenDelayParameters gScreenDelayParameters;
extern u8 *gActorEffectManager;
/* 0x03003A50 */
extern u32 gSceneEffectPalettePhase;
extern u32 gSpriteBurstManager;
extern struct MotionTargets *gSceneLinkList;
extern u32 gSpritePairPool;
/* 0x03006A9C */
extern const struct EEPROMConfig *gEEPROMConfig;
extern u8 gAnimPanelSavedSelections[];
/* 0x03005304 */
extern u32 gEffectRandomTableIndex;
/* 0x03005470 */
extern u16 gMusicSlotSongs[];
extern u8 *gTerrainColumnIndices;
extern struct TerrainColumn *gTerrainColumns;
extern struct Packet gMessagePacket;
extern void *gHresvelgrProjectilePool;
extern struct ProjectilePool *gPooledSpriteManager;
extern struct SpriteEffectPool *gSpriteEffectPool;
extern struct SpatialIndex *gSpatialIndex;
extern struct SceneParticlePool *gSceneParticlePool;
extern u32 gPanelSequence;
extern struct Slot600Manager *gFanCannonManager;
extern u32 gMovingSpritePool;
extern u8 *gSelectionPanel;
extern u32 gSceneProcParticles;
extern u32 gScreenMoteManager;
extern u32 gNamedEntryPool;
extern struct TileTriggerManager *gTileTriggerManager;
extern struct SmallCallbackPool *gCompactParticlePool;
extern void *gMotionEntryManager;
extern u32 gLinkPanel;
/* 0x03005274 */
extern u16 gSceneSelectionDisplaySlots[];
extern struct Pool *gPursuingActorManager;
extern u32 gSpriteSlots;
extern u32 gScriptedColliderManager;
extern u16 gPlayerNotificationPending;
extern u32 gSegmentDisplay;
extern u32 gActiveRecordMenu;
/* 0x03001734 */
extern u32 gSerialLatestPacketIndex;
/* 0x03005340 */
extern s16 gSerialLatestPackets[];
extern struct RegionSpriteManager *gRegionActorPool;
extern u32 gEffectQuad;
/* 0x03003A10 */
extern SerialIntrFunc gSerialInterruptState[];
extern u8 *gCompactSpritePool;
/* 0x03005250 */
extern u32 gLinkMenuActive;
extern u16 gPlayerCount;
extern u16 gPlayerCueTimer;
extern struct SpriteParticlePool *gSpriteGroup;
extern u32 gSceneStatusIndicatorManager;
extern u8 *gSceneProcSprites;
extern struct FeatherPool *gFadingSpritePool;
/* 0x03005420 */
extern u8 *gActiveCamera;
/* 0x03005300 */
extern u32 gTerrainTrapSerial;
extern void *gDirectionBurstEmitter;
extern u32 gNamePlaceholderScript[];
/* 0x03001730 */
extern u32 gSerialReceivePending;
/* 0x0300172C */
extern u32 gSerialReceiveIndex;
/* 0x03005370 */
extern s16 gSerialReceivePackets[];
extern u32 gSceneCallbackPool;
extern u8 *gWoodenCrateManager;
extern u32 gColliderSpritePool;
extern u32 gSlotMonitor;
extern struct ScriptPromptSettings gScriptPromptSettings;
extern u16 gSceneHudMode;
extern u16 gForceDirectionMode;
extern u8 *gAnimatedBodyPool;
extern u32 gRisingSpritePool;
extern u32 gSpriteNoticeManager;
/* 0x03001688 */
extern u8 *gColliderManager;
extern void *gSceneComposite;
extern u16 gTrackedPlayerId;
extern u32 gActorParticlePool;
extern u32 gGrowthHazardManager;
extern struct FragmentBurstPool *gSpriteGroupPool;
extern struct ColorFadeController *gColorFadeController;
extern struct LinkedWidgetSet *gLinkedWidgetSet;
extern struct WeatherState *gWeatherState;
extern struct WindowManager *gWindowManager;
extern struct Transition *gTransition;
extern struct LinkMenu *gLinkMenu;
extern struct ResultMenu *gResultMenu;
extern struct SceneActorControlManager *gSceneActorControlManager;
extern struct ColliderEffectPool *gColliderEffectPool;
extern struct SceneParticleBurstPool *gSceneParticleBurstPool;
extern struct PairSlotPool *gPairSlotPool;
extern struct BurstSparkPool *gBurstSparkPool;
extern struct PaletteFade *gPaletteFade;
extern struct ParticleManager *gParticleManager;
extern struct GameOver *gGameOver;
extern struct SceneNpc *gSceneNpc;
extern struct CloudTask *gCloudTask;
extern struct ScreenParams gScreenParams;
extern struct ScreenTextParameters gScreenTextParameters;
extern struct ScriptCommandTable gScriptCommandTable;
extern struct DebrisPool *gDebrisPool;
extern struct ParticlePool *gParticlePool;
extern struct HomingManager *gHomingManager;
/* 0x030025E8 */
extern struct CommandSet gCommandSet;
/* 0x03002600 */
extern struct RegionList *gRegionList;
/* 0x030054D0 */
extern struct Snapshot gSnapshot;
extern u32 gShadowManager;              /* shadow task singleton (Shadow_*) */
extern u32 gScaledShadowManager;        /* scaled shadow manager singleton (ScaledShadow_*) */
extern void *gSceneActorEffectsManager; /* scene actor effects manager (SceneActorEffects_*) */
extern u32 gFragmentManager;            /* fragment manager singleton (FragmentManager_*) */
extern u32 gAnimatedParticleManager;    /* animated particle manager (AnimatedParticleManager_*) */
extern void *gFadingParticleManager;    /* fading particle manager (FadingParticleManager_*) */
extern u32 gScanlineWave;               /* scanline wave effect task (ScanlineWave_*) */
extern u32 gPaletteCycle;               /* palette cycle task (PaletteCycle_*) */
extern u32 gCircleWindow;               /* circle window task (CircleWindow_*) */
extern u32 gTileAnimation;              /* tile animation task (TileAnimation_*) */
extern u8 *gTransientSpritePool;        /* transient sprite pool (TransientSpritePool_*) */
extern u32 gSpritePoolManager;          /* sprite pool singleton (SpritePool_*) */
extern struct Actor *gMessageWindow;    /* message window (MessageWindow_*) */
extern struct Window *gScrollingDialogue; /* scrolling dialogue window (ScrollingDialogue_*) */
extern u8 gWirelessSessionState;          /* wireless session state byte (Wireless_*) */
extern u32 gWirelessTradeMenu;            /* wireless trade/exchange menu (WirelessTradeMenu_*) */
extern u32 gScenePool;                    /* scene pool manager (ScenePool_*) */
extern u8 *gScriptedParticlePool;         /* scripted particle pool (ScriptedParticle_*) */
extern u8 *gSunDiscPool;                  /* sun disc pool (SunDiscPool_*) */
extern u8 *gFragmentEffectManager;        /* fragment effect manager (FragmentEffect_*) */
extern u32 gFloorSwitchManager;           /* floor switch manager (FloorSwitch_*) */
extern u32 gSaveMenu;                     /* save menu task (SaveMenu_*) */
extern u8 *gSceneEffectQueue;             /* scene effect queue (SceneEffectQueue_*) */
extern void *gEffectPoolManager;          /* effect pool (EffectPool_*) */
extern void *gVanargandPartController;    /* Vanargand part controller */
extern void *gSparkleEmitter;             /* sparkle emitter (SparkleEmitter_*) */
extern u32 gShipWheel;                    /* ship wheel task (ShipWheel_*) */
extern u16 gInventoryUseFlags;            /* inventory use-permission flags */
extern void *gHresvelgr;                  /* Hresvelgr boss task (Hresvelgr_Create/Destroy) */
extern u32 gRatatoskParticlePool;         /* Ratatosk animated particle pool */
extern u32 gSceneProjectilePool;          /* scene projectile pool (SceneProjectile_*) */
extern u32 gSpriteParticleManager;        /* sprite particle manager (SpriteParticle_*) */
extern u32 gProjectilePool;               /* projectile pool (ProjectilePool_*) */
extern u32 gEnemyParticlePool;            /* enemy particle pool (EnemyParticle_*) */
extern u32 gForgingBook;                  /* forging book task (ForgingBook_*) */
extern u32 gShadowRegistry;               /* shadow registry task (ShadowRegistry_*) */
/* 0x03002604 */
extern u8 *gSolarController; /* solar sensor controller (Solar_*) */
/* 0x030053F0 */
extern u32 gScenePauseFlags; /* scene pause/mute flags (bit tests across effects and actors) */
extern void *
    gParticleGroupManager; /* provisional: six-group particle manager singleton (ParticleManager_Initialize) */
extern s32
    gWeatherCooldownBase; /* weather cooldown base, configured by Weather_ConfigureCooldown */
extern s32 gWeatherCooldownRange; /* random range added to the weather cooldown */
extern u32 gStormActive;          /* set while Weather_Update is in storm phase 3 */
extern s32 gStormTimer; /* provisional: storm duration/delay countdown shared by weather phases */
extern u8
    *gSceneActorRegistry[8]; /* eight scene actor registry slots (SceneActors_FindAvailable) */
extern void
    *gActorPoolManager; /* actor pool singleton; separate slot after the eight-entry registry */
extern u32 gWirelessLastStatus; /* cached wireless status used when its controller is absent */
extern struct ActorAttachmentManagerView
    *gActorAttachmentManager; /* actor attachment manager singleton */
extern struct SceneParticlePool
    *gSceneParticleEmitterPool; /* provisional: scene particle emitter pool singleton */
extern struct NamedObjectManager
    *gNamedActorManager;           /* named actor object pool singleton; provisional common name */
extern void *gActorGroupPool;      /* four-slot actor group and callback pool singleton */
extern u32 gLinkSessionStep;       /* provisional: Registry1745_RunSession diagnostic step marker */
extern u8 *gConfirmPanel;          /* provisional: Registry3C9F confirmation panel singleton */
extern void *gRegistryE7BCManager; /* provisional: registry E7BC pooled-object manager */
extern s32 gOffsetActorAnimationRepeats; /* repeated offset-actor animation counter */
extern s32 gHresvelgrAttackCount;        /* provisional: Hresvelgr attack-cycle counter */
extern s32
    gHresvelgrHazardIndex; /* nearest active growth hazard selected by Hresvelgr_UpdateHazardVisit */
extern u32 gHresvelgrVolleyState;            /* provisional: Hresvelgr volley state word */
extern s32 gHresvelgrVolleyCount;            /* alternating Hresvelgr volley pulse counter */
extern struct F3E4Actor *gRegistryF3E4Actor; /* provisional: registry F3E4 singleton actor */
extern u16
    gSceneNpcBlendPalette[16]; /* sixteen-color output palette blended by SceneNpc_BlendPalette */
extern u8
    *gScriptedActorController;     /* provisional: singleton scripted actor animation controller */
extern u8 *gScriptSectionBases[4]; /* four resolved section bases from the loaded script header */
extern struct ScriptCommandTable *
    gScriptCommandTables; /* registered native command-table list head; separate from section bases */
extern s32 gWeatherCollisionTimer;   /* provisional: shared weather and collision countdown */
extern void *gSceneActorListManager; /* provisional: scene actor list and packet-queue manager */
extern struct Engine *
    gWirelessController; /* wireless controller singleton; existing per-unit Engine views retained */
extern u16 gPlayerInteractionMode; /* provisional: player interaction and passage mode halfword */
extern u16
    gPlayerSavedMenuOption; /* saved menu option restored when the second script-status slot is set */
extern u16 gPlayerLinkAttackBonus; /* provisional: link action-mode attack bonus override */
extern u16 gPlayerScriptState;     /* provisional: shared player script-state halfword */
extern u16 gPlayerActionPending;   /* provisional: shared pending player-action counter */
extern u16 gPlayerDirectionLatch;  /* provisional: direction and action latch halfword */
extern u16 gPlayerPassageState;    /* provisional: shared player passage/interaction state */
extern u16 gPlayerBackgroundMode;  /* provisional: background/player action mode halfword */
extern u16
    gPlayerInitialSlotValues[2]; /* two script-status values saved before the local player exists */
extern void *
    gPlayerPendingActor; /* provisional: pending player interaction/notification actor; independent of slot values */
extern u16 gPlayerBackgroundState; /* provisional: shared background/player state halfword */
extern u16
    gPlayerAbilityLatched; /* provisional: ability request latch; ROM reader loads an unsigned halfword */
extern u16
    gPlayerSolarActionPending; /* provisional: solar action pending flag; separate from four slot words */
extern u16
    gPlayerEffectPendingCount; /* provisional: pending player effect count; separate from actor pointers */
extern u8 *gPlayerAttackEffect; /* player attack effect singleton */
extern u16 gHudResetFlag;       /* provisional: HUD flag initialized to one by Hud_ResetGlobals */
extern u8 gNameEntrySavedState[4]; /* cursor position, selection and per-slot name-entry state */
extern s32
    gScreenTextSavedValue; /* provisional: value retained between screen-text menu instances */
extern u32
    gSaveStateSignature; /* main-state save signature checked when reading and written before saving */
extern u8 *
    gScriptLocalVariables; /* 0x400-byte local script-variable region at selected base plus 0x1E00 */
extern u8 *gBackupGameVariables;    /* backup game-variable region at selected base plus 0x1A00 */
extern u8 *gGameVariables;          /* active 0x400-byte game-variable region */
extern u8 *gBackupGameState;        /* backup main-state region at selected base plus 0xB00 */
extern u32 gHeapEndGuard;           /* heap-end guard word expected to equal 0x516B92EE */
extern u8 gBgMapWorkBuffer[0x2000]; /* background map work buffer, cleared for 0x2000 bytes */
extern u8
    gGameStateStorage[]; /* backing storage for timer-jittered game-state and variable regions */
/* 0x03005428 */
extern u16
    gSilentGlyph; /* glyph code that plays no text sound; set by Text_SetSilentCharacterFromScript, compared in TextState_PlayGlyphSound */
/* 0x030054B0 */
extern u32
    gRegionOverride; /* region override flag: set by Region_SetCurrentFromScript, cleared when the region list is destroyed */
/* 0x0300546C */
extern u32
    gClockSynced; /* nonzero once the clock has been synced from the RTC; SunClock_Refresh syncs while it is zero */

#endif
