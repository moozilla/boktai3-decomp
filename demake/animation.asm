; Decode the selected DE23 walking sequence directly from (frame, duration)
; pairs. The image stays live while the next metasprite streams into spare VRAM.
player_pose:
    ld a,[facing]
    ld e,a
    ld d,0
    ld a,[shot_pose_timer]
    and a
    jr z,.movement
    dec a
    ld [shot_pose_timer],a
    ld hl,gun_poses
    cp 10
    jr nc,.recoil
    cp 5
    jr c,.choose
    ld a,10
    jr .offset
.recoil:
    ld a,5
.offset:
    add e
    ld e,a
    jr .choose
.movement:
    ld a,[walking]
    and a
    jr nz,.walk
    xor a
    ld [anim_wait],a
    ld hl,idle_poses
.choose:
    add hl,de
    ld a,[hl]
    ld [want_pose],a
    ret
.walk:
    ld a,[anim_dir]
    cp e
    jr nz,.first
    ld a,[anim_wait]
    and a
    jr z,.first
    dec a
    ld [anim_wait],a
    ret nz
    ld a,[anim_key]
    add 2
    jr .read
.first:
    ld a,e
    ld [anim_dir],a
    xor a
.read:
    ld [anim_key],a
    ld b,a
    ld a,e
    add a
    ld e,a
    ld hl,walk_ptrs
    add hl,de
    ld a,[hl+]
    ld h,[hl]
    ld l,a
    push hl
    ld e,b
    add hl,de
    ld a,[hl+]
    cp 255
    jr z,.loop
    pop de
    jr .frame
.loop:
    pop hl
    xor a
    ld [anim_key],a
    ld a,[hl+]
.frame:
    ld [want_pose],a
    ld a,[hl]
    ld [anim_wait],a
    ret
