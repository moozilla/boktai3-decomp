#ifndef EFFECT_EMITTER_H
#define EFFECT_EMITTER_H
#include "global.h"
#include "scene_object.h"

// Tentative: a hovering, bobbing prop with a shadow that can be hit; when its health runs out it
// bursts into an effect (sub_0800AFE8), see homing_effect.h.

struct EmitterVec {
    s16 x, y, z, pad;
};

struct EmitterSprite {
    u32 flags; // bit 0: hidden
    u8 pad4[12];
    u16 frame;
    u8 pad12[10];
    struct EmitterVec position;
    u8 tail[8];
};

// Collider contact data of the thing that hit us (callback argument).
struct EmitterAttack {
    u8 pad0[0x38];
    u32 flags; // bit 6: strong attack
    u16 pad3C, damage;
};

struct EffectEmitter {
    u8 task[24];
    u16 id, pad1A;
    u8 pad1C[4];
    void *scene; // the scene (owner) this emitter belongs to; hidden when not current
    s16 health;
    u16 maximum, cooldown, phase, timer, flash;
    u8 rising, pad31;
    s16 baseY, offset; // vertical bobbing around baseY
    u16 count, kind;   // effect parameters
    u16 pad3A;
    struct SceneAnimation resource;
    struct EmitterSprite sprite;
    u8 shadow[64];
    struct SceneCollider collider;
};

struct EmitterWorld {
    u8 pad[0x5a0];
    void *scene;
};

extern struct EmitterWorld *gUnk_02000710;
extern u16 gUnk_08604AB4[];

void EffectEmitter_ReceiveHit(struct EmitterAttack *, void *, struct EffectEmitter *);
s32 EffectEmitter_Update(struct EffectEmitter *);
u32 sub_0800C630(struct EffectEmitter *);
s32 EffectEmitter_Initialize(struct EffectEmitter *, u32, u32);
void *sub_0800C7AC(u32, u32);

/* Engine helpers. */
s32 sub_0800AFE8(s32, u32, s32, struct EmitterVec *, void *, s32, s32);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void *sub_0822C034(void *);
void sub_0821FEB4(struct SceneCollider *, u32, u32, u32, u32, struct SceneExtent *,
                  struct SceneExtent *);
void sub_0821FF84(struct SceneCollider *,
                  void (*)(struct EmitterAttack *, void *, struct EffectEmitter *),
                  struct EffectEmitter *);
void sub_0821FE40(struct SceneCollider *);
void sub_0821FE6C(struct SceneCollider *);
void sub_082151E4(void *, u32);
void sub_082144A4(void *, void *, u32);
void sub_08214514(void *);
void sub_08013440(void *);
void sub_08013424(void *);
void sub_08013684(void *);
void Shadow_Initialize(void *, void *, u32);
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void *, void *);
void sub_0821A0C0(void *);

#endif
