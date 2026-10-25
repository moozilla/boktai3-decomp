#define SlotMotion_SetCallback Ratatosk_SetParticleUpdateCallback
#include "global.h"
struct SlotMotion {
    u16 field0, duration;
    u16 field4;
    u8 state, field7;
    u32 mode, scale, callback, field14;
};
void sub_081FB808(void);
void Memory_Clear(void *, u32);
void Ratatosk_SetParticleUpdateCallback(struct SlotMotion *, u32);
void SlotMotion_Initialize(struct SlotMotion *motion)
{
    Memory_Clear(motion, 0x18);
    Ratatosk_SetParticleUpdateCallback(motion, (u32)sub_081FB808);
    motion->duration = 0x14;
    motion->mode = 5;
    motion->state = 0;
    motion->field14 = 0;
    motion->scale = 0x1000;
}
