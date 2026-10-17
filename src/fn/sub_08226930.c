#include "global.h"

typedef struct {
    u16 id;
    u8 unk[4];
    u8 count[2];
    u16 *messages[2][4];
    u8 pad[8];
    void *next;
} Entry;

struct Vec {
    u16 x, y, z, w;
};

extern u8 *gUnk_0200047C;

void sub_08020D54(void *, void *);
void sub_08226918(u8 *);
void sub_08226920(u8 *);
void sub_08226228(void);
void sub_08226268(struct Vec *);
void sub_0822632C(s32, struct Vec *);
void sub_08226474(s32);
void sub_08226558(s32, s32, s32);
void sub_082264D0(s32);

void sub_08226930(u8 *p)
{
    Entry *entry = (Entry *)(p + 0x20);
    s32 i;
    struct Vec snap;
    struct Vec pan;

    i = 0;
    if (i < entry->count[gUnk_0200047C[0x24]])
        do {
            u16 *message;
            u8 *controller = gUnk_0200047C;
            message = entry->messages[controller[0x24]][i];
            sub_08020D54(entry, message);
            switch (((u8 *)message)[6]) {
                case 0:
                    sub_08226918(p);
                    sub_08226228();
                    sub_08226920(p);
                    break;
                case 1:
                    sub_08226918(p);
                    snap.x = message[4];
                    snap.y = message[5];
                    snap.z = message[6];
                    sub_08226268(&snap);
                    sub_08226920(p);
                    break;
                case 2:
                    sub_08226918(p);
                    pan.x = message[4];
                    pan.y = message[5];
                    pan.z = message[6];
                    sub_0822632C((s16)message[7], &pan);
                    break;
                case 3:
                    sub_08226918(p);
                    sub_08226474((s16)message[4]);
                    break;
                case 4:
                    sub_08226918(p);
                    sub_08226558((s16)message[4], (s16)message[5], (s16)message[6]);
                    break;
                case 5:
                    sub_08226918(p);
                    sub_082264D0((s16)message[4]);
                    break;
            }
            i++;
        } while (i < entry->count[gUnk_0200047C[0x24]]);
}
