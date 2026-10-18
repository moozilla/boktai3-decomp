#include "global.h"

struct Armor6CallbackSlot {
    u32 active;
    void *destroy;
    u8 unknown08[0x38];
};

/* Armor 6 is a tentative family name. Only observed offsets are modeled. */
struct Armor6 {
    u8 unknown000[0x3C];
    u8 sceneBody[0x78];
    u8 animation[4];
    u8 animationActive;
    u8 unknown0B9[0x5B];
    u8 shadow[0x6C];
    u8 component180[0x10];
    u8 collider190[0x54];
    u8 collider1E4[0x54];
    u8 collider238[0x54];
    u8 collider28C[0xC4];
    struct Armor6CallbackSlot slots350[3];
    struct Armor6CallbackSlot slots410[9];
    struct Armor6CallbackSlot slot650;
    u8 unknown690[0x44C];
    u8 messageReceiver[0x7C];
    u8 componentB58[1];
};

s32 Callback_InvokeActor(void *, void *);
void Armor6_DestroySpriteColliders(void *);
void ScaledShadow_Destroy(void *);
void Collider_Remove(void *);
void sub_0815F738(void *);
void SceneBody_Unregister(void *);
void Anim_Remove(void *);
void SceneMessages_UnregisterReceiver(void *);

static inline void Armor6_ClearCallback(struct Armor6CallbackSlot *slot)
{
    slot->active = 0;
    if (slot->destroy) {
        Callback_InvokeActor(slot, slot->destroy);
        slot->destroy = 0;
    }
}

s32 Armor6_Destroy(struct Armor6 *actor)
{
    s32 i;
    Armor6_ClearCallback(&actor->slot650);
    Armor6_ClearCallback(&actor->slots350[0]);
    Armor6_ClearCallback(&actor->slots350[1]);
    Armor6_ClearCallback(&actor->slots350[2]);
    for (i = 0; i < 9; i++) {
        Armor6_ClearCallback(&actor->slots410[i]);
    }
    Armor6_DestroySpriteColliders(actor->componentB58);
    ScaledShadow_Destroy(actor->shadow);
    Collider_Remove(actor->collider1E4);
    Collider_Remove(actor->collider238);
    Collider_Remove(actor->collider28C);
    Collider_Remove(actor->collider190);
    sub_0815F738(actor->component180);
    SceneBody_Unregister(actor->sceneBody);
    if (actor->animationActive) {
        Anim_Remove(actor->animation);
    }
    SceneMessages_UnregisterReceiver(actor->messageReceiver);
    return 0;
}
