#include "scene_volume.h"

void SceneVolume_ResetPendingSequence(struct SceneVolumeActor *volume)
{
    if (volume->sequenceMode) {
        volume->sequenceTimer = 0;
        volume->sequenceMode = 0;
        volume->sequenceLength = 0x34;
        volume->sequence = volume->sequenceBuffer;
    }
}
