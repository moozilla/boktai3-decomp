#include "global.h"
typedef struct {
    u32 id;
    u8 active, pad[3];
    int time;
    u8 sprite[44], animation[48];
    u32 callback;
    u8 tail[68];
} Slot;
typedef struct {
    u8 pad[24];
    Slot slots[16];
} Manager;

int sub_08249248(Manager *, Slot *);
void sub_08214514(void *);
int SpritePool_Update(Manager *p)
{
    Slot *q = p->slots;
    int i = 15;
    do {
        if (q->active) {
            if (q->callback) {
                if (sub_08249248(p, q)) {
                    sub_08214514(q->sprite);
                    q->active = 0;
                    q->id = 0;
                }
                q->time++;
            } else {
                sub_08214514(q->sprite);
                q->active = 0;
                q->id = 0;
            }
        }
        i--;
        q++;
    } while (i >= 0);
    return 0;
}
