#define SceneEffect_EmitPositionPattern sub_080DA810
#define SceneEffect_CreateParticle ExplosionEffect_CreateDelayed
#define gSceneEffectPatternArguments          gUnk_086061C0
#define gSceneEffectFourOffsetsX        gUnk_086061C4
#define gSceneEffectFourOffsetsZ        gUnk_086061CC
#define gSceneEffectTwoOffsetsX         gUnk_086061D4
#define gSceneEffectTwoOffsetsZ         gUnk_086061D8
#include "global.h"
#include "scene_actor_registry.h"
extern const u16 gUnk_086061D4[], gUnk_086061D8[], gUnk_086061C4[], gUnk_086061CC[];
extern const u8 gUnk_086061C0[];
u8 *ExplosionEffect_CreateDelayed(void *, u32, s32, s32, s32, u32);
void sub_080DA810(struct SceneActorRegistryActor *emitter)
{
    struct ContactPackedVector position;
    s32 i;
    position = *(struct ContactPackedVector *)&emitter->position;
    position.y += 0x40;
    switch (emitter->paletteOffset) {
        case 0:
            ExplosionEffect_CreateDelayed(&position, 0, emitter->effectDurationB8, emitter->effectParameterC0, 0, 0);
            break;
        case 1:
            for (i = 0; i < 2; i++) {
                struct ContactPosition *p = (struct ContactPosition *)&position;
                position = *(struct ContactPackedVector *)&emitter->position;
                p->y += 0x40;
                p->x += gUnk_086061D4[i];
                p->z += gUnk_086061D8[i];
                ExplosionEffect_CreateDelayed(&position, 0, emitter->effectDurationB8, emitter->effectParameterC0, 0,
                             gUnk_086061C0[i]);
            }
            break;
        case 2:
            for (i = 0; i < 4; i++) {
                struct ContactPosition *p = (struct ContactPosition *)&position;
                position = *(struct ContactPackedVector *)&emitter->position;
                p->y += 0x40;
                p->x += gUnk_086061C4[i];
                p->z += gUnk_086061CC[i];
                ExplosionEffect_CreateDelayed(&position, 0, emitter->effectDurationB8, emitter->effectParameterC0, 0,
                             gUnk_086061C0[i]);
            }
            break;
    }
}
