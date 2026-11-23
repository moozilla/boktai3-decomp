#include "scene_volume.h"

void SceneVolume_ResetPendingSequence(struct SceneVolumeActor *volume)
{
    if (volume->pendingReset) {
        volume->sequenceState = 0;
        volume->pendingReset = 0;
        volume->sequenceLength = 0x34;
        volume->sequence = volume->sequenceBuffer;
    }
}
