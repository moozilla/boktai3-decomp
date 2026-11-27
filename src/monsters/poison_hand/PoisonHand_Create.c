#include "monsters/poison_hand.h"
s32 PoisonHand_Update(struct PoisonHand *);
s32 PoisonHand_Destroy(u8 *);
s32 PoisonHand_InitializeFromScript(void *, void *);
void *PoisonHand_Create(void *p)
{
    void *r = Task_Create(8, sizeof(struct PoisonHand));
    if (r) {
        Task_SetCallbacks(r, (void (*)(void *))PoisonHand_Update,
                          (void (*)(void *))PoisonHand_Destroy);
        if (PoisonHand_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
