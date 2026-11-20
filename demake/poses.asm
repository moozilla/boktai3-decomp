; Each actor has a live 24x32 metasprite and a spare 192-byte tile region.
; It stays visible while its next pose is assembled in the spare region over
; VBlanks. Tile pairs remain even for the original DMG's 8x16 OBJ mode.
pose_source:
    add a
    ld e,a
    ld d,0
    ld hl,pose_ptrs
    add hl,de
    ld a,[hl+]
    ld h,[hl]
    ld l,a
    ret
tile_address:
    ; DE = $8000 + tile * 16.
    ld d,a
    swap a
    and 0xf0
    ld e,a
    ld a,d
    swap a
    and 15
    or 0x80
    ld d,a
    ret
reset_poses:
    xor a
    ld [upload_left],a
    ld [upload_slot],a
    ld [live_tile],a
    ld a,12
    ld [spare_tile],a
    ld a,24
    ld [live_tile+1],a
    ld a,36
    ld [spare_tile+1],a
    ld a,48
    ld [live_tile+2],a
    ld a,60
    ld [spare_tile+2],a
    ld a,8
    ld [want_pose],a
    ld [live_pose],a
    call pose_source
    ld de,0x8000
    ld bc,192
    call copy
    ld a,10
    ld [want_pose+1],a
    ld [live_pose+1],a
    call pose_source
    ld de,0x8180
    ld bc,192
    call copy
    ld a,255
    ld [want_pose+2],a
    ld [live_pose+2],a
    ret

prepare_pose:
    ld a,[upload_left]
    and a
    ret nz
    ld a,[upload_slot]
    ld e,a
    ld d,0
    ld b,3
.scan:
    inc e
    ld a,e
    cp 3
    jr c,.slot
    ld e,0
.slot:
    ld hl,want_pose
    add hl,de
    ld a,[hl]
    cp 255
    jr z,.next
    ld hl,live_pose
    add hl,de
    cp [hl]
    jr nz,.fetch
.next:
    dec b
    jr nz,.scan
    ret
.fetch:
    ld [upload_pose],a
    push de
    call pose_source
    ld a,h
    ld [upload_src_hi],a
    ld a,l
    ld [upload_src_lo],a
    pop de
    ld a,e
    ld [upload_slot],a
    ld hl,spare_tile
    add hl,de
    ld a,[hl]
    call tile_address
    ld a,d
    ld [upload_dest_hi],a
    ld a,e
    ld [upload_dest_lo],a
    ld a,6
    ld [upload_left],a
    ret

flush_pose:
    ; A diagonal map refill takes both row and column slots this VBlank.
    ; Leave the pose transfer for the following tick in that case.
    ld a,[row_pending]
    ld hl,col_pending
    and [hl]
    ret nz
    ld a,[upload_left]
    and a
    ret z
    ld a,[upload_src_hi]
    ld h,a
    ld a,[upload_src_lo]
    ld l,a
    ld a,[upload_dest_hi]
    ld d,a
    ld a,[upload_dest_lo]
    ld e,a
    ld b,8
.copy:
    ld a,[hl+]
    ld [de],a
    inc de
    ld a,[hl+]
    ld [de],a
    inc de
    ld a,[hl+]
    ld [de],a
    inc de
    ld a,[hl+]
    ld [de],a
    inc de
    dec b
    jr nz,.copy
    ld a,h
    ld [upload_src_hi],a
    ld a,l
    ld [upload_src_lo],a
    ld a,d
    ld [upload_dest_hi],a
    ld a,e
    ld [upload_dest_lo],a
    ld hl,upload_left
    dec [hl]
    ret nz
    ld a,[upload_slot]
    ld e,a
    ld d,0
    ld hl,live_pose
    add hl,de
    ld a,[upload_pose]
    ld [hl],a
    ld hl,live_tile
    add hl,de
    ld b,[hl]
    push hl
    ld hl,spare_tile
    add hl,de
    ld a,[hl]
    ld [hl],b
    pop hl
    ld [hl],a
    ret
