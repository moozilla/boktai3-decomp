#ifndef GUARD_SCENE_ACTOR_SCRIPT_H
#define GUARD_SCENE_ACTOR_SCRIPT_H
#include "scene_actor_registry.h"
/* Tentative view of the script-controlled bytes in the actor's context tail. */
struct SceneActorScriptControlView {
    u8 unknown0[0x42];
    u16 property42;
    u8 unknown44[8];
    u16 parameter4C, radius;
    u32 radiusSquared;
    u8 unknown54[0x30];
    u8 parameter0;
    u8 unknown85;
    u8 parameter1;
};
struct SceneActorScriptPositionView {
    u8 unknown0[0x5C];
    struct ContactPosition position;
    u8 unknown64[0x25A];
    s16 groundHeight;
    u8 unknown2C0[0x2A];
    s16 heightOffset;
    u8 unknown2EC[8];
    u16 cellIndex;
};
/* Offset names remain provisional until actor simulation consumers are reviewed. */
struct SceneActorScriptPropertiesView {
    u32 objectType;
    u8 unknown4[0x98];
    s16 health;
    u8 property9E, property9F, propertyA0, unknownA1;
    u16 propertyA2, propertyA4, propertyA6, propertyA8;
    u8 unknownAA[6];
    s16 baseHealth;
    u16 propertyB2;
    u16 parametersB4[4], parametersBC[4], parametersC4[4];
    u32 parametersCC[4];
    u8 unknownDC[0xE];
    u8 behaviorEA, unknownEB;
    u16 range, rangeCopy;
    u8 unknownF0[8];
    u32 rangeSquared, rangeSquaredCopy;
    u8 unknown100[0xEC];
    u32 property1EC, property1F0, property1F4, property1F8;
    u32 parameters1FC[5];
    u8 unknown210[0x1C4];
    struct SceneActorScriptControlView control;
    u8 unknown45C[0x15A];
    u16 quarterHealth;
};
struct SceneActorRegistryActor *SceneActorRegistry_FindById(u32);
u8 *sub_0821D64C(u32, u32);
#endif
