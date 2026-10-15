// MATCHING_CONCESSION: register-pinning
#include "scene_panel.h"
void ScenePanel_CycleOverlayPalette(struct ScenePanel *panel)
{
    u32 frame;
    {
        u16 *counter = &panel->overlayFrame;
        register u32 old asm("r0") = *counter;
        frame = old + 1;
        *counter = frame;
    }
    {
        register u32 mask asm("r0") = 0xFFFF;
        u32 savedMask;
        register u32 masked asm("r0");
        asm("" : "+r"(mask));
        savedMask = mask;
        masked = frame;
        asm("" : "+r"(savedMask), "+r"(masked));
        masked &= savedMask;
        masked &= 1;
        if (masked) {
            u32 bank = ((u16)frame >> 1) & 3;
            CpuSet((u8 *)panel->overlayPalette + bank * 32, (void *)0x03005120, 0x04000008);
        }
    }
}
