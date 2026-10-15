#ifndef GUARD_LEVEL_ADVANCE_DISPLAY_H
#define GUARD_LEVEL_ADVANCE_DISPLAY_H
#include "engine.h"
#include "scene_vec.h"
/* Tentative: level progression plus a second, currently disabled notification. */
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
    u8 unknown4[20];
    union LevelAdvancePosition position;
    u8 unknown20[8];
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
#define LEVEL_ADVANCE_PROGRESS ((struct LevelAdvanceProgress *)gUnk_02000710)
s32 sub_08056F20(struct LevelAdvanceManager *, u32);
s32 sub_08056F30(struct LevelAdvanceManager *);
s32 sub_08056F80(struct LevelAdvanceManager *);
void sub_08056F88(struct LevelAdvanceParticle *);
void sub_0805704C(struct LevelAdvanceParticle *, void *);
void sub_080570BC(struct LevelAdvanceManager *);
void sub_08057114(struct LevelAdvanceManager *, s32);
void sub_08057158(struct LevelAdvanceManager *);
s32 sub_0805732C(struct LevelAdvanceManager *);
s32 sub_080573C8(struct LevelAdvanceManager *);
void sub_080573F8(struct LevelAdvanceManager *);
void sub_08057430(struct LevelAdvanceManager *);
s32 sub_08057480(struct LevelAdvanceManager *);
void sub_08217DE0(struct LevelAdvanceParticle *, void *, u32);
void sub_08217EEC(struct LevelAdvanceParticle *, void *, u32);
void sub_08217EC4(struct LevelAdvanceParticle *, s32, s32);
void sub_08217ECC(struct LevelAdvanceParticle *, u32);
void sub_082151E4(void *, u32);
void sub_082144A4(struct LevelAdvanceSprite *, void *, u32);
void sub_08215284(void *, u32);
void sub_08214514(struct LevelAdvanceSprite *);
s32 Script_GetValueSafe(void);
#endif
