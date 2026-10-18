#define ActorEmitter_UpdateAnimation sub_0811EFF0
#define ActorEmitter_CanEmit Modifier_IsFirstSourceReady
#include "global.h"
struct EmitterVector { s32 x, y, z; u32 :32; };
struct EmitterActor { u8 pad0[0x38]; s32 height; };
struct EmitterOwner {
    u8 pad0[0x28];
    struct EmitterVector position;
    u8 pad38[0x18];
    s32 height;
    u8 pad54[0x1C];
    s16 energy;
    u8 pad72[0x25A];
    u8 profile[0xC];
    u8 pad2D8[0x610];
    s32 velocity;
    u8 pad8EC[0xC];
    s8 kind;
};
struct ActorEmitter {
    u8 pad0[8];
    u16 id;
    u8 padA[6];
    u16 duration;
    u8 pad12[2];
    u8 collision[0x40];
    u8 unknown54, entered;
    u8 pad56[2];
    u16 timer;
    u8 pad5A[2];
    u16 animation;
    u8 pad5E[2];
    struct EmitterOwner *owner;
};
u8 Modifier_IsFirstSourceReady(struct ActorEmitter *);
s32 sub_0811EAF8(void *, u32, s32);
s32 sub_08114928(struct EmitterVector *, s32, struct EmitterVector *, void *, s32, s32, s32, s32);
void sub_0811EE68(void *, struct ActorEmitter *, s32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);
u8 sub_08116C74(void *);
void Sound_PlaySong(u32);
void Character_GetHeightAdjustedPosition(void *, s16 *);
void sub_0811B164(void *, s32, s32, s32);
void sub_0811B2FC(void *);
s32 sub_0811EFF0(struct ActorEmitter *emitter)
{
    struct EmitterOwner *owner;
    struct EmitterActor *actor;
    s32 cost;
    s16 position[3];
    s16 *pos;
    if (!Modifier_IsFirstSourceReady(emitter)) goto fail;
    owner = emitter->owner;
    actor = (struct EmitterActor *)((u8 *)owner + 0x18);
    cost = sub_0811EAF8(actor, emitter->id, 9);
    if (owner->energy >= cost) goto active;
fail:
    return 0;
active:
    if (emitter->entered) {
        Anim_SetKeyframes((u8 *)owner + 0x670, (u8 *)owner + 0x7F0, emitter->animation, 1, 0);
        if (sub_08116C74(owner)) Sound_PlaySong(0x93);
        emitter->entered = 0;
    }
    sub_0811EE68(actor, emitter, cost);
    Anim_Step((u8 *)owner + 0x670, (u8 *)owner + 0x7F0);
    pos = position;
    Character_GetHeightAdjustedPosition(actor, pos);
    emitter = (struct ActorEmitter *)emitter->collision;
    sub_0811B164(emitter, position[0], pos[1], pos[2]);
    sub_0811B2FC(emitter);
    return 1;
}
