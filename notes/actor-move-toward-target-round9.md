# Actor_MoveTowardTarget

All 1,640 bytes match in ordinary C. The eight-way direction helper keeps the
sector in a signed halfword. The caller subtracts four and masks the promoted
result when recording the facing. The sprite bobs every fourth tick and both
animation updates use the shared sprite playback helper.

The movement and initial setup were also checked as part of the full 30,356-byte
controller translation unit.
