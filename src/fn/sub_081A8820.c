#include "global.h"

#define ActorSoundPhase_StopSong Vanargand_StopActionSequence
struct ActorSoundPhase {
    u8 f0[0xAA];
    u8 st;
    u8 g[4];
    u8 af;
    u8 b0;
    u8 f2[0xC4 - 0xB1];
    u32 c4;
    u8 pad[0x5c2 - 0xc8];
    s16 variant;
};
void Vanargand_StopActionSequence(void *);
void ScenePanel_UpdatePairConditions(void *);
void ScenePanel_ApplyPairTransition(void *);
static inline u8 chk(struct ActorSoundPhase *p)
{
    u8 *q = &p->b0;
    if (*q != 0) {
        *q = 0;
        *(q - 1) = 0;
        return TRUE;
    }
    return FALSE;
}
static inline u8 SelectPhase(s16 variant)
{
    if (variant == 0)
        return 16;
    return 13;
}
void ActorSoundPhase_UpdateConditionalStop(struct ActorSoundPhase *p)
{
    struct ActorSoundPhase *q = p;
    if (chk(p)) {
        u32 v = SelectPhase(*(s16 *)((u8 *)p + 0x5c2));
        do {
            Vanargand_StopActionSequence((u8 *)q + 0x21C);
            q->st = v;
        } while (0);
    }
    if (q->af != 0) {
        ScenePanel_UpdatePairConditions(p);
        ScenePanel_ApplyPairTransition(p);
    }
}
