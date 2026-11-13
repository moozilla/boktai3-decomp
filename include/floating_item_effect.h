#ifndef GUARD_FLOATING_ITEM_EFFECT_H
#define GUARD_FLOATING_ITEM_EFFECT_H
#include "choice_menu.h"
#include "scene_input_widget.h"
#include "player.h"

/* Eight item icons can bob, wait, spin away, or fly toward a player/target. */
struct FloatingEffect {
    u32 key;
    u8 state, entered, kind, mode;
    u32 item, timer;
    u16 duration;
    u8 index, unknown13[5];
    struct ContactPosition position;
    struct ContactPosition *target;
    struct SceneInputWidget sprite;
};
struct LinkedWidgetSet {
    struct Task task;
    u32 active;
    struct ChoiceResource resource;
    struct FloatingEffect effects[8];
};
void FloatingEffect_Init(struct FloatingEffect *);
void FloatingEffect_Project(struct FloatingEffect *);
void FloatingEffect_Bob(struct LinkedWidgetSet *, struct FloatingEffect *);
void FloatingEffect_Wait(struct LinkedWidgetSet *, struct FloatingEffect *);
void FloatingEffect_ShrinkAway(struct LinkedWidgetSet *, struct FloatingEffect *, s32);
void FloatingEffect_RiseAndSettle(struct LinkedWidgetSet *, struct FloatingEffect *);
void FloatingEffect_RiseAndShrink(struct LinkedWidgetSet *, struct FloatingEffect *, s32);
void FloatingItemEffect_UpdateFlight(struct LinkedWidgetSet *, struct FloatingEffect *, s32);
s32 FloatingEffectManager_Update(struct LinkedWidgetSet *);
u32 FloatingEffectManager_Destroy(struct LinkedWidgetSet *);
s32 LinkedWidgetSet_Init(struct LinkedWidgetSet *);
struct LinkedWidgetSet *FloatingEffectManager_Create(void);
struct LinkedWidgetSet *FloatingEffectManager_GetOrCreate(void);
s32 FloatingItemEffect_Spawn(s32, s32, s32, s32, struct ContactPosition *, struct ContactPosition *,
                             s32, u32);
void ScanlineWave_Reset(void);
extern void (*gUnk_08604C64[])(struct LinkedWidgetSet *, struct FloatingEffect *, s32);
u32 Selection_GetDisplayFrame(u32);
s32 Inventory_IsKeyItemId(u32);
s32 Widget_InitLinked(void *, void *, u16, u32, u8, u8, u8, const void *);
#endif
