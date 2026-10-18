#define SceneActor_SpawnSpriteEffect sub_081825D0
#include "global.h"
#include "contact_actor.h"
struct SceneEffectActor {
    u8 unknown0[0x48];
    struct ContactPosition position;
};
struct SpriteEffectController;
extern const struct ContactPosition gUnk_0824EB7C;
struct SpriteEffectController *SpriteEffectController_Create(const struct ContactPosition *, const struct ContactPosition *, s32, u32, u32, u32, u32, u32, u32, u32, u32, u32, s32, s32, s32, s32, s32, s32);
void sub_081825D0(struct SceneEffectActor *p)
{
    SpriteEffectController_Create(&p->position, &gUnk_0824EB7C, 0x1c1a, 0xd1b8, 0x12, 0, 0x11, 0, 1, 0, 0, 0, 4, 1, 0x400008, 30, 8, 8);
}
