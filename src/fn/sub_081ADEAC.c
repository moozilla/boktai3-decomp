#define Vanargand_InitBackgroundAttachment sub_081ADEAC
#define Vanargand_SetProjectedBackgroundPosition sub_081A64C8
#define gDisplayControlShadow gUnk_03004BD8
#include "global.h"
struct VanargandBackgroundAttachment {
    void *resource;
    void *position;
    void *coordinates;
    u8 offset[8];
    u8 state;
    u8 enabled;
    u16 timer;
};
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
u8 *sub_08216E54(u16);
void sub_081A64C8(s32, void *, s32, s32);
extern u16 gUnk_03004BD8;
static inline void Vanargand_ClearBackgroundOffset(void *destination)
{
    volatile u32 zero = 0;
    CpuSet((const void *)&zero, destination, 0x05000002);
}
void sub_081ADEAC(u8 *owner, struct VanargandBackgroundAttachment *part, void *coordinates)
{
    s32 layer;
    void *resource = sub_08216E54(0xa96a);
    part->resource = resource;
    layer = 2;
    Bg_SetupLayers(3, 0, resource, 0, 0, 1, &layer);
    part->coordinates = coordinates;
    Vanargand_ClearBackgroundOffset(part->offset);
    part->timer = 0;
    part->enabled = 0;
    part->state = 0;
    {
        u16 *control = &gUnk_03004BD8;
        s32 mask = ~0x200;
        *control = mask & *control;
    }
    part->position = owner + 0x918;
    sub_081A64C8(1, part->coordinates, 136, -70);
}
