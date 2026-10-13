#include "global.h"
extern void *gUnk_0200047C;
void *sub_08219FBC(int, int);
void sub_0821A04C(void *, void *, void *);
int SceneMessages_Initialize(void *, int);
void sub_0821A0C0(void *);
void SceneMessages_Update(void);
void sub_08020C28(void);
void *SceneMessages_Create(int id)
{
    void *p;
    if (gUnk_0200047C)
        return 0;
    p = sub_08219FBC(2, 0xac);
    if (p) {
        sub_0821A04C(p, SceneMessages_Update, sub_08020C28);
        if (SceneMessages_Initialize(p, id) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
