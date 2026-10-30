#include "global.h"
#include "scene_actor_style.h"
#include "scene_pool.h"
#include "input.h"
#include "game_state.h"
#include "ram.h"
#include "engine_globals.h"
#include "area_effect_link.h"

struct GloveModel;
struct Glove;
union GloveAnimationKey {
    u32 word;
    u16 half[2];
};

struct GloveMotion {
    u8 pad0[4];
    u32 unk4;
    u32 unk8;
    u32 boundsFlags;
    struct ScenePoolVec32 position;
    s32 outX, outY, outZ; /* 0x20 */
    u8 pad2C[0x38 - 0x2C];
    s32 unk38;
    s32 unk3C;
    u32 flags40;
    u8 color[4];
    struct SceneActorStyle style;
    u8 pad64[4];
    u32 pendingDamage;
    u8 pad6C[0x7E - 0x6C];
    u16 recoveryValue;
    struct GloveModel *parts;
    u8 pad84[0xC0 - 0x84];
    s32 unkC0;
    u8 padC4[4];
    u32 secondaryBias;
    u8 padCC[0x2B0 - 0xCC];
    u32 primaryCallback;
    u8 pad2B4, primaryKind;
    u8 pad2B6[8];
    u16 primaryCharge;
    u8 pad2C0[0x305 - 0x2C0];
    u8 primarySpriteRefresh, primaryLatched;
    u8 actionActive;
    u16 actionCooldown;
    u8 pad30A[4];
    u16 primaryKeyframes;
    u8 pad310[4];
    u32 secondaryCallback;
    u8 pad318;
    u8 actionKind;
    u8 pad31A[0x324 - 0x31A];
    u8 secondaryEnabled;
    s8 secondarySpriteSlot;
    u16 recoveryTimer, cooldown;
    u8 secondaryRelease, secondaryLatched;
    u8 pad32C[0x524 - 0x32C];
    union GloveAnimationKey animation;
    u8 pad528[4];
    u8 contactIndex[0x40];
    u32 motionFlags;
    u8 pad570[10];
    u8 damageTimer;
    u8 pad57B[0x59E - 0x57B];
    u16 spatialTimer;
    u8 pad5A0[0x5AC - 0x5A0];
    s32 vy; /* 0x5AC */
    s32 y;  /* 0x5B0 */
    s32 vz; /* 0x5B4 */
    u8 pad5B8[4];
    struct ScenePoolVec32 previousPosition;
    s32 dx; /* 0x5CC */
    s32 dy; /* 0x5D0 */
    s32 dz; /* 0x5D4 */
    u8 pad5D8[4];
    s32 x; /* 0x5DC */
    u8 pad5E0[0x5EC - 0x5E0];
    s8 steps; /* 0x5EC */
    u8 pad5ED;
    s8 damageFlags;
    u8 pad5EF;
    u16 angle; /* 0x5F0 */
    u8 pad5F2[2];
    struct Glove *owner;
};

struct GloveModelPart {
    u8 pad0[8];
    u32 flags;
    u8 padC[0x48 - 0xC];
    void *palette;
    u8 pad4C[0x60 - 0x4C];
};
struct GloveModel {
    struct GloveModelPart parts[5];
    u8 primaryAnimation[0x30];
    u8 palette[0x40];
};

struct Glove {
    u8 pad0[0x18];
    struct GloveMotion motion;
    struct GloveModel model;
    s8 marker, markerSlot;
    u8 pad862[2];
    u32 timer;
    u8 active;
    u8 pad869[3];
    void (*update)(void *);
    u8 pad870[0x8A4 - 0x870];
    struct ScenePoolVec32 origin, scroll;
    u16 *input;
    u8 pad8C8[0x8E4 - 0x8C8];
    u32 previousInput;
    s32 height;
    u8 pad8EC[8];
    u16 flags, blinkTimer;
    s8 participant;
    u8 pad8F9[3];
    s32 resourceFraction;
    u32 activeFrames;
    u32 localScript, entryScript, eventScript, nextScript;
    s32 launchSpeed;
    u8 launchPhase;
    u8 pad919[3];
    u32 participantIndex;
    u8 pad920[0x30];
    u32 pending[2];
};
s32 Div(s32, s32);
s32 Math_Atan2(s32, s32);
void Terrain_ResolveActorMotion(struct GloveMotion *);
void Actor_UpdateShadowSpriteParts(struct GloveMotion *, void *);

extern u32 gUnk_020004E0[];
void MotionActor_ParsePosition(void *);
void CharacterModel_SetPrimaryAppearance(void *, void *);
void CharacterModel_SetSecondaryAppearance(void *, void *, void *);
void Glove_InitializeStyleModifiers(void *);
void ActorSprite_InitPartDrawings(void *, void *, u32);
void FieldHud_InitializeWidgets(void *, void *, void *, u32, void *);
void sub_081175F8(void *);
void sub_081172C4(void *);
void Glove_UpdateStopping(void *);
void CharacterPreview_InitializePalette(void *, void *, u32);
void ActorRecoil_InitCollisionVolumes(void *);
void SceneTilemap_SetScrollSource(void *);
void Glove_RegisterMessageReceiver(void *);
static inline u32 Glove_ModeMask(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}
static inline s32 Glove_LinkParticipant(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->localPlayer;
}
static inline void Glove_SetCallback(struct Glove *p, void (*fn)(void *), s32 zero)
{
    p->active = 1;
    p->update = fn;
    p->timer = zero;
}
inline static void Glove_SetInitialCallback(struct Glove *p, void (*fn)(void *))
{
    p->active = 1;
    p->update = fn;
    p->timer = 0;
}

static inline void Glove_ReadMode(u32 *result, u32 mask)
{
    *result = (gEngineFlags | gScenePauseFlags) & mask;
}

s32 Glove_InitFromScript(struct Glove *p)
{
    struct GloveMotion *self = &p->motion;
    s32 h;
    s32 dx;
    s32 dz;
    u32 mode;
    u32 a;
    u32 b;
    Memory_Clear(&p->motion.previousPosition, 0x10);
    p->activeFrames = 0;
    p->resourceFraction = 0;
    p->motion.owner = p;
    MotionActor_ParsePosition(p);
    Memory_Clear(&p->scroll, 0x10);
    Memory_Clear(gUnk_020004E0, 0x10);
    CharacterModel_SetPrimaryAppearance(&p->model, &p->motion.position);
    CharacterModel_SetSecondaryAppearance(&p->model, &p->motion.position, &p->motion.unk38);
    p->participant = Script_GetKeywordValue('i', 0);
    p->entryScript = Script_GetKeywordValue('G', 0);
    p->eventScript = Script_GetKeywordValue('e', 0);
    p->nextScript = Script_GetKeywordValue('n', 0);
    if (!Glove_ModeMask(0x800))
        p->localScript = Script_GetKeywordValue('g', 0);
    Glove_InitializeStyleModifiers(p);
    ActorSprite_InitPartDrawings(self, &p->model, 0x3d99);
    if ((p->motion.x) == 0x20000)
        self->x = -0x40000;
    h = self->vy + self->unkC0;
    self->y += 0x200;
    if (self->steps > 0) {
        self->x += self->dx;
        self->dx += Div(-self->dx, self->steps);
        self->dy += Div(-self->dy, self->steps);
        self->dz += Div(-self->dz, self->steps);
        self->steps--;
    } else {
        Memory_Clear(&self->dx, 0x10);
    }
    {
        s32 *pprevX;
        s32 *pprevZ;
        s32 *px;
        s32 *py;
        s32 lim;
        self->outX = (h + self->x) - self->previousPosition.x;
        self->outY = self->y + self->dy;
        self->outZ = self->vz + self->dz;
        dx = (self->x - self->previousPosition.x) >> 12;
        dz = (self->position.z - self->previousPosition.z) >> 12;
        lim = 0x3000;
        px = &self->x;
        pprevX = &self->previousPosition.x;
        py = &self->y;
        pprevZ = &self->previousPosition.z;
        if (h > lim)
            dx = 0x80;
        if ((dx == 0) && (dz == 0))
            self->angle = 0xFFFF;
        else
            self->angle = (Math_Atan2(dx, dz) >> 5) << 5;
        *pprevX = *px;
        *pprevZ = self->position.z;
        Terrain_ResolveActorMotion(self);
        a = ((u32)(~(self->unk3C >> 12))) >> 31;
        b = ((u32)(~(self->unk38 >> 12))) >> 31;
        if (((((self->unk4 != self->unk8) && (self->unk4 & 0x1000)) && (!(self->unk8 & 0xF000))) &&
             (b <= 1)) &&
            (a == 1)) {
            s32 v = ((self->vy >> 10) + (self->unkC0 >> 8)) >> 1;
            if (v < 4)
                v = 4;
            if (v > 18)
                v = 18;
            *py -= v << 9;
            self->flags40 &= ~1;
        }
        Actor_UpdateShadowSpriteParts(self, &p->model);
    }
    Glove_ReadMode(&mode, 0x800);
    if (mode) {
        if (p->participant == Glove_LinkParticipant()) {
            struct AreaEffectLinkState *ref = gLinkSession;
            void *optional;
            if (ref) {
                u32 kind = ref->unavailable;
                optional = ref->unknown260;
                if (kind != 1)
                    optional = 0;
            } else
                optional = 0;
            FieldHud_InitializeWidgets(&self->style.initialWord, &p->height,
                                       &self->style.initialHalfword,
                                       (u16)self->style.currentHalfword, optional);
        }
        Glove_SetInitialCallback(p, sub_081175F8);
    } else {
        s32 ready;
        FieldHud_InitializeWidgets(&self->style.initialWord, &p->height,
                                   &self->style.initialHalfword, (u16)self->style.currentHalfword,
                                   &p->activeFrames);
        ready = Script_GetKeywordValue('R', 0);
        if (ready)
            Glove_SetCallback(p, (void (*)(void *))Glove_UpdateStopping, 0);
        else
            Glove_SetCallback(p, sub_081172C4, ready);
    }
    CharacterPreview_InitializePalette(&self->pendingDamage, &p->model, self->style.parameterD);
    ActorRecoil_InitCollisionVolumes(p);
    SceneTilemap_SetScrollSource(gUnk_020004E0);
    gParticipantActors[p->participant] = (struct ScenePoolPlayer *)p;
    Glove_RegisterMessageReceiver(p);
    p->input = gPadState + p->participant * 4;
    p->participantIndex = p->participant;
    Memory_Clear(p->pending, 8);
    return 0;
}
