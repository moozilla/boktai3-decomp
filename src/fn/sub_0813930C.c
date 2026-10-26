#include "global.h"
#include "ram.h"
struct SceneProcSprite {
    u8 data[0x1c];
    u32 flags;
    u8 spriteRest[0x18];
    u8 cleared[8];
    u8 pad40[8];
    u8 animation[0x89];
    u8 active;
    u8 padD2[0xe];
};
struct SceneProcSprites {
    u8 pad[0x1c];
    struct SceneProcSprite entries[4];
};
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void *ScriptProc_Start(u16, u16);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Memory_Clear(void *, u32);
s32 SceneProcSprites_Initialize(struct SceneProcSprites *p)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        void *proc;
        if (!AnimData_Load(p->entries[i].data, 0x409c))
            return -1;
        Sprite_Init(&p->entries[i].flags, p->entries[i].data, 0);
        proc = ScriptProc_Start(0x922e, 0x1c22);
        *(void **)((u8 *)p + 0x398) = proc;
        Anim_SetSequence(p->entries[i].animation, proc, 10, 0, 0);
        AnimData_SelectPaletteBank(p->entries[i].data, 0xe4);
        Memory_Clear(p->entries[i].cleared, 8);
        p->entries[i].active = 0;
        p->entries[i].flags |= 0x11;
        *((u8 *)&p->entries[i].flags + 7) = 1;
    }
    gSceneProcSprites = p;
    return 0;
}
