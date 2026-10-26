#include "global.h"
#include "engine.h"
#include "ram.h"
struct DrawFlags {
    u8 pad0[0x16];
    u16 flags;
};
struct A8CBObject {
    u8 pad0[0x10];
    u8 block10[0x3B];
    u8 field4B;
    u8 pad4C[0x1C];
    u8 block68[0x48];
    u8 blockB0[0x50C];
    u8 block5BC[0x3C];
    u8 anim[0x20];
    u8 block618[0x234];
    u32 field84C;
    u8 active;
    u8 pad851[3];
    void (*update)(void);
    u8 pad858[0x34];
    struct DrawFlags draw;
    u8 pad8A4[6];
    u8 field8AA;
    u8 pad8AB[9];
    u32 field8B4;
};
void *RegistryA8CB_EnsureManager(void);
struct A8CBObject *RegistryA8CB_FindAvailableEntry(void *manager);
void ActorScript_ParseParams(struct A8CBObject *);
void ActorSprite_InitPartDrawings(struct A8CBObject *, void *, s32);
void CharacterPreview_InitializePalette(void *, void *, u8);
void SceneSlot_RegisterObject(void *);
void ActorCollision_InitBounds(struct A8CBObject *);
void RegistryA8CB_RegisterMessageReceiver(struct A8CBObject *);
void Modifier_InitDrainState(void *, void *, s32, s32);
void ActorPhysics_UpdateFallMotion(void);
/* Registry routine A8CB: takes an object from the manager at 0x0200020C
   (created on demand), reads its options (ActorScript_ParseParams: -n, -p, -s, -P),
   loads sprite 0x3D99 and starts it with update ActorPhysics_UpdateFallMotion (tentative). */
void RegistryA8CB_CreateFromScript(void)
{
    void *manager = gActorManager;
    struct A8CBObject *e;
    void *anim;
    u16 one;
    if (!manager) {
        manager = RegistryA8CB_EnsureManager();
        if (!manager)
            return;
    }
    e = RegistryA8CB_FindAvailableEntry(manager);
    if (!e)
        return;
    Memory_Clear(e->block5BC, 16);
    e->field8AA = 0xFF;
    ActorScript_ParseParams(e);
    anim = e->anim;
    ActorSprite_InitPartDrawings(e, anim, 0x3D99);
    CharacterPreview_InitializePalette(e->block68, anim, e->field4B);
    SceneSlot_RegisterObject(e->block10);
    ActorCollision_InitBounds(e);
    {
        void (*update)(void) = ActorPhysics_UpdateFallMotion;
        u8 *active = &e->active;
        u32 zero = 0;
        one = 1;
        *active = one;
        e->update = update;
        e->field84C = zero;
    }
    RegistryA8CB_RegisterMessageReceiver(e);
    {
        struct DrawFlags *draw = &e->draw;
        draw->flags = one | draw->flags;
    }
    Modifier_InitDrainState(e->blockB0, e->block618, 0, 0);
    e->field8B4 = one;
}
