#ifndef GUARD_RAM_H
#define GUARD_RAM_H

#include "global.h"

typedef void (*SerialIntrFunc)(void);

/* RAM objects; each row of symbols/ram.csv with header include/ram.h. */
/* 0x020004BC */
extern u8 *gPanelSpritePool;
/* 0x020005F8 */
extern u8 *gDirectionalSpritePool;
/* 0x0200020C */
extern void *gActorManager;
/* 0x0300531C */
extern s32 gSerialConnectionRole;
/* 0x03005324 */
extern s32 gSerialTransferStatus;
/* 0x02000208 */
extern u8 *gScriptRecordTable;
/* 0x02000214 */
extern u8 *gFieldHud;
/* 0x020005EC */
extern u8 *gSlotController;
/* 0x02000420 */
extern struct ScreenDelayParameters gScreenDelayParameters;
/* 0x02000180 */
extern u8 *gActorEffectManager;
/* 0x03003A50 */
extern u32 gSceneEffectPalettePhase;
/* 0x02000150 */
extern u32 gSpriteBurstManager;
/* 0x02000598 */
extern struct MotionTargets *gSceneLinkList;
/* 0x020004D4 */
extern u32 gSpritePairPool;
/* 0x03006A9C */
extern const struct EEPROMConfig *gEEPROMConfig;
/* 0x020005C0 */
extern u8 gAnimPanelSavedSelections[];
/* 0x03005304 */
extern u32 gEffectRandomTableIndex; /* wraps through the fixed ROM sample table at 0x086141BC */
/* 0x03005470 */
extern u16 gMusicSlotSongs[];
/* 0x020001F8 */
extern u8 *gTerrainColumnIndices;
/* 0x020001FC */
extern struct TerrainColumn *gTerrainColumns;
/* 0x020001F0 */
extern struct Packet gMessagePacket;
/* 0x02000244 */
extern void *gHresvelgrProjectilePool;
/* 0x02000164 */
extern struct ProjectilePool *gPooledSpriteManager;
/* 0x0200013C */
extern struct SpriteEffectPool *gSpriteEffectPool;
/* 0x02000200 */
extern struct SpatialIndex *gSpatialIndex;
/* 0x02000194 */
extern struct SceneParticlePool *gSceneParticlePool;
/* 0x020001C8 */
extern u32 gPanelSequence;
/* 0x02000600 */
extern struct Slot600Manager *gFanCannonManager;
/* 0x0200016C */
extern u32 gMovingSpritePool;
/* 0x0200025C */
extern u8 *gSelectionPanel;
/* 0x020004D0 */
extern u32 gSceneProcParticles;
/* 0x020001E0 */
extern u32 gScreenMoteManager;
/* 0x020001BC */
extern u32 gNamedEntryPool;
/* 0x020001B8 */
extern struct TileTriggerManager *gTileTriggerManager;
/* 0x020001E8 */
extern struct SmallCallbackPool *gCompactParticlePool;
/* 0x02000240 */
extern void *gMotionEntryManager;
/* 0x020001C4 */
extern u32 gLinkPanel;
/* 0x03005274 */
extern u16 gSceneSelectionDisplaySlots[];
/* 0x02000184 */
extern struct Pool *gPursuingActorManager;
/* 0x020004C0 */
extern u32 gSpriteSlots;
/* 0x02000190 */
extern u32 gScriptedColliderManager;
/* 0x02000594 */
extern u16 gPlayerNotificationPending;
/* 0x02000260 */
extern u32 gSegmentDisplay;
/* 0x020001D0 */
extern u32 gActiveRecordMenu;
/* 0x03001734 */
extern u32 gSerialLatestPacketIndex;
/* 0x03005340 */
extern s16 gSerialLatestPackets[];
/* 0x020001B4 */
extern struct RegionSpriteManager *gRegionActorPool;
/* 0x02000098 */
extern u32 gEffectQuad;
/* 0x03003A10 */
extern SerialIntrFunc gSerialInterruptState[];
/* 0x020004D8 */
extern u8 *gCompactSpritePool;
/* 0x03005250 */
extern u32 gLinkMenuActive;
/* 0x02000544 */
extern u16 gPlayerCount;
/* 0x02000518 */
extern u16 gPlayerCueTimer;
/* 0x02000138 */
extern struct SpriteParticlePool *gSpriteGroup;
/* 0x02000158 */
extern u32 gSceneStatusIndicatorManager;
/* 0x020004CC */
extern u8 *gSceneProcSprites;
/* 0x02000174 */
extern struct FeatherPool *gFadingSpritePool;
/* 0x03005420 */
extern u8 *gActiveCamera;
/* 0x03005300 */
extern u32 gTerrainTrapSerial;
/* 0x02000238 */
extern void *gDirectionBurstEmitter;
/* 0x02000328 */
extern u32 gNamePlaceholderScript[];
/* 0x03001730 */
extern u32 gSerialReceivePending;
/* 0x0300172C */
extern u32 gSerialReceiveIndex;
/* 0x03005370 */
extern s16 gSerialReceivePackets[];
/* 0x020004C8 */
extern struct SceneCallbackPool *gSceneCallbackPool;
/* 0x020001A4 */
extern u8 *gWoodenCrateManager;
/* 0x02000474 */
extern u32 gColliderSpritePool;
/* 0x020004B0 */
extern u32 gSlotMonitor;
/* 0x02000388 */
extern struct ScriptPromptSettings gScriptPromptSettings;
/* 0x020005A4 */
extern u16 gSceneHudMode;
/* 0x02000558 */
extern u16 gForceDirectionMode;
/* 0x02000140 */
extern u8 *gAnimatedBodyPool;
/* 0x020004A0 */
extern u32 gRisingSpritePool;
/* 0x02000144 */
extern u32 gSpriteNoticeManager;
/* 0x03001688 */
extern u8 *gColliderManager;
/* 0x02000250 */
extern void *gSceneComposite;
/* 0x02000508 */
extern u16 gTrackedPlayerId[4];
/* 0x020001E4 */
extern u32 gActorParticlePool;
/* 0x02000258 */
extern u32 gGrowthHazardManager;
/* 0x02000170 */
extern struct FragmentBurstPool *gSpriteGroupPool;
/* 0x02000020 */
extern struct ColorFadeController *gColorFadeController;
/* 0x02000078 */
extern struct LinkedWidgetSet *gLinkedWidgetSet;
/* 0x02000090 */
extern struct WeatherState *gWeatherState;
/* 0x020000E4 */
extern struct WindowManager *gWindowManager;
/* 0x020000E8 */
extern struct Transition *gTransition;
/* 0x020000F8 */
extern struct LinkMenu *gLinkMenu;
/* 0x02000104 */
extern struct ResultMenu *gResultMenu;
/* 0x02000128 */
extern struct SceneActorControlManager *gSceneActorControlManager;
/* 0x0200012C */
extern struct ColliderEffectPool *gColliderEffectPool;
/* 0x02000130 */
extern struct SceneParticleBurstPool *gSceneParticleBurstPool;
/* 0x02000148 */
extern struct PairSlotPool *gPairSlotPool;
/* 0x0200015C */
extern struct BurstSparkPool *gBurstSparkPool;
/* 0x020001D8 */
extern struct PaletteFade *gPaletteFade;
/* 0x020001DC */
extern struct ParticleManager *gParticleManager;
/* 0x02000210 */
extern struct GameOver *gGameOver;
/* 0x02000274 */
extern struct SceneNpc *gSceneNpc;
/* 0x02000298 */
extern struct CloudTask *gCloudTask;
/* 0x020002F8 */
extern struct ScreenParams gScreenParams;
/* 0x020003F8 */
extern struct ScreenTextParameters gScreenTextParameters;
/* 0x02000428 */
extern struct ScriptCommandTable gScriptCommandTable;
/* 0x020004C4 */
extern struct DebrisPool *gDebrisPool;
/* 0x020005E0 */
extern struct ParticlePool *gParticlePool;
/* 0x020005F4 */
extern struct HomingManager *gHomingManager;
/* 0x030025E8 */
extern struct CommandSet gCommandSet;
/* 0x03002600 */
extern struct RegionList *gRegionList;
/* 0x030054D0 */
extern struct Snapshot gSnapshot;
/* 0x02000054 */
extern u32 gShadowManager; /* shadow task singleton (Shadow_*) */
/* 0x02000058 */
extern u32 gScaledShadowManager; /* scaled shadow manager singleton (ScaledShadow_*) */
/* 0x0200005C */
extern void *gSceneActorEffectsManager; /* scene actor effects manager (SceneActorEffects_*) */
/* 0x02000064 */
extern u32 gFragmentManager; /* fragment manager singleton (FragmentManager_*) */
/* 0x02000068 */
extern u32 gAnimatedParticleManager; /* animated particle manager (AnimatedParticleManager_*) */
/* 0x0200006C */
extern void *gFadingParticleManager; /* fading particle manager (FadingParticleManager_*) */
/* 0x0200007C */
extern u32 gScanlineWave; /* scanline wave effect task (ScanlineWave_*) */
/* 0x02000094 */
extern u32 gPaletteCycle; /* palette cycle task (PaletteCycle_*) */
/* 0x0200009C */
extern u32 gCircleWindow; /* circle window task (CircleWindow_*) */
/* 0x020000A4 */
extern u32 gTileAnimation; /* tile animation task (TileAnimation_*) */
/* 0x020000AC */
extern u8 *gTransientSpritePool; /* transient sprite pool (TransientSpritePool_*) */
/* 0x020000B0 */
extern u32 gSpritePoolManager; /* sprite pool singleton (SpritePool_*) */
/* 0x020000DC */
extern struct Actor *gMessageWindow; /* message window (MessageWindow_*) */
/* 0x020000E0 */
extern struct Window *gScrollingDialogue; /* scrolling dialogue window (ScrollingDialogue_*) */
/* 0x020000EC */
extern u8 gWirelessSessionState; /* wireless session state byte (Wireless_*) */
/* 0x020000FC */
extern u32 gWirelessTradeMenu; /* wireless trade/exchange menu (WirelessTradeMenu_*) */
/* 0x02000108 */
extern u32 gScenePool; /* scene pool manager (ScenePool_*) */
/* 0x02000154 */
extern u8 *gScriptedParticlePool; /* scripted particle pool (ScriptedParticle_*) */
/* 0x0200017C */
extern u8 *gSunDiscPool; /* sun disc pool (SunDiscPool_*) */
/* 0x0200018C */
extern u8 *gFragmentEffectManager; /* fragment effect manager (FragmentEffect_*) */
/* 0x020001A8 */
extern u32 gFloorSwitchManager; /* floor switch manager (FloorSwitch_*) */
/* 0x020001D4 */
extern u32 gSaveMenu; /* save menu task (SaveMenu_*) */
/* 0x02000218 */
extern u8 *gSceneEffectQueue; /* scene effect queue (SceneEffectQueue_*) */
/* 0x0200023C */
extern void *gEffectPoolManager; /* effect pool (EffectPool_*) */
/* 0x02000254 */
extern void *gVanargandPartController; /* Vanargand part controller */
/* 0x02000264 */
extern void *gSparkleEmitter; /* sparkle emitter (SparkleEmitter_*) */
/* 0x0200026C */
extern u32 gShipWheel; /* ship wheel task (ShipWheel_*) */
/* 0x020005A0 */
extern u16 gInventoryUseFlags; /* inventory use-permission flags */
/* 0x020005AC */
extern void *gHresvelgr; /* Hresvelgr boss task (Hresvelgr_Create/Destroy) */
/* 0x020005E4 */
extern u32 gRatatoskParticlePool; /* Ratatosk animated particle pool */
/* 0x020005E8 */
extern u32 gSceneProjectilePool; /* scene projectile pool (SceneProjectile_*) */
/* 0x020005F0 */
extern u32 gSpriteParticleManager; /* sprite particle manager (SpriteParticle_*) */
/* 0x020005FC */
extern u32 gProjectilePool; /* projectile pool (ProjectilePool_*) */
/* 0x02000604 */
extern u32 gEnemyParticlePool; /* enemy particle pool (EnemyParticle_*) */
/* 0x02000608 */
extern u32 gForgingBook; /* forging book task (ForgingBook_*) */
/* 0x02000470 */
extern u32 gShadowRegistry; /* shadow registry task (ShadowRegistry_*) */
/* 0x03002604 */
extern u8 *gSolarController; /* solar sensor controller (Solar_*) */
/* 0x030053F0 */
extern u32 gScenePauseFlags; /* scene pause/mute flags (bit tests across effects and actors) */
/* 0x02000060 */
extern void *
    gParticleGroupManager; /* provisional: six-group particle manager singleton (ParticleManager_Initialize) */
/* 0x02000080 */
extern s32
    gWeatherCooldownBase; /* weather cooldown base, configured by Weather_ConfigureCooldown */
/* 0x02000084 */
extern s32 gWeatherCooldownRange; /* random range added to the weather cooldown */
/* 0x02000088 */
extern u32 gStormActive; /* set while Weather_Update is in storm phase 3 */
/* 0x0200008C */
extern s32 gStormTimer; /* provisional: storm duration/delay countdown shared by weather phases */
/* 0x020000B8 */
extern u8
    *gSceneActorRegistry[8]; /* eight scene actor registry slots (SceneActors_FindAvailable) */
/* 0x020000D8 */
extern void
    *gActorPoolManager; /* actor pool singleton; separate slot after the eight-entry registry */
/* 0x020000F0 */
extern u32 gWirelessLastStatus; /* cached wireless status used when its controller is absent */
/* 0x02000124 */
extern struct ActorAttachmentManagerView
    *gActorAttachmentManager; /* actor attachment manager singleton */
/* 0x02000134 */
extern struct SceneParticlePool
    *gSceneParticleEmitterPool; /* provisional: scene particle emitter pool singleton */
/* 0x020001AC */
extern struct NamedObjectManager
    *gNamedActorManager; /* named actor object pool singleton; provisional common name */
/* 0x020001B0 */
extern void *gActorGroupPool; /* four-slot actor group and callback pool singleton */
/* 0x020001C0 */
extern u32 gLinkSessionStep; /* provisional: Registry1745_RunSession diagnostic step marker */
/* 0x020001CC */
extern u8 *gConfirmPanel; /* provisional: Registry3C9F confirmation panel singleton */
/* 0x02000204 */
extern void *gRegistryE7BCManager; /* provisional: registry E7BC pooled-object manager */
/* 0x0200021C */
extern s32 gOffsetActorAnimationRepeats; /* repeated offset-actor animation counter */
/* 0x02000220 */
extern s32 gHresvelgrAttackCount; /* provisional: Hresvelgr attack-cycle counter */
/* 0x0200022C */
extern s32
    gHresvelgrHazardIndex; /* nearest active growth hazard selected by Hresvelgr_UpdateHazardVisit */
/* 0x02000230 */
extern u32 gHresvelgrVolleyState; /* provisional: Hresvelgr volley state word */
/* 0x02000234 */
extern s32 gHresvelgrVolleyCount; /* alternating Hresvelgr volley pulse counter */
/* 0x02000268 */
extern struct F3E4Actor *gRegistryF3E4Actor; /* provisional: registry F3E4 singleton actor */
/* 0x02000278 */
extern u16
    gSceneNpcBlendPalette[16]; /* sixteen-color output palette blended by SceneNpc_BlendPalette */
/* 0x020003C8 */
extern u8
    *gScriptedActorController; /* provisional: singleton scripted actor animation controller */
/* 0x02000448 */
extern u8 *gScriptSectionBases[4]; /* four resolved section bases from the loaded script header */
/* 0x02000458 */
extern struct ScriptCommandTable *
    gScriptCommandTables; /* registered native command-table list head; separate from section bases */
/* 0x02000478 */
extern s32 gWeatherCollisionTimer; /* provisional: shared weather and collision countdown */
/* 0x02000480 */
extern void *gSceneActorListManager; /* provisional: scene actor list and packet-queue manager */
/* 0x02000484 */
extern struct Engine *
    gWirelessController; /* wireless controller singleton; existing per-unit Engine views retained */
/* 0x02000500 */
extern u16 gPlayerInteractionMode; /* provisional: player interaction and passage mode halfword */
/* 0x02000504 */
extern u16
    gPlayerSavedMenuOption; /* saved menu option restored when the second script-status slot is set */
/* 0x02000514 */
extern u16 gPlayerLinkAttackBonus; /* provisional: link action-mode attack bonus override */
/* 0x02000520 */
extern u16 gPlayerScriptState; /* provisional: shared player script-state halfword */
/* 0x02000528 */
extern u16 gPlayerActionPending; /* provisional: shared pending player-action counter */
/* 0x0200052C */
extern u16 gPlayerDirectionLatch; /* provisional: direction and action latch halfword */
/* 0x02000534 */
extern u16 gPlayerPassageState; /* provisional: shared player passage/interaction state */
/* 0x0200053C */
extern u16 gPlayerBackgroundMode; /* provisional: background/player action mode halfword */
/* 0x02000548 */
extern u16
    gPlayerInitialSlotValues[2]; /* two script-status values saved before the local player exists */
/* 0x0200054C */
extern void *
    gPlayerPendingActor; /* provisional: pending player interaction/notification actor; independent of slot values */
/* 0x02000550 */
extern u16 gPlayerBackgroundState; /* provisional: shared background/player state halfword */
/* 0x02000554 */
extern u16
    gPlayerAbilityLatched; /* provisional: ability request latch; ROM reader loads an unsigned halfword */
/* 0x02000570 */
extern u16
    gPlayerSolarActionPending; /* provisional: solar action pending flag; separate from four slot words */
/* 0x02000590 */
extern u16
    gPlayerEffectPendingCount; /* provisional: pending player effect count; separate from actor pointers */
/* 0x0200059C */
extern u8 *gPlayerAttackEffect; /* player attack effect singleton */
/* 0x020005A8 */
extern u16 gHudResetFlag; /* provisional: HUD flag initialized to one by Hud_ResetGlobals */
/* 0x020005B4 */
extern u8 gNameEntrySavedState[4]; /* cursor position, selection and per-slot name-entry state */
/* 0x020005DC */
extern s32
    gScreenTextSavedValue; /* provisional: value retained between screen-text menu instances */
/* 0x0200060C */
extern u32
    gSaveStateSignature; /* main-state save signature checked when reading and written before saving */
/* 0x02000700 */
extern u8 *
    gScriptLocalVariables; /* 0x400-byte local script-variable region at selected base plus 0x1E00 */
/* 0x02000704 */
extern u8 *gBackupGameVariables; /* backup game-variable region at selected base plus 0x1A00 */
/* 0x02000708 */
extern u8 *gGameVariables; /* active 0x400-byte game-variable region */
/* 0x0200070C */
extern u8 *gBackupGameState; /* backup main-state region at selected base plus 0xB00 */
/* 0x0201F400 */
extern u32 gHeapEndGuard; /* heap-end guard word expected to equal 0x516B92EE */
/* 0x02037000 */
extern u8 gBgMapWorkBuffer[0x2000]; /* background map work buffer, cleared for 0x2000 bytes */
/* 0x0203C400 */
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
/* 0x0203B400 */
extern u16 gRandomTable[1024]; /* filled by Random_InitValueTable and indexed by gRandomIndex */
/* The table as a sampling loop holds it: set at the top of each pass, read through .samples.
   As a union it is re-set every pass instead of hoisted (tools/scope/README.md, fact 21). */
union RandomTableView {
    const u16 *samples;
};

#endif
