// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneObjects_Append sub_0801E184
typedef struct {
    u32 words[2];
} Vec;
void *sub_08219C40(int);
void sub_08219DD8(void *, int);
int sub_0822C034(void *);
void sub_0801F058(void *, void *);
void sub_0801F5EC(void *, void *);
void sub_0801EC4C(void *, void *);
int SceneObjects_Append(u8 *p, u16 id, Vec *position, int value, u8 kind, u8 variant,
                        u8 orientation, int type, int field88, int flags, int field41, u32 *data,
                        u8 field44, int field45)
{
    u8 *slot;
    int i;
    u8 zeroByte;
    u16 zeroHalf;
    if (*(u16 *)(p + 0x1a) >= *(int *)(p + 0x30))
        return -1;
    slot = *(u8 **)(p + 0x42c) + *(u16 *)(p + 0x1a) * 140;
    zeroByte = 0;
    zeroHalf = 0;
    asm("" : "+r"(zeroByte), "+r"(zeroHalf));
    *(u16 *)(slot + 0x34) = id;
    *(Vec *)(slot + 0x68) = *position;
    *(Vec *)(slot + 0x78) = *position;
    slot[0x36] = kind;
    slot[0x37] = variant;
    slot[0x38] = orientation;
    slot[0x3a] = zeroByte;
    slot[0x40] = type;
    *(u16 *)(slot + 0x88) = field88;
    *(int *)(slot + 0x48) = flags;
    slot[0x41] = field41;
    *(int *)(slot + 0x4c) = value;
    *(u16 *)(slot + 0x80) = zeroHalf;
    *(u16 *)(slot + 0x42) = sub_0822C034(position);
    slot[0x44] = field44;
    *(void **)(slot + 0x2c) = sub_08219C40(16);
    if (!*(void **)(slot + 0x2c))
        return -1;
    sub_08219DD8(*(void **)(slot + 0x2c), 16);
    switch (slot[0x40]) {
        case 1:
            sub_0801F058(p, slot);
            break;
        case 2:
            slot[0x45] = field45;
            sub_0801F5EC(p, slot);
            break;
        case 8:
            slot[0x45] = 1;
            sub_0801F5EC(p, slot);
            break;
        case 9:
            slot[0x45] = 2;
            sub_0801F5EC(p, slot);
            break;
        case 10:
            slot[0x45] = 3;
            sub_0801F5EC(p, slot);
            break;
        case 11:
            slot[0x45] = 4;
            sub_0801F5EC(p, slot);
            break;
        case 12:
            slot[0x45] = 5;
            sub_0801F5EC(p, slot);
            break;
        case 13:
            slot[0x45] = 6;
            sub_0801F5EC(p, slot);
            break;
        case 14:
            slot[0x45] = 7;
            sub_0801F5EC(p, slot);
            break;
        case 0:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 15:
            sub_0801EC4C(p, slot);
            break;
        case 16:
            sub_0801F5EC(p, slot);
            break;
    }
    if (*(int *)(slot + 0x48)) {
        u32 *cursor = data;
        u32 *out = (u32 *)(slot + 0x50);
        for (i = 3; i >= 0; i--)
            *out++ = *cursor++;
    } else
        *(int *)(slot + 0x50) = 0;
    asm("" ::: "memory");
    ++*(u16 *)(p + 0x1a);
}
