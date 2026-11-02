#define gPairedSceneHalfStateCallbacks gUnk_086120A0
#define gActorUpdatePaused             gEnginePauseFlags
#include "paired_scene_actor.h"
#include "render.h"
extern u32 gUnk_02000560;
extern void *gPairedSceneHalfStateCallbacks[];
void Vanargand_ClearPartContact(void *);
void sub_081AFF98(struct PairedSceneHalf *);
void sub_081AF7F4(struct PairedSceneHalf *);
void sub_08249244(struct VanargandAttackPair *, struct PairedSceneHalf *, s32, void *);
void PairedSceneActor_UpdateHalves(struct VanargandAttackPair *actor)
{
    s32 i;
    struct PairedSceneHalf *half;
    u8 *state;
    u32 *timer;
    u32 mask;
    if (!(gEnginePauseFlags & 7)) {
        mask = 4;
        timer = &actor->halves[0].stateTimer;
        state = &actor->halves[0].requestedState;
        half = &actor->halves[0];
        i = 1;
        do {
            if (*state) {
                u32 paused;
                Vanargand_ClearPartContact(half);
                paused = gUnk_02000560 & mask;
                if (!paused) {
                    sub_081AFF98(half);
                    sub_081AF7F4(half);
                    state[3] = paused;
                    {
                        void **table = gPairedSceneHalfStateCallbacks;
                        void **callback = (void **)((*state << 2) + (u32)table);
                        sub_08249244(actor, half, *timer, *callback);
                    }
                }
                PairedSceneActor_UpdateHalfPresentation(actor, half);
                ++*timer;
            }
            timer = (u32 *)((u8 *)timer + sizeof(*half));
            state += sizeof(*half);
            half++;
        } while (--i >= 0);
    }
}
