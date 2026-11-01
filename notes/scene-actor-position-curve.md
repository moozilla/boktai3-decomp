# Scene actor position curve

Tags: #scene-actors #matching

SceneActorControl_AdvancePositionCurve matches all 480 bytes. It uses the same fixed-point
interpolation and randomized time increment as Bee_AdvanceMovementCurve. The controller
keeps its result at +0x3FC and its endpoints at +0x41C and +0x424. The Bee, Ghost, and Lun
callers also match with the shared interpolation helper.
