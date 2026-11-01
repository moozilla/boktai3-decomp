# Scene actor retreat route

Tags: #scene-actors #terrain #matching

SceneActorTerrain_RequestRetreatRoute matches all 968 bytes. It starts opposite the
actor's facing, scans a row or column of equal-height cells, then rotates to the next
direction if the scan returned the actor's current cell. It tries at most four directions.
Mummy_RequestStraightRoute uses the same scan and path request for a single direction.
The route owner and start position are separate arguments to the path request.
