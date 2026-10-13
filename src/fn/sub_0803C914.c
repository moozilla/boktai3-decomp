#include "global.h"
struct Item {u8 pad[0x30];int id;};
void *sub_0803BA90(int);
void sub_08222410(void*,void*,void*);
void sub_0822241C(void*,void*);
void sub_0803C914(u8 *p,struct Item *item)
{
    void *resource=sub_0803BA90(item->id);
    sub_08222410(item,resource,item);
    sub_0822241C(p+0x2354,item);
}
