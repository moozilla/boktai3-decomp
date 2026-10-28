#include "global.h"
#include "render.h"

void Memory_Copy(u32, u32, u32);

void Text_QueueTileUpload(u32 v, u32 b)
{
    if (gTextTileUploadCount <= 0x100) {
        gTextTileUploadIndices[gTextTileUploadCount] = v;
        Memory_Copy(gTextTileUploadCount * 32 + 0x02034C00, b, 0x20);
        gTextTileUploadCount++;
    }
}
