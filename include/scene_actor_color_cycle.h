#ifndef FOUNDRY_SCENE_ACTOR_COLOR_CYCLE_H
#define FOUNDRY_SCENE_ACTOR_COLOR_CYCLE_H
struct SceneActorColorCycle {
    u8 unknown0[0xCC4];
    s8 red, green, blue;
    u8 unknownCC7[0xCF0 - 0xCC7];
    s32 (*update)(struct SceneActorColorCycle *);
    u8 unknownCF4[8];
    s32 timer;
    u8 unknownD00[0xD32 - 0xD00];
    u16 color;
    u8 unknownD34[0x1E];
    s8 duration;
};
#endif
