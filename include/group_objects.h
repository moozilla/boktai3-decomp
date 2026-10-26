#ifndef GROUP_OBJECTS_H
#define GROUP_OBJECTS_H
#include "global.h"
#include "engine.h"

// Tentative: a task managing sixteen small board pieces. Command packets queued by the scene
// launch them (sub_0800FC28 and friends), each piece then drifts, animates and bobs.

struct GroupVec {
    s16 x, y, z, pad;
};

// Animation header embedded in each piece (frame table walker in BoardPiece_UpdateAnimation).
struct GroupAnim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};

struct GroupSprite {
    u32 flags;
    u8 pad4[3];
    u8 mode;
    u8 pad8[8];
    u16 frame;
    u8 pad12[10];
    struct GroupVec position;
};

struct GroupResource {
    u8 pad[12];
    void *palette;
    u8 tail[12];
};

struct GroupPiece {
    u8 active, state, done, angle;
    u16 timer;
    s8 direction;
    s8 heading;
    u8 speed, pad9;
    u16 targetY, duration, pade;
    struct GroupVec position, target;
    struct GroupVec anchor, offset; // 0x20, 0x28
    struct GroupAnim anim;
    struct GroupSprite sprite; // 0x40
    u8 pad64[8];
    struct GroupResource resource; // 0x6C
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

struct GroupActor {
    u8 pad0[12];
    struct GroupVec position;
};

struct GroupObjects {
    u8 task[24];
    u32 active, timer;
    void *animation;
    struct GroupPiece pieces[16]; // 0x24
    s8 paletteActive, paletteMask;
    u16 actorId;
    u8 palette[32];
    struct GroupActor *actor;
    struct GroupVec origin;
    struct GroupQueue queue; // 0xCD4
};

extern struct GroupGlobals {
    u8 pad[36], bank;
} *gUnk_0200047C;

extern void (*gUnk_08604B48[])(struct GroupObjects *, struct GroupPiece *, int);

void Palette_InterpolateBanks(void *, void *, void *, int, int);
void Shadow_Destroy(void *);
void Shadow_Initialize(void *, void *, int);
void SceneMessages_RegisterReceiver(void *, int, int);
void SceneMessages_UnregisterReceiver(void *);
void SceneMessages_SetPendingTrigger(void *, void *);
struct GroupActor *SceneBody_GetById(int);
int Mod(int, int);

int BoardPiece_Initialize(struct GroupObjects *, struct GroupPiece *, int);
u32 BoardPiece_Destroy(struct GroupObjects *, struct GroupPiece *, int);
void BoardPiece_UpdateAnimation(struct GroupPiece *);
int BoardPieces_DispatchCommands(struct GroupObjects *);
void BoardPiece_Launch(struct GroupObjects *, struct GroupPiece *, int);
void BoardPiece_Deactivate(struct GroupObjects *, struct GroupPiece *);
int GroupObjects_Update(struct GroupObjects *);
int GroupObjects_Destroy(struct GroupObjects *);
int GroupObjects_Initialize(struct GroupObjects *, int);
void *GroupObjects_CreateTask(void *);

void Vec16_Lerp(void *, void *, void *, int, int);
void BoardPiece_SetFacingAnim(struct GroupObjects *, struct GroupPiece *, int);
int sub_0800FC28(struct GroupObjects *, struct GroupQueue *, struct GroupCommand *);
int sub_0800FE98(struct GroupObjects *, struct GroupQueue *, struct GroupCommand *);
int sub_0801010C(struct GroupObjects *, struct GroupQueue *, struct GroupCommand *);
int sub_08010298(struct GroupObjects *, struct GroupQueue *, struct GroupCommand *);

static inline int randomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)gUnk_0203B400);
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

static inline void polar(struct GroupVec *v, int angle, int radius)
{
    v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gUnk_086149C4[angle & 255] * radius);
}

#endif
