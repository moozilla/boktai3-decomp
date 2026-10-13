#ifndef COLLISION_CONTROLLER_H
#define COLLISION_CONTROLLER_H
#include "global.h"
#include "scene_object.h"

// Tentative: a pool of 16 small hit-detecting props (idle sprite, hit animation, then spawns an
// effect burst) owned by a task, created from script. Each slot embeds a collider
// (struct SceneCollider), a sprite record and an animation player.

struct CollisionVec {
    s16 x, y, z, pad;
};

struct CollisionSprite {
    u32 flags; // bit 0: animation finished, bit 2/3: flip
    u8 pad4[4];
    u16 tile;
    u8 width, height;
    u8 pad0C[12];
    struct CollisionVec position;
    u8 tail[8];
};

struct CollisionAnim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};

// Update modes (CollisionSlot.mode): 0 idle animation, 1 hit animation.
struct CollisionSlot {
    u16 id;
    u8 mode;
    u8 type;
    u8 kind;
    u8 active;
    u8 finished; // set when the slot is done and should be released
    u8 entered;  // set to 1 when a new mode starts
    u8 done;     // animation finished this frame
    u8 pad9;
    u16 timer, delay;
    u16 attack, range, script; // effect parameters from the script
    struct SceneCollider collider;
    struct CollisionSprite sprite;
    struct CollisionAnim anim;
};

struct CollisionGraphics {
    u32 pad;
    u16 base;
};

struct CollisionManager {
    u8 task[24];
    u8 paused;
    u8 pad19[3];
    struct CollisionGraphics *graphics;          // hit sprite graphics
    struct CollisionGraphics *animationGraphics; // idle sprite graphics
    void *hitResource;
    void *idleResource;
    struct CollisionSlot slots[16];
};

/* The other party of a collision callback (guess: its actor); only the hit flag is used here. */
struct CollisionOther {
    u8 pad[0x43];
    u8 hit;
};

struct CollisionScene {
    u8 pad[14];
    u8 mode;
};

struct CollisionActor {
    u8 pad[0x448];
    u32 flags;
    u8 pad44C[0xe8];
    u16 mode, pad536, active;
};

extern struct CollisionManager *gUnk_0200003C;
extern struct CollisionScene gUnk_03005430;
extern u32 gUnk_030053F4;
extern struct CollisionActor *gUnk_02000580;
extern s32 gUnk_02000478;
extern u16 gUnk_02000534, gUnk_02000500;
extern void *gUnk_08604A00[2][3];

void sub_08009E30(void);
struct CollisionSlot *sub_08009E3C(struct CollisionManager *);
s32 CollisionController_InitSlot(struct CollisionManager *, struct CollisionSlot *);
u32 sub_08009F34(struct CollisionManager *, struct CollisionSlot *);
void sub_08009E5C(struct SceneCollider *, struct CollisionOther *, struct CollisionSlot *);
void sub_08009F54(u32, struct CollisionSlot *);
void CollisionController_UpdateIdleAnimation(struct CollisionManager *, struct CollisionSlot *);
void sub_0800A16C(u32, struct CollisionSlot *);
void CollisionController_UpdateHitAnimation(struct CollisionManager *, struct CollisionSlot *);
s32 CollisionController_UpdateAll(struct CollisionManager *);
s32 sub_0800A500(struct CollisionManager *);
s32 CollisionController_InitManager(struct CollisionManager *, void *owner);
struct CollisionManager *sub_0800A62C(void *owner, u32);
s32 CollisionController_ScriptCreate(void);
s32 sub_0800A78C(void);
void sub_0800A7D0(void);

/* Engine helpers. */
void sub_0821FEB4(struct SceneCollider *, u32, u32, u32, u32, struct SceneExtent *,
                  struct SceneExtent *);
void sub_0821FF84(struct SceneCollider *,
                  void (*)(struct SceneCollider *, struct CollisionOther *, struct CollisionSlot *),
                  struct CollisionSlot *);
void sub_0821FE40(struct SceneCollider *);
void sub_0821FE6C(struct SceneCollider *);
void sub_08217DE0(void *, void *, u32);
void sub_08217EEC(void *, void *, u32);
void sub_08217EC4(void *, s32, s32);
void sub_08217EAC(void *);
void sub_08220C8C(void *, void *, u32, u32, u32);
void sub_0822B3BC(u32);
void sub_08249240(struct CollisionManager *, struct CollisionSlot *, void *);
void *sub_08215184(u32);
void *sub_0821A520(u32, u32);
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_08219DD8(void *, u32);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void sub_0821AAD8(void *);
void sub_0821B4C8(void *, s32, s32);
void sub_0800AC44(u32, u32, u32, struct CollisionVec *, struct SceneExtent *, u32, u32);

#endif
