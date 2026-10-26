#ifndef GUARD_RAM_H
#define GUARD_RAM_H

#include "global.h"

/* RAM objects; each row of symbols/ram.csv with header include/ram.h. */
extern u8 *gPanelSpritePool;
extern u8 *gDirectionalSpritePool;
extern void *gActorManager;
extern s32 gSerialConnectionRole;
extern s32 gSerialTransferStatus;
extern u8 *gScriptRecordTable;
extern u8 *gFieldHud;
extern u8 *gSlotController;
extern struct ScreenDelayParameters gScreenDelayParameters;
extern u8 *gActorEffectManager;
extern u32 gSceneEffectPalettePhase;
extern u32 gSpriteBurstManager;
extern struct MotionTargets *gSceneLinkList;
extern u32 gSpritePairPool;
extern const struct EEPROMConfig *gEEPROMConfig;
extern u8 gAnimPanelSavedSelections[];
extern u32 gEffectRandomTableIndex;
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
extern u16 gSceneSelectionDisplaySlots[];
extern struct Pool *gPursuingActorManager;
extern u32 gSpriteSlots;
extern u32 gScriptedColliderManager;
extern u16 gPlayerNotificationPending;
extern u32 gSegmentDisplay;
extern u32 gActiveRecordMenu;
extern u32 gSerialLatestPacketIndex;
extern s16 gSerialLatestPackets[];
extern struct RegionSpriteManager *gRegionActorPool;
extern u32 gEffectQuad;
extern u8 gSerialInterruptState[];
extern u8 *gCompactSpritePool;
extern u32 gLinkMenuActive;
extern u16 gPlayerCount;
extern u16 gPlayerCueTimer;
extern struct SpriteParticlePool *gSpriteGroup;
extern u32 gSceneStatusIndicatorManager;
extern u8 *gSceneProcSprites;
extern struct FeatherPool *gFadingSpritePool;
extern u8 *gActiveCamera;
extern u32 gTerrainTrapSerial;
extern void *gDirectionBurstEmitter;
extern u32 gNamePlaceholderScript[];
extern u32 gSerialReceivePending;
extern u32 gSerialReceiveIndex;
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
extern u8 *gColliderManager;
extern void *gSceneComposite;
extern u16 gTrackedPlayerId;
extern u32 gActorParticlePool;
extern u32 gGrowthHazardManager;
extern struct FragmentBurstPool *gSpriteGroupPool;

#endif
