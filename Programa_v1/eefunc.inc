; ============================================================================
; ReadConf - Lee de EE y escribe las variables persistentes (0xE0..0xFF)
; ============================================================================
 ERRORLEVEL -302
ReadConf
  bcf     STATUS,RP0          ;
  bsf     STATUS,RP1          ; --> BANK2
  clrf    EEADR               ; Address to read
  clrf    EEADRH
  bsf     STATUS,RP0          ; --> BANK3
  bcf     EECON1,EEPGD        ; Point to Data memory
  ; leo el primer byte
  bsf     EECON1,RD           ; EE Read
  bcf     STATUS,RP0          ; --> BANK2
  movf    EEDATA,w            ; EEDATA --> w
  ; comparo el primer byte con una mascara de 1s y 0s
  ; para ver si tiene datos válidos
  sublw   MARCA_EE_OK
  btfss   STATUS,Z
  goto    ReadConfEnd         ; los datos en EE no son validos
  ; sigo en BANK2
ReadConfValid
  bcf     STATUS,IRP     ; pasa a banco 0/1 para direccionamiento indirecto
  movlw   CONFIG_START   ; cargo la dirección de inicio de variables persistentes
  movwf   FSR
  ; sigo en BANK2
ReadConfLoop
  incf    EEADR,f             ; Address to read
  bsf     STATUS,RP0          ; --> BANK3
  bsf     EECON1,RD           ; EE Read
  bcf     STATUS,RP0          ; --> BANK2
  movf    EEDATA,w            ; EEDATA --> w
  movwf   INDF                ; w --> (FSR->f)
  incfsz  FSR,f               ; recorro hasta el overflow del FSR (cuando excede 0xFF)
  goto    ReadConfLoop
ReadConfEnd
  bcf     STATUS,RP0
  bcf     STATUS,RP1 ; --> BANK0
  return
 ERRORLEVEL +302
; ============================================================================
; WriteConf - escribe en EE las variables persistentes (CONFIG_START..0xFF)
; ============================================================================
 ERRORLEVEL -302
WriteConf
  clrwdt
  bcf     SAVETOEE  ; reseteo el flag
  bsf     STATUS,RP1          ;
  bsf     STATUS,RP0          ; --> BANK3
  btfsc   EECON1,WR           ; \Wait for write
  goto    $-1                 ; /to complete
  bcf     STATUS, RP0         ; --> BANK2
  clrf    EEADR               ; Address to write
  clrf    EEADRH
  movlw   MARCA_EE_OK         ; Data Memory Value
  movwf   EEDATA              ; w --> EEDATA
  bsf     STATUS,RP0          ; --> BANK3
  bcf     EECON1,EEPGD        ; Point to DATA
  ;memory
  bsf     EECON1,WREN ;Enable writes
  bcf     INTCON,GIE ;Disable INTs.
  btfsc   INTCON,GIE
  goto    $-2
  ; >>>> bloque loco para pasar la protección >>>>
  movlw   0x55         ;
  movwf   EECON2       ;Write 55h
  movlw   0xAA         ;
  movwf   EECON2         ;Write AAh
  bsf     EECON1,WR      ;Set WR bit to
  ;begin write
  ; <<<< bloque loco para pasar la protección <<<<
  bsf     INTCON,GIE          ;Enable INTs.
  ; sigo en BANK3
WriteConInit
  bcf     STATUS,IRP     ; pasa a banco 0/1 para direccionamiento indirecto
  movlw   CONFIG_START   ; cargo la dirección de inicio de variables persistentes
  movwf   FSR
  ; sigo en BANK3
WriteConfLoop
  btfsc   EECON1,WR           ; \Wait for write
  goto    $-1                 ; /to complete
  clrwdt
  bcf     STATUS, RP0         ; --> BANK2
  incf    EEADR,f             ; Address to write
  movf    INDF,w              ; (FSR->f) --> w
  movwf   EEDATA              ; w --> EEDATA
  bsf     STATUS,RP0          ; --> BANK3
  bcf     INTCON,GIE ;Disable INTs.
  btfsc   INTCON,GIE
  goto    $-2
  ; >>>> bloque loco para pasar la protección >>>>
  movlw   0x55                ;
  movwf   EECON2              ;Write 55h
  movlw   0xAA                ;
  movwf   EECON2              ;Write AAh
  bsf     EECON1,WR           ;Set WR bit to
  ;begin write
  ; <<<< bloque loco para pasar la protección <<<<
  bsf     INTCON,GIE          ;Enable INTs.
  incfsz  FSR,f               ; recorro hasta el overflow del FSR (cuando excede 0xFF)
  goto    WriteConfLoop
WriteConfEnd
  bcf     EECON1,WREN         ;Disable writes
  bcf     STATUS,RP0          ; --> BANK0
  bcf     STATUS,RP1
  return
 ERRORLEVEL +302
