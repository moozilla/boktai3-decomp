#include "global.h"
struct Task {
    u8 pad0[0x18];
    void *listener;
    u16 state;
};
void sub_082279A8(int, int, int, int, int, int, int);
void sub_08036C54(void);
void *sub_08037DC0(void);
void Task_Kill(void *);
int sub_08034084(void);
void Script_Call(void *, int);
int sub_080394D4(struct Task *p)
{
    switch (p->state) {
        case 0:
            sub_082279A8(4, 0, 0, 0, 0, 65535, 0);
            sub_08036C54();
            {
                void *q = sub_08037DC0();
                if (q)
                    Task_Kill(q);
            }
            p->state++;
            break;
        case 1:
            if (!sub_08034084())
                p->state++;
            break;
        case 2:
            if (p->listener)
                Script_Call(p->listener, 0);
            Task_Kill(p);
            break;
    }
    return 0;
}
