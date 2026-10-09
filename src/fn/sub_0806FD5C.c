#include "global.h"
typedef struct { u16 type; u8 id; u8 pad; u16 value; u8 data[2]; } Data;
typedef struct { u32 unk; Data *data; } Entry;
typedef struct { u8 pad[0x18]; Entry *entries; u8 pad2[8]; void *arg; } Obj;
s32 sub_0821A3E8(void *, Entry **);
void sub_0806FC24(u8, void *);
void sub_0806FC64(u8, void *);
void sub_0806FD08(u8, void *, u16);
void sub_0806FD5C(Obj *p) {
    s32 n = sub_0821A3E8(p->arg, &p->entries);
    Entry *entry;
    if (n > 0) {
        entry = p->entries;
        while (n-- > 0) {
            Data *data = entry->data;
            switch (data->type) {
            case 1: sub_0806FC24(data->id, &data->value); break;
            case 2: sub_0806FC64(data->id, &data->value); break;
            case 3: sub_0806FD08(data->id, data->data, data->value); break;
            }
            entry++;
        }
    }
}
