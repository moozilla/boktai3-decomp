#include "global.h"
#include "render.h"
// MATCHING_CONCESSION: register-pinning
#define gDisplayControlShadow gDispCntShadow
#define gPaletteShadow        gUnk_03004FC0

struct S {
    u8 pad[0x18];
    u8 *resource;
};
extern u16 gUnk_03004FC0[];
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void Bg_SetPriority(u32, s32);
u8 *sub_08216E54(u16);
void *ScriptProc_Start(u16, u16);
void sub_08215D40(u16, void *, s32, u16 *, u16);
void *SceneTransition_StartSunScript(void);

void sub_081C4708(struct S *p)
{
    u8 *resource;
    u32 value;
    register void *script asm("r2");
    u16 cmd;
    u16 field;
    void *sun;

    {
        u16 *control = &gDispCntShadow;
        u32 mask = 0xfffff0ff;
        u16 v = *control;
        mask &= v;
        {
            u16 enabled = 0x800;
            *control = mask | enabled;
        }
    }
    resource = sub_08216E54(0xb65e);
    p->resource = resource;
    value = 0;
    Bg_SetupLayers(2, 0, resource, 0, 0, 1, &value);
    Bg_SetPriority(3, 3);
    script = ScriptProc_Start(0xceef, 0x8e32);
    {
        void *s = script;
        cmd = 0xd759;
        field = *(u16 *)((u8 *)s + 0xa);
    }
    sub_08215D40(2, script, 1, &cmd, field);
    sun = SceneTransition_StartSunScript();
    CpuSet((u8 *)sun + 0x14, gUnk_03004FC0, 0x04000080);
}
