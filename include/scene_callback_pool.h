#ifndef FOUNDRY_SCENE_CALLBACK_POOL_H
#define FOUNDRY_SCENE_CALLBACK_POOL_H
struct ScenePoolSlot {
    void *callback;
    u8 unknown4[0x78];
    u8 sprite[0x53];
    u8 active;
    u8 unknownD0[8];
};
struct SceneCallbackPool {
    u8 task[0x18];
    struct ScenePoolSlot slots[16];
    u32 frame;
};
#endif
