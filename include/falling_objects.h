#ifndef FALLING_OBJECTS_H
#define FALLING_OBJECTS_H
#include "engine.h"

// Tentative: a manager task owning a linked list of objects that drop from above (type 0 and 1
// props, type 2+ variants with cycling frames), land, then get destroyed or bounce away.

struct FallingVec {
    s16 x, y, z, pad;
};

struct FallingSprite {
    u32 flags; // bit 0: hidden, bit 1: scaled, bit 2/3: flip (from the animation)
    u8 pad4[3], priority;
    u8 scaleX, scaleY;
    u8 padA[6];
    u16 frame;
    u8 pad12[10];
    struct FallingVec position;
    u8 tail[8];
};

struct FallingAnim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};

struct FallingObject {
    u16 id;
    u8 type, entered, state, flags; // flags: 1 body collider, 2 attack collider, 4 big
    u16 timer, parameter;
    u8 animationDone, effectActive, animated, remove, frameTimer, frame;
    s16 health;
    u16 speed, ground, pad16;
    struct FallingVec position;
    void *collider, *attack;
    u8 resource[28];
    struct FallingSprite sprite;
    struct FallingAnim anim;
    u8 effect[108];
    struct FallingObject *previous, *next;
};

struct FallingObjectManager {
    u8 task[24];
    void *animation;
    struct FallingObject *head;
};

// Collider contact data of whatever hit the object (callback argument).
struct FallingHit {
    u8 pad0[0x38];
    u32 flags; // bit 3: strong attack (doubles damage)
    u16 pad3C, damage;
};

struct FallingPackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

extern struct FallingObjectManager *gUnk_0200046C;

enum {
    FALLING_STATE_FALLING,
    FALLING_STATE_LANDING,
    FALLING_STATE_WAITING,
    FALLING_STATE_SHATTER,
    FALLING_STATE_BOUNCE_AWAY,
    FALLING_STATE_FINISH
};

static inline void FallingObject_SetFrame(struct FallingSprite *sprite, u16 frame)
{
    sprite->frame = frame;
}

static inline u32 FallingObject_Random(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)gUnk_0203B400);
}

static inline void FallingObject_SetState(struct FallingObject *object, u32 state)
{
    u32 value = state;
    u32 zero = 0;
    object->state = value;
    object->timer = zero;
    object->entered = 1;
}

typedef void (*FallingObjectState)(struct FallingObject *);
extern FallingObjectState gUnk_08604ACC[][4]; // [type][state]

void FallingObject_ClearEffect(void *object); // release the attached effect
u32 FallingObject_Destroy(struct FallingObject *object);
s32 FallingObject_Create(u16 id, s32 type, u32 flags, u32 parameter, u32 speed, u32 health,
                         u32 power, u32 force, struct FallingVec *position);
s32 FallingObject_CreateFromScript(void);
s32 FallingObject_SpawnFinishParticles(struct FallingObject *object);
void FallingObject_ReceiveHit(struct FallingHit *hit, u32 unused, struct FallingObject *object);
void FallingObject_ReceiveAttackHit(void);
u32 FallingObjectManager_AddObject(struct FallingObjectManager *manager,
                                   struct FallingObject *object);
u32 FallingObjectManager_RemoveObject(struct FallingObjectManager *manager,
                                      struct FallingObject *object);
u32 FallingObjectManager_Destroy(struct FallingObjectManager *manager);
s32 FallingObjectManager_Initialize(struct FallingObjectManager *manager, void *unused);
void *FallingObjectManager_Create(void *a, u32 unused);
s32 FallingObjectManager_Update(struct FallingObjectManager *manager);

#endif
