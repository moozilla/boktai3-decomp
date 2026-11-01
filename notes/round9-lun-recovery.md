# Lun recovery

Tags: 0809B7FC #lun #depin

`Lun_ResumeAfterLiftPenalty` matches all 336 bytes without a barrier.
Loading the contact flags before the halfword state initializer keeps the
shared byte zero in r7 across both branches while preserving each branch's
separate counter zero.

Lun_UpdateHoverHeight matches all 476 bytes without its two pins or either
barrier. The two early exits each set the actor height from its ground height
and hover offset. Writing that action at both exits lets agbcc join the stores
and choose the original registers for the actor flag and ground offsets.
