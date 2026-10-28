#include "global.h"
#include "ram.h"

struct Position {
    s32 x, y, z;
    u32 unused;
};
struct Motion {
    u8 unk_00[4];
    u32 terrain, previousTerrain;
    u8 unk_0C[4];
    struct Position position;
    s32 outX, outY, outZ;
    u8 unk_2C[0x38 - 0x2C];
    s32 heightA, heightB;
    u32 collision;
    u8 unk_44[0x68 - 0x44];
    u32 pending;
    u8 unk_6C[0xC0 - 0x6C];
    s32 bias;
    u8 unk_C4[0x52C - 0xC4];
    u32 flags52C;
    u8 unk_530[0x56C - 0x530];
    u32 flags56C;
    u8 unk_570[10];
    u8 mode;
    u8 unk_57B[0x5AC - 0x57B];
    s32 forward, vertical, lateral;
    u32 unk_5B8;
    s32 previousX, previousY, previousZ;
    u32 unk_5C8;
    s32 impulseX, impulseY, impulseZ, impulseW;
    s32 offsetX;
    u8 unk_5E0[12];
    s8 decay;
    u8 unk_5ED;
    u8 flags;
    u8 unk_5EF;
    u16 angle;
    u8 unk_5F2[6];
};
struct Glove {
    u8 unk_00[0x18];
    struct Motion motion;
    u8 model[0x864 - 0x610];
    u32 timer;
    u8 entered;
    u8 unk_869[3];
    void (*callback)(void *);
    u8 unk_870[0x8A4 - 0x870];
    struct Position origin;
    u8 unk_8B4[0x8E8 - 0x8B4];
    s32 height;
    u8 unk_8EC[8];
    u16 flags;
    u8 unk_8F6[2];
    s8 participant;
    u8 unk_8F9[7];
    u32 result;
    u32 unk_904;
    u32 script;
};
struct LinkMenu {
    u8 unk_00[0x18];
    s16 participant;
    u8 unk_1A[0x53 - 0x1A];
    u8 type;
    u8 unk_54[0x264 - 0x54];
    u32 result;
};
struct ScriptArgs {
    u32 count : 16;
    u32 : 16;
    s32 *values;
};
extern struct LinkMenu *gUnk_020004B4;
void Registry1745_SetMenuRequest(u32);
s32 GameState_GetResultIndex(void);
void Script_Call(u32, void *);
u8 LinkParticipant_IsLocallySelected(struct Glove *);
void Sound_StopSong(u32);
void Sound_StopTrackedSong(u32);
void ScenePool_ClampMotionSpeeds(struct Motion *);
void Actor_ClampMovementSpeed(struct Motion *);
s32 Div(s32, s32);
void Memory_Clear(void *, u32);
s32 Math_Atan2(s32, s32);
void Terrain_ResolveActorMotion(struct Motion *);
void Actor_UpdateShadowSpriteParts(struct Motion *, void *);
void CharacterModel_UpdateAccessoryAnimations(void *);
void MotionActor_RunPendingScript(void *);
void MotionActor_RunDelayedScript(void *);
static inline void ReadParticipant(struct Glove *p, s32 *out, s8 **ptr)
{
    s8 *participant = &p->participant;
    *out = *participant;
    *ptr = participant;
}
static inline void StoreValue(s32 *out, s32 value)
{
    *out = value;
}
static inline u32 *LinkResult(void)
{
    struct LinkMenu *p = gUnk_020004B4;
    u32 *result;
    if (p) {
        u32 type = p->type;
        result = &p->result;
        if (type != 1)
            result = 0;
    } else
        result = 0;
    return result;
}
static inline s32 LinkParticipant(void)
{
    if (!gUnk_020004B4)
        return -1;
    return gUnk_020004B4->participant;
}
static inline void AddFlags(u32 mask, u16 *flags)
{
    *flags = mask | *flags;
}
static inline u32 LinkFlags(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}
static inline void SetCallback(struct Glove *p, void (*callback)(void *), u32 zero)
{
    p->entered = 1;
    p->callback = callback;
    p->timer = zero;
}
#ifdef NONMATCHING
// fuzzy: 86.8%
// scope shape 7 (differences once low registers are set aside)
// confidence=low: per-frame glove update: runs its entry script, steps the motion, or switches to a delayed callback when far from its origin.
void sub_08116E44(struct Glove *p)
{
    struct Motion *self = &p->motion;
    s32 values[2];
    struct ScriptArgs args;
    s32 h, dx, dz;
    u32 a, b;
    s32 *bias, *forward, *vertical, *offset, *impulseY, *impulseZ;
    s32 *previousX, *previousZ, *lateral;
    if (p->entered) {
        if (LinkFlags(0x800)) {
            s8 *participant;
            ReadParticipant(p, &values[0], &participant);
            values[1] = *LinkResult();
            if (*participant == LinkParticipant())
                Registry1745_SetMenuRequest(4);
        } else {
            values[0] = GameState_GetResultIndex();
            values[1] = p->result;
        }
        args.count = 2;
        args.values = values;
        Script_Call(p->script, &args);
        p->script = 0;
        if (LinkParticipant_IsLocallySelected(p)) {
            Sound_StopSong(0x87);
            Sound_StopSong(0x88);
            Sound_StopSong(0x89);
            Sound_StopSong(0x8A);
            Sound_StopSong(0x8B);
            Sound_StopSong(0x93);
            if (!LinkFlags(0x800)) {
                Sound_StopTrackedSong(0x32);
                Sound_StopTrackedSong(0x34);
                Sound_StopTrackedSong(0x37);
                Sound_StopTrackedSong(0x29);
            }
        }
        p->origin = self->position;
        p->entered = 0;
    }
    {
        u32 *flags = &self->flags52C;
        u32 bit = 1;
        *flags = bit | *flags;
        flags = &self->flags56C;
        *flags = bit | *flags;
    }
    {
        u8 *flags = &self->flags;
        s32 mask = ~1;
        u32 zero;
        mask &= *flags;
        zero = 0;
        *flags = mask;
        self->mode = zero;
        self->pending = zero;
    }
    {
        u32 mask = 4;
        mask |= p->flags;
        p->flags = mask;
    }
    if (((self->position.x - p->origin.x) >> 12) <= 127) {
        ScenePool_ClampMotionSpeeds(self);
        h = self->forward + self->bias;
        {
            s32 limit = 0xF000;
            forward = &self->forward;
            bias = &self->bias;
            if (h > limit)
                h = limit;
        }
        self->vertical += 0x200;
        {
            s32 decay = self->decay;
            vertical = &self->vertical;
            if (decay > 0) {
                self->offsetX += self->impulseX;
                self->impulseX += Div(-self->impulseX, self->decay);
                self->impulseY += Div(-self->impulseY, self->decay);
                self->impulseZ += Div(-self->impulseZ, self->decay);
                self->decay--;
                offset = &self->offsetX;
                impulseY = &self->impulseY;
                impulseZ = &self->impulseZ;
            } else {
                Memory_Clear(&self->impulseX, 16);
                offset = &self->offsetX;
                impulseY = &self->impulseY;
                impulseZ = &self->impulseZ;
            }
        }
        Actor_ClampMovementSpeed(self);
        self->outX = *offset + h - self->previousX;
        self->outY = *vertical + *impulseY;
        self->outZ = self->lateral + *impulseZ;
        dx = (*offset - self->previousX) >> 12;
        dz = (self->position.z - self->previousZ) >> 12;
        {
            s32 limit = 0x3000;
            previousX = &self->previousX;
            lateral = &self->lateral;
            previousZ = &self->previousZ;
            if (h > limit)
                dx = 0x80;
        }
        if (dx == 0 && dz == 0)
            self->angle = 0xFFFF;
        else
            self->angle = (Math_Atan2(dx, dz) >> 5) << 5;
        *previousX = *offset;
        *previousZ = self->position.z;
        Terrain_ResolveActorMotion(self);
        a = (u32) ~(self->heightB >> 12) >> 31;
        b = (u32) ~(self->heightA >> 12) >> 31;
        if (self->terrain != self->previousTerrain && (self->terrain & 0x1000) &&
            !(self->previousTerrain & 0xF000) && b <= 1 && a == 1) {
            s32 v = ((*forward >> 10) + (*bias >> 8)) >> 1;
            if (v < 4)
                v = 4;
            if (v > 18)
                v = 18;
            *vertical -= v << 9;
            self->collision &= ~1;
        }
        p->height = h;
        Actor_UpdateShadowSpriteParts(self, p->model);
        if (self->collision & 2)
            *lateral = 0;
        if (self->collision & 1)
            *vertical = 0;
    } else {
        u32 flags = LinkFlags(0x800);
        if (flags)
            SetCallback(p, MotionActor_RunPendingScript, 0);
        else
            SetCallback(p, MotionActor_RunDelayedScript, flags);
        return;
    }
    CharacterModel_UpdateAccessoryAnimations(p->model);
    p->timer++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08116E44);
#endif
