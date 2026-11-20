; Skeleton instances share a 16-byte working record. ROM-side route records
; retain their HP positions, waits and facing. Slot 0/1 use OBJ banks 1/2.
sk_address:
    ld a,[sk_slot]
    swap a
    add 0x20
    ld l,a
    ld h,0xc3
    ret
sk_path:
    ld a,[sk_slot]
    rrca
    ld l,a
    ld h,0x71
    ret
sk_get:
    call sk_address
    ld de,sk_base
    jr sk_copy
sk_put:
    call sk_address
    ld d,h
    ld e,l
    ld hl,sk_base
sk_copy:
    ; All actor records and their working copy lie in one WRAM page.
    ld b,4
.loop:
    ld a,[hl+]
    ld [de],a
    inc e
    ld a,[hl+]
    ld [de],a
    inc e
    ld a,[hl+]
    ld [de],a
    inc e
    ld a,[hl+]
    ld [de],a
    inc e
    dec b
    jr nz,.loop
    ret
sk_next:
    ld hl,sk_slot
    inc [hl]
    ld a,[header+16]
    cp [hl]
    ret

sk_init:
    xor a
    ld [sk_slot],a
    ld a,[header+16]
    and a
    ret z
.loop:
    ld hl,sk_base
    ld bc,16
    call clear
    call sk_path
    inc hl
    ld a,[hl+]
    ld [sk_x],a
    ld a,[hl+]
    ld [sk_z],a
    ld a,[hl]
    ld [sk_timer],a
    call sk_put
    ld a,[sk_slot]
    and a
    ld de,0x8180
    jr z,.copy
    ld de,0x8300
.copy:
    ld a,skeleton_pose
    call load_pose
    ld a,[sk_slot]
    ld e,a
    ld d,0
    ld hl,live_pose+1
    add hl,de
    ld [hl],skeleton_pose
    ld hl,want_pose+1
    add hl,de
    ld [hl],skeleton_pose
    call sk_next
    jr nz,.loop
    ret

sk_update:
    xor a
    ld [sk_slot],a
    ld a,[header+16]
    and a
    ret z
.loop:
    call sk_get
    call sk_tick
    call sk_put
    call sk_next
    jr nz,.loop
    ret

sk_tick:
    ld a,[sk_phase]
    cp 5
    ret z
    ; Inspect the existing solar shot before doing the next motion step.
    ld a,[projectile]
    and a
    jr z,.player
    ld a,[shot_x]
    ld hl,sk_x
    sub [hl]
    add 4
    cp 9
    jr nc,.player
    ld a,[shot_z]
    ld hl,sk_z
    sub [hl]
    add 4
    cp 9
    jr nc,.player
    xor a
    ld [projectile],a
    ld a,5
    ld [sk_phase],a
    jp hit_sound
.player:
    ld a,[sk_x]
    ld hl,player_x
    sub [hl]
    add 2
    cp 5
    jr nc,.phase
    ld a,[sk_z]
    ld hl,player_z
    sub [hl]
    add 2
    cp 5
    jr nc,.phase
    ld a,[grace]
    and a
    call z,player_hit
.phase:
    ld a,[sk_phase]
    and a
    jr z,.patrol
    cp 1
    jr z,.wind
    cp 2
    jr z,.burst
    cp 3
    jr z,.decay
    ld hl,sk_timer
    dec [hl]
    ret nz
    xor a
    ld [sk_phase],a
    ret
.wind:
    ld hl,sk_timer
    dec [hl]
    ret nz
    ld [hl],4
    ld a,2
    ld [sk_phase],a
    ret
.burst:
    call sk_step
    ld hl,sk_timer
    dec [hl]
    ret nz
    ld a,3
    ld [sk_phase],a
    ld a,lunge_speed
    ld [sk_speed],a
    xor a
    ld [sk_frac],a
    ret
.decay:
    ; Skeleton_TickDecayingLunge: start at 0x48 world units, halve each
    ; fourth tick. Integrate the sub-cell remainder in the DMG working byte.
    ld a,[sk_speed]
    ld hl,sk_frac
    add [hl]
    ld b,0
.integrate:
    cp 32
    jr c,.fraction
    sub 32
    inc b
    jr .integrate
.fraction:
    ld [hl],a
    ld a,b
    and a
    jr z,.halve
.steps:
    push bc
    call sk_step
    pop bc
    dec b
    jr nz,.steps
.halve:
    ld hl,sk_timer
    inc [hl]
    ld a,[hl]
    and lunge_period_mask
    ret nz
    ld hl,sk_speed
    srl [hl]
    ret nz
    ld a,4
    ld [sk_phase],a
    ld a,40
    ld [sk_timer],a
    ret
.patrol:
    ld a,[player_x]
    ld hl,sk_x
    sub [hl]
    add 12
    cp 25
    jr nc,.route
    ld a,[player_z]
    ld hl,sk_z
    sub [hl]
    add 12
    cp 25
    jr nc,.route
    ld a,[player_x]
    ld hl,sk_x
    call sk_sign
    ld [sk_dx],a
    ld a,[player_z]
    ld hl,sk_z
    call sk_sign
    ld [sk_dz],a
    ld a,12
    ld [sk_timer],a
    ld a,1
    ld [sk_phase],a
    ret
.route:
    ld hl,sk_timer
    ld a,[hl]
    and a
    jr z,.walk
    dec [hl]
    ret
.walk:
    ld a,[tick]
    and 3
    ret nz
    call sk_path
    ld a,[sk_point]
    add a
    add a
    ld e,a
    ld d,0
    add hl,de
    inc hl
    ld d,[hl]
    inc hl
    ld e,[hl]
    inc hl
    ld b,[hl]
    ld a,d
    ld hl,sk_x
    call sk_sign
    ld [sk_dx],a
    ld c,a
    ld a,e
    ld hl,sk_z
    call sk_sign
    ld [sk_dz],a
    or c
    jp nz,sk_step
    ld a,b
    ld [sk_timer],a
    ld hl,sk_point
    inc [hl]
    ld a,[hl]
    call sk_point_wrap
    ret
sk_point_wrap:
    ld b,a
    call sk_path
    ld a,b
    cp [hl]
    ret c
    xor a
    ld [sk_point],a
    ret
sk_sign:
    cp [hl]
    jr z,.zero
    ld a,255
    ret c
    ld a,1
    ret
.zero:
    xor a
    ret
sk_step:
    ld a,[sk_x]
    ld b,a
    ld a,[sk_z]
    ld c,a
    call enemy_surface
    ld [sk_facing],a
    ld a,[sk_dx]
    add b
    ld b,a
    ld a,[sk_dz]
    add c
    ld c,a
    call enemy_surface
    ret c
    ld hl,sk_facing
    sub [hl]
    inc a
    cp 3
    ret nc
    ld a,b
    ld [sk_x],a
    ld a,c
    ld [sk_z],a
    ret

sk_draw:
    xor a
    ld [sk_slot],a
    ld a,[header+16]
    and a
    ret z
.loop:
    call sk_get
    ld a,[sk_phase]
    cp 5
    jr z,.next
    and a
    jr nz,.striking
    ld a,[tick]
    and 16
    ld a,skeleton_pose
    jr z,.pose
    inc a
    jr .pose
.striking:
    cp 1
    ld a,skeleton_pose+2
    jr z,.pose
    inc a
.pose:
    ld b,a
    ld a,[sk_slot]
    ld e,a
    ld d,0
    ld hl,want_pose+1
    add hl,de
    ld [hl],b
    ld hl,live_tile+1
    add hl,de
    ld a,[hl]
    ld [pose],a
    ld a,[sk_dx]
    cp 255
    ld a,0x10
    jr nz,.attr
    or 0x20
.attr:
    ld [attr],a
    ld a,[sk_x]
    ld b,a
    ld a,[sk_z]
    ld c,a
    call project
    call actor_oam
.next:
    call sk_next
    jr nz,.loop
    ret
