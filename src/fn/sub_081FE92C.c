#define RisingActorConfig_SetCallback SunOrb_SetUpdateCallback
#include "global.h"
struct RisingActorConfig {
    u16 field0, duration, field4;
    u8 state, field7;
    u32 field8;
    u8 mode, fieldD;
    u16 fieldE, field10, field12, field14;
    u8 pad16[2];
    u32 field18;
    void (*callback)(void);
};
void Memory_Clear(void *, u32);
void OrbitEffect_FireVolley(void);
void SunOrb_SetUpdateCallback(struct RisingActorConfig *, void (*)(void));
void RisingActorConfig_Initialize(struct RisingActorConfig *config)
{
    Memory_Clear(config, 0x20);
    SunOrb_SetUpdateCallback(config, OrbitEffect_FireVolley);
    config->field0 = 0;
    config->duration = 600;
    config->field18 = 4;
    config->state = 0;
    config->mode = 2;
    config->fieldD = 0;
    config->fieldE = 30;
    config->field10 = 15;
    config->field12 = 30;
    config->field4 = 1;
    config->field14 = 0;
}
