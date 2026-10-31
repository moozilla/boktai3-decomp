#ifndef ITEM_CONTAINER_H
#define ITEM_CONTAINER_H
#include "global.h"
#include "scene_object.h"
#include "player.h"

/*
 * "Triggers": dropped items (type 0) and treasure containers (type 1) that live in a linked list
 * owned by a TriggerManager task. Each is a 0xBC-byte record with a sprite or an object, an
 * animation, a shadow effect and an update callback that walks it through falling, landing,
 * idle, opening and locked states.
 */

/* Animation state advanced by the shared frame stepper (16 bytes). */
struct TriggerAnim {
    const u16 *data; /* frame table; each entry: frame number << 6 | flip bits | factor */
    u8 flags;        /* bit 2: play backwards, bit 3: hold on the last frame */
    u8 count;
    u8 factor;
    u8 rate;
    u16 frame;
    u16 unused;
    u16 scale;
    u16 timer;
};

struct TriggerGraphics {
    u32 pad0;
    u16 base; /* first tile of the item graphics */
};

/* Sprite used by dropped items (40 bytes). */
struct TriggerSprite {
    u32 flags; /* bit 0: hidden */
    u8 pad4[4];
    u16 tile;
    u8 width, height;
    u8 padC[3];
    u8 mode, priority;
    u8 pad11[7];
    struct SceneVec position;
    u8 pad20[8];
};

/* Draw object used by containers (44 bytes). */
struct TriggerObject {
    u32 flags; /* bit 0: hidden, bit 2: flipped, bit 3: flipped vertically (guess) */
    u8 pad4[12];
    u16 frame;
    u8 pad12[10];
    struct SceneVec position;
    u8 pad24[8];
};

/* The actor that tries to open a container (position at +0xC, slot index at +8). */
struct TriggerOpener {
    u8 pad0[8];
    u8 index;
    u8 pad9[3];
    struct SceneVec position;
};

struct Trigger;
struct TriggerManager;
typedef void (*TriggerUpdate)(struct TriggerManager *, struct Trigger *);

struct Trigger {
    s16 id, kind; /* item id and sub-kind */
    s8 actor;     /* opener slot, -1 when none */
    u8 type;      /* 0 dropped item, 1 container */
    u8 flags;     /* bit 0: delete */
    u8 flip;
    u8 persistent;
    u8 started;
    u8 visible;
    u8 flash;
    u8 tick;
    u8 cooldown;
    u8 armed;
    u8 shake;
    s16 baseX, baseZ;
    u8 option;
    u8 condition; /* container kind: 0, 1 or other */
    u16 sound;    /* sound handle, or region the trigger belongs to */
    u32 timer;
    struct TriggerSprite *sprite;
    struct TriggerObject *object;
    struct SceneAnimation *other;
    struct TriggerAnim *animation;
    struct SceneVec *position;
    u32 startScript, script, arguments[3];
    TriggerUpdate update;
    u8 effect[108]; /* shadow effect */
    struct Trigger *previous, *next;
};

struct TriggerManager {
    u8 task[24];
    struct TriggerGraphics *graphics;
    void *resource;
    u16 kind;
    u8 pad22[2];
    u8 soundDelay;
    u8 pad25[3];
    struct Trigger *head;
    struct Player *actors[4];
    struct SceneVec projected[4]; /* isometric positions of the actors */
};

struct TriggerScriptArgs {
    u32 count : 16;
    u32 unused : 16;
    s32 *values;
};

/* 0x02000030 */
extern struct TriggerManager *gTriggerManager;

s32 Trigger_RunScript(struct Trigger *, u32 script, s32 event);
u32 TriggerManager_InsertTrigger(struct Trigger *);
u32 TriggerManager_RemoveTrigger(struct Trigger *);
u32 Trigger_Destroy(struct Trigger *);
void Trigger_UpdateShake(struct Trigger *);
s32 Trigger_UpdateProximity(struct TriggerManager *, struct Trigger *);
s32 Trigger_TryOpenNearby(struct TriggerOpener *);
void Trigger_UpdateFalling(struct TriggerManager *, struct Trigger *);
void Trigger_UpdateLanding(struct TriggerManager *, struct Trigger *);
void Trigger_UpdateBobbingAnimation(struct TriggerManager *, struct Trigger *);
void Trigger_UpdateGrounded(struct TriggerManager *, struct Trigger *);
void Trigger_UpdateDropTimer(struct TriggerManager *, struct Trigger *);
void Trigger_UpdateIdleAnimation(struct TriggerManager *, struct Trigger *);
void Trigger_UpdateOpeningAnimation(struct TriggerManager *, struct Trigger *);
void Trigger_UpdateActivation(struct TriggerManager *, struct Trigger *);
void Trigger_UpdateLockedPause(struct TriggerManager *, struct Trigger *);
void Trigger_UpdateLockedRecovery(struct TriggerManager *, struct Trigger *);
s32 TriggerManager_Update(struct TriggerManager *);
u32 TriggerManager_DestroyManager(struct TriggerManager *);
s32 TriggerManager_Initialize(struct TriggerManager *, void *);
void *TriggerManager_InitManager(void *, void *);
struct TriggerManager *TriggerManager_GetOrCreate(void);
s32 Trigger_CreateDrop(struct SceneVec *position, s32 id, s32 kind, u32 script, u32 *arguments,
                       s32 sound, s32 persistent, s32 option);
s32 Trigger_CreateDropFromScript(void);
s32 Trigger_CreateContainer(struct SceneVec *position, s32 id, s32 kind, s32 flip, s32 bob,
                            u32 startScript, u32 script, u32 *arguments, s32 sound, s32 hidden,
                            s32 condition);
s32 Trigger_CreateContainerFromScript(void);
s32 TriggerManager_DestroyAllOfType(u32 type);
u32 TriggerManager_DestroyAllOfTypeFromScript(void);
s32 Trigger_MoveDropsByItem(struct SceneVec *position, s32 id, s32 kind);
u32 Trigger_MoveDropsByItemFromScript(void);

#endif
