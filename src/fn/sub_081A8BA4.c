#define ActorSoundPhase_EnterPhase21 sub_081A8BA4
#define ActorSoundPhase_StopSong Vanargand_StopActionSequence
#include "global.h"
struct ActorSoundPhase {
    u8 f0[0xAA];
    u8 st;
    u8 previousPhase;
    u8 g[3];
    u8 af;
    u8 b0;
    u8 f2[0xC4 - 0xB1];
    u32 c4;
    u8 pad[0xf38-0xc8];
    s32 attachment;
};
void Vanargand_StopActionSequence(void *);
void sub_08159164(s32);
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
void sub_081A8BA4(struct ActorSoundPhase *p)
{
    struct ActorSoundPhase *q = p;
    if (chk(p)) {
        if (p->previousPhase != 21) {
            u32 v = 21;
            Vanargand_StopActionSequence((u8 *)p + 0x21c);
            p->st = v;
        }
        sub_08159164(q->attachment);
    }
    p->c4++;
}
