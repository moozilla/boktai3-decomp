#include "global.h"
#include "ram.h"
struct SceneProcParticle {
    u8 pad00[0x44];
    u8 data[0x1c];
    u32 spriteFlags;
    u8 spriteRest[0x18];
    u8 cleared[8];
    u8 pad84[8];
    u8 animation[0x10];
    void *procedure;
    u8 padA0[0x2a];
    u8 used;
    u8 padCB[0xd];
};
struct SceneProcParticles {
    u8 pad[0x18];
    struct SceneProcParticle entries[6];
    u32 count;
};
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void *ScriptProc_Start(u16, u16);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Memory_Clear(void *, u32);
s32 SceneProcParticles_Initialize(struct SceneProcParticles *p)
{
    s32 i;
    for (i = 0; i < 6; i++) {
        struct SceneProcParticle *s = &p->entries[i];
        void *proc;
        if (!AnimData_Load(s->data, 0x409c))
            return -1;
        Sprite_Init(&s->spriteFlags, s->data, 0);
        proc = ScriptProc_Start(0x922e, 0x1c22);
        s->procedure = proc;
        Anim_SetSequence(s->animation, proc, 7, 0, 0);
        AnimData_SelectPaletteBank(s->data, 0xe4);
        Memory_Clear(s->cleared, 8);
        s->spriteFlags |= 0x11;
        *((u8 *)&s->spriteFlags + 7) = 2;
        s->used = 0;
    }
    p->count = 0;
    gSceneProcParticles = p;
    return 0;
}
