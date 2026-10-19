
#ifdef NONMATCHING
// fuzzy: 98.0%
// foundry draft (worker best 98.0%); what differs is mostly register allocation and instruction order
#define ActorSoundPhase_UpdateConditionalStop sub_081A8820
#define ActorSoundPhase_StopSong              Vanargand_StopActionSequence
#include "global.h"
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
void sub_081A8820(struct ActorSoundPhase *p)
{
    struct ActorSoundPhase *q = p;
    if (chk(p)) {
        u32 v = SelectPhase(*(s16 *)((u8 *)p + 0x5c2));
        Vanargand_StopActionSequence((u8 *)p + 0x21C);
        p->st = v;
    }
    if (p->af != 0) {
        ScenePanel_UpdatePairConditions(q);
        ScenePanel_ApplyPairTransition(q);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081A8820);
#endif
