#ifndef GUARD_LEVEL_ADVANCE_DISPLAY_H
#define GUARD_LEVEL_ADVANCE_DISPLAY_H
#include "engine.h"
#include "scene_vec.h"
union LevelAdvancePosition {
    struct SceneVec world;
    u32 words[2];
};
struct LevelAdvanceSprite {
    u32 flags;
    u8 unknown4[3], priority, unknown8[8];
    u16 frame;
    u8 unknown12[10];
    union LevelAdvancePosition position;
    u8 unknown24[8];
};
struct LevelAdvanceParticle {
    u32 flags;
    u8 queued, scaleX, scaleY, rotation;
    u8 unknown8[4];
    u8 spriteOffsetX, spriteOffsetY, paletteSlot, mode, status, listIndex;
    u8 unknown12[2];
    u32 frameState;
    union LevelAdvancePosition position;
    struct LevelAdvanceParticle *previous, *next;
    s16 offsetX, offsetY, offsetZ, unknown2E;
    s16 velocityX, velocityY, velocityZ, unknown36;
    u16 active, frame;
};
struct LevelAdvanceManager {
    u8 task[0x18];
    struct LevelAdvanceSprite sprite;
    u8 graphics[0x1C];
    u16 kind, active, frame;
    u8 unknown66[12];
    u16 secondaryKind;
    u32 nextThreshold;
    s32 *thresholds;
    void *particleResource;
    struct LevelAdvanceParticle particles[8];
};
struct LevelAdvanceProgress {
    u8 unknown0[0x40];
    s16 level;
    u16 levelBonus;            /* tentative: incremented by two per advancement */
    u32 unknown44, experience; /* experience interpretation tentative */
};
struct LevelAdvancePlayer {
    u8 unknown0[0x1C];
    u32 mode;
    u8 unknown20[0x10];
    union LevelAdvancePosition position;
};
extern struct LevelAdvancePlayer *gUnk_02000580[4];
extern struct LevelAdvanceManager *gUnk_02000120;
#define LEVEL_ADVANCE_PROGRESS ((struct LevelAdvanceProgress *)gGameState)
s32 LevelAdvanceDisplay_GetThreshold(struct LevelAdvanceManager *, u32);
s32 LevelAdvanceDisplay_TryAdvanceLevel(struct LevelAdvanceManager *);
s32 LevelAdvanceDisplay_PollSecondaryEvent(struct LevelAdvanceManager *);
void LevelAdvanceDisplay_StartParticle(struct LevelAdvanceParticle *);
void LevelAdvanceDisplay_UpdateParticle(struct LevelAdvanceParticle *, void *);
void LevelAdvanceDisplay_StartLevelAnimation(struct LevelAdvanceManager *);
void LevelAdvanceDisplay_StartSecondaryAnimation(struct LevelAdvanceManager *, s32);
void LevelAdvanceDisplay_UpdateAnimation(struct LevelAdvanceManager *);
s32 LevelAdvanceDisplay_UpdateManager(struct LevelAdvanceManager *);
s32 LevelAdvanceDisplay_DestroyManager(struct LevelAdvanceManager *);
void LevelAdvanceDisplay_InitSprite(struct LevelAdvanceManager *);
void LevelAdvanceDisplay_InitParticles(struct LevelAdvanceManager *);
s32 LevelAdvanceDisplay_InitFromScript(struct LevelAdvanceManager *);
void ParticleSprite_Initialize(struct LevelAdvanceParticle *, void *, u32);
void SpriteDrawable_SelectTileFrame(struct LevelAdvanceParticle *, void *, u32);
void Effect_SetOrigin(struct LevelAdvanceParticle *, s32, s32);
void Effect_SetPalette(struct LevelAdvanceParticle *, u32);
s32 Script_GetValueSafe(void);
#endif
