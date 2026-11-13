#include "global.h"
#include "input.h"
#include "render.h"

// Provisional name: TimedPresentation_Update

struct TimedSequence {
    u8 pad[0x18];
    s32 state, timer, exitTimer, minShift, fadeShift, duration, unused;
    void *handle;
};

void Menu_StartFadeOut(s32);

s32 Heap_IsEndGuardValid(void);

void Script_Call(void *, void *);

void Task_Kill(void *);

// Wait for the sequence duration or an eligible skip, then finish its exit fade.

void TimedPresentation_Update(struct TimedSequence *p)
{
    switch (p->state) {
        case 0:
            if (p->timer > p->duration - (1 << p->fadeShift)) {
                p->state = 1;
                Menu_StartFadeOut(p->fadeShift);
            } else if (p->timer > (1 << p->minShift) && Heap_IsEndGuardValid() &&
                       (gPadState[1] & 1)) {
                p->state = 1;
                Menu_StartFadeOut(p->fadeShift);
            }
            break;
        case 1:
            if (p->exitTimer > (1 << p->fadeShift)) {
                Script_Call(p->handle, 0);
                Task_Kill(p);
            }
            p->exitTimer++;
            break;
        case 2:
            break;
    }
    p->timer++;
}

void TimedPresentation_Destroy(void)
{
    u16 *r = &gDispCntShadow;
    u32 m = ~3;
    *r = m & *r;
}

void Bg_ClearTilemap(u32);

void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);

void *ScriptProc_Start(u32, u32);

void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);

void Menu_StartFadeIn(u32);

s32 TimedPresentation_Initialize(struct TimedSequence *sequence)
{
    u32 zero, value;
    void *p, *q;
    sequence->minShift = 4;
    sequence->fadeShift = 4;
    sequence->duration = 300;
    if (Script_SeekToKeyword(0x66)) {
        sequence->minShift = Script_GetValue();
        sequence->fadeShift = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x64))
        sequence->duration = Script_GetValue();
    if (Script_SeekToKeyword(0x74))
        sequence->unused = Script_GetValue();
    if (Script_SeekToKeyword(0x65))
        sequence->handle = (void *)Script_GetValue();
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero = 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    p = ScriptProc_Start(0xC091, 0x52AE);
    value = sequence->unused;
    Bg_SetupLayers(2, 0, p, 0, zero, 1, &value);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    q = (u8 *)ScriptProc_Start(0x92B3, 0xD985) + 0x14;
    CpuSet(q, gBgPaletteBuffer, 0x100);
    Menu_StartFadeIn(sequence->minShift);
    return 0;
}
