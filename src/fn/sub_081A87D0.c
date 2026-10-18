#define ActorSoundPhase_UpdateStop sub_081A87D0
#define ActorSoundPhase_StopSong Vanargand_StopActionSequence
#include "global.h"
struct ActorSoundPhase {
    u8 f0[0xAA];
    u8 st;
    u8 g[4];
    u8 af;
    u8 b0;
    u8 f2[0xC4 - 0xB1];
    u32 c4;
};
void Vanargand_StopActionSequence(void *);
void sub_081A7C94(void *);
void sub_081A7D80(void *);
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
void sub_081A87D0(struct ActorSoundPhase *p)
{
    struct ActorSoundPhase *q = p;
    if (chk(p)) {
        u32 v = 15;
        Vanargand_StopActionSequence((u8 *)p + 0x21C);
        p->st = v;
    }
    if (p->af != 0) {
        sub_081A7C94(q);
        sub_081A7D80(q);
    }
}
