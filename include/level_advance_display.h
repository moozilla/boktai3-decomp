#ifndef GUARD_LEVEL_ADVANCE_DISPLAY_H
#define GUARD_LEVEL_ADVANCE_DISPLAY_H
#include "engine.h"
#include "scene_vec.h"
#include "sprite_drawable_api.h"
#include "player.h"
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
    struct SpriteDrawable sprite;
    s16 offsetX, offsetY, offsetZ, unknown2E;
    s16 velocityX, velocityY, velocityZ, unknown36;
    u16 active, frame;
};
struct LevelAdvanceManager {
    struct Task task;
    struct LevelAdvanceSprite sprite;
    u8 graphics[0x1C];
    u16 kind, active, frame;
    u8 unknown66[12];
    u16 secondaryKind;
    u32 nextThreshold;
    s32 *thresholds;
    const struct SpriteEffectTileResource *particleResource;
    struct LevelAdvanceParticle particles[8];
};
struct LevelAdvanceProgress {
    u8 unknown0[0x40];
    s16 level;
    u16 levelBonus;            /* tentative: incremented by two per advancement */
    u32 unknown44, experience; /* experience interpretation tentative */
};
/* 0x02000120 */
extern struct LevelAdvanceManager *gLevelAdvanceManager;
#define LEVEL_ADVANCE_PROGRESS ((struct LevelAdvanceProgress *)gGameState)
s32 LevelAdvanceDisplay_GetThreshold(struct LevelAdvanceManager *, u32);
s32 LevelAdvanceDisplay_TryAdvanceLevel(struct LevelAdvanceManager *);
s32 LevelAdvanceDisplay_PollSecondaryEvent(struct LevelAdvanceManager *);
void LevelAdvanceDisplay_StartParticle(struct LevelAdvanceParticle *);
void LevelAdvanceDisplay_UpdateParticle(struct LevelAdvanceParticle *,
                                        const struct SpriteEffectTileResource *);
void LevelAdvanceDisplay_StartLevelAnimation(struct LevelAdvanceManager *);
void LevelAdvanceDisplay_StartSecondaryAnimation(struct LevelAdvanceManager *, s32);
void LevelAdvanceDisplay_UpdateAnimation(struct LevelAdvanceManager *);
s32 LevelAdvanceDisplay_UpdateManager(struct LevelAdvanceManager *);
s32 LevelAdvanceDisplay_DestroyManager(struct LevelAdvanceManager *);
void LevelAdvanceDisplay_InitSprite(struct LevelAdvanceManager *);
void LevelAdvanceDisplay_InitParticles(struct LevelAdvanceManager *);
s32 LevelAdvanceDisplay_InitFromScript(struct LevelAdvanceManager *);
s32 Script_GetValueSafe(void);
#endif
