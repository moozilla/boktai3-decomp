#include "global.h"
#define RecordBuffer_CopyPayloads sub_08138630
struct RecordPayload { u32 words[8]; };
struct RecordCopyCount { u8 unknown0[0x1A]; u16 count; };
struct RecordCopySource { u8 unknown0[0x1C]; struct RecordPayload payload; u8 unknown3C[8]; };
void sub_08138630(struct RecordCopyCount *header, u8 *buffer)
{
    s32 count = header->count;
    if (count) {
        u8 *source;
        u8 *destination = buffer;
        s32 remaining;
        source = destination;
        source += 0xE0;
        remaining = count;
        do {
            *(struct RecordPayload *)(destination + 4) = ((struct RecordCopySource *)source)->payload;
            destination += 0x20;
            source += 0x44;
        } while (--remaining);
    }
}
