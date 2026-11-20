#include "orbit_background.h"
#include "render.h"
#include "gba/io_reg.h"
#include "ram.h"
void *ScriptProc_Start(u16, u16);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, void *);
void Bg_SetPriority(u32, u32);
static inline void Bg_DisableDisplay(u32 layers)
{
    gDispCntShadow &= ~layers;
}

void OrbitBackground_LoadResource(struct OrbitBackground *p)
{
    u32 layers[1];
    u32 specialMode, pending;
    p->resource = ScriptProc_Start(0xC091, 0xB12);
    layers[0] = 0;
    Bg_SetupLayers(3, 2, p->resource, 0, 0, 1, layers);
    Bg_SetPriority(1, 1);
    pending = gPlayerActionPending;
    specialMode = 0;
    if (pending == 1)
        specialMode = 1;
    if (specialMode != 0) {
        Bg_DisableDisplay(DISPCNT_BG1_ON);
        p->specialMode = 1;
    } else {
        if (p->enabled != 0) {
            Bg_EnableDisplay(DISPCNT_BG1_ON);
        } else {
            Bg_DisableDisplay(DISPCNT_BG1_ON);
        }
        p->specialMode = 0;
    }
}
