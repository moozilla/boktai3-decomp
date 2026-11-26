#ifndef GROUP_OBJECTS_H
#define GROUP_OBJECTS_H
#include "global.h"
#include "engine/engine.h"
#include "engine/video/display_sequence.h"
#include "scene/actor/scene_actor_components.h"
#include "engine/video/sprite.h"

// Tentative: a task managing sixteen small board pieces. Command packets queued by the scene
// launch them (BatFlockBats_StartStraightAdvance and friends), each piece then drifts, animates and bobs.

struct GroupPiece {
    u8 active, state, done, angle;
    u16 timer;
    s8 direction;
    s8 heading;
    u8 speed, pad9;
    u16 targetY, duration, pade;
    struct ContactPosition position, target;
    struct ContactPosition anchor, offset; // 0x20, 0x28
    struct DisplaySequenceState anim;
    struct DisplaySequenceSprite sprite; // 0x40
    struct SpriteRenderResource resource; // 0x6C
    u8 transform[64];              // 0x88, shadow
};

// Queued command packet (type selects the launch routine).
struct GroupCommand {
    u8 pad0[6];
    u8 type;
    u8 pad7;
    u16 originX, originY, originZ; // 0x8
    s16 radius0;                   // 0xE
    s16 radius1, radius2;          // 0x10, 0x12
    s16 direction;                 // 0x14
    s16 spread;                    // 0x16
};

struct GroupQueue {
    u8 pad[6], count[2];
    struct GroupCommand *commands[2][4];
    u8 tail[12];
};

struct GroupObjects {
    struct Task task;
    u32 active, timer;
    void *animation;
    struct GroupPiece pieces[16]; // 0x24
    s8 paletteActive, paletteMask;
    u16 actorId;
    u8 palette[32];
    struct SceneActorBody *actor;
    struct ContactPosition origin;
    struct GroupQueue queue; // 0xCD4
};

extern struct GroupGlobals {
    u8 pad[36], bank;
} *gSceneEventDispatchState;

extern void (*gUnk_08604B48[])(struct GroupObjects *, struct GroupPiece *, int);

void Palette_InterpolateBanks(void *, void *, void *, int, int);
void Shadow_Destroy(void *);
void Shadow_Initialize(void *, void *, int);
void SceneMessages_RegisterReceiver(void *, int, int);
void SceneMessages_UnregisterReceiver(void *);
void SceneMessages_SetPendingTrigger(void *, void *);
struct SceneActorBody *SceneBody_GetById(u16);
int Mod(int, int);

int BatFlockBat_Initialize(struct GroupObjects *, struct GroupPiece *, int);
u32 BatFlockBat_Destroy(struct GroupObjects *, struct GroupPiece *, int);
void BatFlockBat_UpdateAnimation(struct GroupPiece *);
int BatFlockBats_DispatchCommands(struct GroupObjects *);
void BatFlockBat_Launch(struct GroupObjects *, struct GroupPiece *, int);
void BatFlockBat_Deactivate(struct GroupObjects *, struct GroupPiece *);
int BatFlock_Update(struct GroupObjects *);
int BatFlock_Destroy(struct GroupObjects *);
int BatFlock_Initialize(struct GroupObjects *, int);
void *BatFlock_CreateTask(void *);

void Vec16_Lerp(void *, void *, void *, int, int);
void BatFlockBat_SetFacingAnim(struct GroupObjects *, struct GroupPiece *, int);
int BatFlockBats_StartStraightAdvance(struct GroupObjects *, struct GroupQueue *, struct GroupCommand *);
int BatFlockBats_StartLaunch(struct GroupObjects *, struct GroupQueue *, struct GroupCommand *);
int BatFlockBats_StartConvergence(struct GroupObjects *, struct GroupQueue *, struct GroupCommand *);
int BatFlockBats_StartActorChase(struct GroupObjects *, struct GroupQueue *, struct GroupCommand *);

static inline int randomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)gRandomTable);
}

static inline int randomByte(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u8 *)(offset + (u32)table);
}

static inline int randomWith(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}

static inline int scale(int n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline void polar(struct ContactPosition *v, int angle, int radius)
{
    v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gUnk_086149C4[angle & 255] * radius);
}

#endif
