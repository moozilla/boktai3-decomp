#define gPairedSceneHalfStateCallbacks gUnk_086120A0
#define gActorUpdatePaused             gEnginePauseFlags
#include "global.h"
#include "render.h"
struct PairedSceneHalf {
    u8 unknown0[0x16D];
    u8 state;
    u8 unknown16E[2];
    u8 finished;
    u8 unknown171[7];
    u32 timer;
    u8 unknown17C[0xA0];
};
struct PairedSceneActor {
    u8 unknown0[0x98];
    struct PairedSceneHalf halves[2];
};
extern u32 gUnk_02000560;
extern void *gPairedSceneHalfStateCallbacks[];
void Vanargand_ClearPartContact(void *);
void sub_081AFF98(struct PairedSceneHalf *);
void sub_081AF7F4(struct PairedSceneHalf *);
void PairedSceneActor_UpdateHalfPresentation(struct PairedSceneActor *, struct PairedSceneHalf *);
void sub_08249244(struct PairedSceneActor *, struct PairedSceneHalf *, s32, void *);
void PairedSceneActor_UpdateHalves(struct PairedSceneActor *actor)
{
    s32 i;
    struct PairedSceneHalf *half;
    u8 *state;
    u32 *timer;
    u32 mask;
    if (!(gEnginePauseFlags & 7)) {
        mask = 4;
        timer = &actor->halves[0].timer;
        state = &actor->halves[0].state;
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
            timer = (u32 *)((u8 *)timer + 0x21C);
            state += 0x21C;
            half = (struct PairedSceneHalf *)((u8 *)half + 0x21C);
        } while (--i >= 0);
    }
}
