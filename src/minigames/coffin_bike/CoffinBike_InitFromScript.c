#include "global.h"
#include "scene/actor/scene_actor_style.h"
#include "link/wireless/scene_pool.h"
#include "engine/input.h"
#include "system/game_state/game_state.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "scene/areas/area_effect_link.h"
#include "engine/engine_pause.h"
#include "scene/collision/scene_actor_damage_visibility.h"
#include "minigames/coffin_bike/coffin_bike.h"

s32 Div(s32, s32);
s32 Math_Atan2(s32, s32);
void Terrain_ResolveActorMotion(struct CoffinBikeMotion *);
void Actor_UpdateShadowSpriteParts(struct CoffinBikeMotion *, void *);

extern u32 gUnk_020004E0[];
void MotionActor_ParsePosition(void *);
void CharacterModel_SetPrimaryAppearance(void *, void *);
void CharacterModel_SetSecondaryAppearance(void *, void *, void *);
void CoffinBike_InitializeStyleModifiers(void *);
void ActorSprite_InitPartDrawings(void *, void *, u32);
void FieldHud_InitializeWidgets(void *, void *, void *, u32, void *);
void CoffinBike_UpdateLaunchSlowdown(void *);
void CoffinBike_UpdateRisingCue(void *);
void CoffinBike_UpdateStopping(void *);
void CharacterPreview_InitializePalette(void *, void *, u32);
void CoffinBike_InitCollisionVolumes(void *);
void SceneTilemap_SetScrollSource(void *);
void CoffinBike_RegisterMessageReceiver(void *);
static inline void CoffinBike_SetCallback(struct CoffinBike *p, void (*fn)(void *), s32 zero)
{
    p->active = 1;
    p->update = fn;
    p->timer = zero;
}
inline static void CoffinBike_SetInitialCallback(struct CoffinBike *p, void (*fn)(void *))
{
    p->active = 1;
    p->update = fn;
    p->timer = 0;
}

static inline void CoffinBike_ReadMode(u32 *result, u32 mask)
{
    *result = (gEngineFlags | gScenePauseFlags) & mask;
}

s32 CoffinBike_InitFromScript(struct CoffinBike *p)
{
    struct CoffinBikeMotion *self = &p->motion;
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
    if (!Engine_PauseBits(0x800))
        p->localScript = Script_GetKeywordValue('g', 0);
    CoffinBike_InitializeStyleModifiers(p);
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
    CoffinBike_ReadMode(&mode, 0x800);
    if (mode) {
        if (p->participant == SceneActorDamage_LocalPlayer()) {
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
        CoffinBike_SetInitialCallback(p, CoffinBike_UpdateLaunchSlowdown);
    } else {
        s32 ready;
        FieldHud_InitializeWidgets(&self->style.initialWord, &p->height,
                                   &self->style.initialHalfword, (u16)self->style.currentHalfword,
                                   &p->activeFrames);
        ready = Script_GetKeywordValue('R', 0);
        if (ready)
            CoffinBike_SetCallback(p, (void (*)(void *))CoffinBike_UpdateStopping, 0);
        else
            CoffinBike_SetCallback(p, CoffinBike_UpdateRisingCue, ready);
    }
    CharacterPreview_InitializePalette(&self->pendingDamage, &p->model, self->style.parameterD);
    CoffinBike_InitCollisionVolumes(p);
    SceneTilemap_SetScrollSource(gUnk_020004E0);
    gParticipantActors[p->participant] = (struct ScenePoolPlayer *)p;
    CoffinBike_RegisterMessageReceiver(p);
    p->input = gPadState + p->participant * 4;
    p->attachmentDesc.resource = p->participant;
    Memory_Clear(&p->offsetX, 8);
    return 0;
}
