#include "global.h"
#include "script/script_utility.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "scene/areas/area_effect_link.h"
#include "engine/engine_pause.h"
#include "minigames/coffin_bike/coffin_bike.h"

struct LinkMenu {
    u8 unk_00[0x18];
    s16 participant;
    u8 unk_1A[0x53 - 0x1A];
    u8 type;
    u8 unk_54[0x264 - 0x54];
    u32 result;
};
void LinkSession_SetMenuRequest(u32);
s32 GameState_GetResultIndex(void);
u8 LinkParticipant_IsLocallySelected(struct CoffinBike *);
void Sound_StopSong(u32);
void Sound_StopTrackedSong(u32);
void ScenePool_ClampMotionSpeeds(struct CoffinBikeMotion *);
void Actor_ClampMovementSpeed(struct CoffinBikeMotion *);
s32 Div(s32, s32);
s32 Math_Atan2(s32, s32);
void Terrain_ResolveActorMotion(struct CoffinBikeMotion *);
void Actor_UpdateShadowSpriteParts(struct CoffinBikeMotion *, void *);
void CharacterModel_UpdateAccessoryAnimations(void *);
void MotionActor_RunPendingScript(void *);
void MotionActor_RunDelayedScript(void *);
static inline u32 *LinkResult(void)
{
    struct LinkMenu *p = ((struct LinkMenu *)gLinkSession);
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
    if (!((struct LinkMenu *)gLinkSession))
        return -1;
    return ((struct LinkMenu *)gLinkSession)->participant;
}
static inline void SetCallback(struct CoffinBike *p, void (*callback)(void *), u32 zero)
{
    p->active = 1;
    p->update = callback;
    p->timer = zero;
}
// confidence=low: per-frame glove update: runs its entry script, steps the motion, or switches to a delayed callback when far from its origin.
static inline s32 CoffinBike_Participant(struct CoffinBike *actor)
{
    return actor->participant;
}

void sub_08116E44(struct CoffinBike *p)
{
    struct CoffinBikeMotion *self = &p->motion;
    s32 values[2];
    struct ScriptUtilityArguments args;
    s32 h, dx, dz;
    u32 a, b;
    s32 *bias, *forward, *vertical, *offset, *impulseY, *impulseZ;
    s32 *previousX, *previousZ, *lateral;
    if (p->active) {
        if (Engine_PauseBits(0x800)) {
            values[0] = CoffinBike_Participant(p);
            values[1] = *LinkResult();
            if (CoffinBike_Participant(p) == LinkParticipant())
                LinkSession_SetMenuRequest(4);
        } else {
            values[0] = GameState_GetResultIndex();
            values[1] = p->activeFrames;
        }
        args.count = 2;
        args.values = values;
        Script_Call(p->entryScript, &args);
        p->entryScript = 0;
        if (LinkParticipant_IsLocallySelected(p)) {
            Sound_StopSong(0x87);
            Sound_StopSong(0x88);
            Sound_StopSong(0x89);
            Sound_StopSong(0x8A);
            Sound_StopSong(0x8B);
            Sound_StopSong(0x93);
            if (!Engine_PauseBits(0x800)) {
                Sound_StopTrackedSong(0x32);
                Sound_StopTrackedSong(0x34);
                Sound_StopTrackedSong(0x37);
                Sound_StopTrackedSong(0x29);
            }
        }
        p->origin = self->position;
        p->active = 0;
    }
    {
        u32 *flags = &self->contact.flags;
        u32 bit = 1;
        *flags = bit | *flags;
        flags = &self->motionFlags;
        *flags = bit | *flags;
    }
    {
        u8 *flags = &self->damageFlags;
        s32 mask = ~1;
        u32 zero;
        mask &= *flags;
        zero = 0;
        *flags = mask;
        self->damageTimer = zero;
        self->palette.pendingDamage = zero;
    }
    {
        u32 mask = 4;
        mask |= p->flags;
        p->flags = mask;
    }
    if (((self->position.x - p->origin.x) >> 12) <= 127) {
        ScenePool_ClampMotionSpeeds(self);
        h = self->vy + self->unkC0;
        {
            s32 limit = 0xF000;
            forward = &self->vy;
            bias = &self->unkC0;
            if (h > limit)
                h = limit;
        }
        self->y += 0x200;
        {
            s32 decay = self->steps;
            vertical = &self->y;
            if (decay > 0) {
                self->x += self->dx;
                self->dx += Div(-self->dx, self->steps);
                self->dy += Div(-self->dy, self->steps);
                self->dz += Div(-self->dz, self->steps);
                self->steps--;
                offset = &self->x;
                impulseY = &self->dy;
                impulseZ = &self->dz;
            } else {
                Memory_Clear(&self->dx, 16);
                offset = &self->x;
                impulseY = &self->dy;
                impulseZ = &self->dz;
            }
        }
        Actor_ClampMovementSpeed(self);
        self->outX = *offset + h - self->previousPosition.x;
        self->outY = *vertical + *impulseY;
        self->outZ = self->vz + *impulseZ;
        dx = (*offset - self->previousPosition.x) >> 12;
        dz = (self->position.z - self->previousPosition.z) >> 12;
        {
            s32 limit = 0x3000;
            previousX = &self->previousPosition.x;
            lateral = &self->vz;
            previousZ = &self->previousPosition.z;
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
        a = (u32) ~(self->unk3C >> 12) >> 31;
        b = (u32) ~(self->unk38 >> 12) >> 31;
        if (self->unk4 != self->unk8 && (self->unk4 & 0x1000) &&
            !(self->unk8 & 0xF000) && b <= 1 && a == 1) {
            s32 v = ((*forward >> 10) + (*bias >> 8)) >> 1;
            if (v < 4)
                v = 4;
            if (v > 18)
                v = 18;
            *vertical -= v << 9;
            self->flags40 &= ~1;
        }
        p->height = h;
        Actor_UpdateShadowSpriteParts(self, &p->model);
        if (self->flags40 & 2)
            *lateral = 0;
        if (self->flags40 & 1)
            *vertical = 0;
    } else {
        u32 flags = Engine_PauseBits(0x800);
        if (flags)
            SetCallback(p, MotionActor_RunPendingScript, 0);
        else
            SetCallback(p, MotionActor_RunDelayedScript, flags);
        return;
    }
    CharacterModel_UpdateAccessoryAnimations(&p->model);
    p->timer++;
}
