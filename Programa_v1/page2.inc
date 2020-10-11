; gpasm cuando se hace un salto a una posicion que no está en la página 1
;       se queja aunque el destino del salto esté en la misma página que
;       el origen
 ERRORLEVEL -306
; =.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=
;
;   PAGE 2 - PAGE 2 - PAGE 2 - PAGE 2 - PAGE 2 - PAGE 2 - PAGE 2 - PAGE 2
;
; =.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=
  org 0x1000    ; cambio de página de memoria de programa (1000h-17FFh)
; ============================================================================
; Zona de tablas
; ============================================================================





; ============================================================================
; FIN - Zona de TABLAS
  org     0x1100
; ============================================================================
Notificaciones
  btfss   HSTONLINE
  return  ; si el host no está no le hablo
  ; me fijo si tengo que esperar
  movf    DelayResend,f
  btfss   STATUS,Z
  return   ; todavía no puedo notificar nada
  btfsc   TXBUFFULL
  return  ; el buffer está lleno, no se puede hacer nada
  ;
  call    NotificarInput
  call    NotificarAnalog
  call    NotificarOutput
  return
; ============================================================================
; NotificarInput - procesa los cambios de estado de las entradas para
; transmitir
; ============================================================================
NotificarInput
  ; inicializo el buffer
  PAGESEL ClrTx
  call    ClrTx
  PAGESEL $
; ----------------------------------------------------------------------------
NotificarInput1
  ; me fijo si tengo que notificar esta entrada
  movf    CountNotifE1,f
  btfsc   STATUS,Z
  goto    NotificarInput2
  ; decremento el contador de reintentos, si llega a cero --> chau
  decfsz  CountNotifE1,f
  goto    NotificarInput1Cont
  bcf     HSTONLINE   ; set off-line
  incf    Variant,f      ; incremento el contador de mensajes
  return
NotificarInput1Cont
  ; cargo el nuevo delay que se incrementa a medida que reintento
  movf    CountNotifE1,w
  sublw   REINT_DELAY
  movwf   DelayResend
  ; armo el mensaje con los datos de la entrada
  movlw   '1'
  movwf   txdat1
  movf    ENTRADA1,w
  movwf   txdat2
  ; le agrego la configuracion de la mascara de notificacion
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movf    MaskNotifE1,w
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movwf   txdat3
  ; le agrego la configuracion de seguidores
  movf    Mascara_E1S1,w
  movwf   txdat4
  goto    NotificarInputFin
; ----------------------------------------------------------------------------
NotificarInput2
  ; me fijo si tengo que notificar esta entrada
  movf    CountNotifE2,f
  btfsc   STATUS,Z
  return
  ; decremento el contador de reintentos, si llega a cero --> chau
  decfsz  CountNotifE2,f
  goto    NotificarInput2Cont
  bcf     HSTONLINE   ; set off-line
  incf    Variant,f      ; incremento el contador de mensajes
  return
NotificarInput2Cont
  ; cargo el nuevo delay que se incrementa a medida que reintento
  movf    CountNotifE2,w
  sublw   REINT_DELAY
  movwf   DelayResend
  ; armo el mensaje con los datos de la entrada
  movlw   '2'
  movwf   txdat1
  movf    ENTRADA2,w
  movwf   txdat2
  ; le agrego la configuracion de la mascara de notificacion
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movf    MaskNotifE2,w
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movwf   txdat3
  ; le agrego la configuracion de seguidores
  movf    Mascara_E2S2,w
  movwf   txdat4
; ----------------------------------------------------------------------------
NotificarInputFin
  ; >>>> mensaje de notificacion >>>>
  movlw   'Q'        ; start char
  movwf   txtyp
  movlw   HSTADDR    ; dest addr
  movwf   txdst
  movf    LocAddr,w  ; local addr
  movwf   txsrc
  movf    Variant,w  ; contador
  movwf   txcnt
  movlw   '6'        ; comando - '6'
  movwf   txfcn
  ; <<<< mensaje de notificacion <<<<
  ; lo encola y termina
  PAGESEL TxEnqueue
  call    TxEnqueue
  PAGESEL $
  return
; ============================================================================
; NotificarAnalog - procesa los cambios de estado de las entradas analogicas
; para transmitir
; ============================================================================
NotificarAnalog
  ; limpio para armar el mensaje
  PAGESEL ClrTx
  call    ClrTx
  PAGESEL $
  bcf     STATUS,IRP ; pasa a banco 0/1 para direccionamiento indirecto
; ----------------------------------------------------------------------------
NotificarAnalog1
  ; me fijo si tengo que notificar esta entrada
  movf    CountNotifA1,f
  btfsc   STATUS,Z
  goto    NotificarAnalog2
  ; decremento el contador de reintentos, si llega a cero --> chau
  decfsz  CountNotifA1,f
  goto    NotificarAnalog1Cont
  bcf     HSTONLINE   ; set off-line
  incf    Variant,f      ; incremento el contador de mensajes
  return
NotificarAnalog1Cont
  ; cargo el nuevo delay que se incrementa a medida que reintento
  movf    CountNotifA1,w
  sublw   REINT_DELAY
  movwf   DelayResend
  ; armo el mensaje
  movlw   '1'
  movwf   txdat1
  btfss   AN1_UMBRAL
  movlw   '0'
  btfsc   AN1_UMBRAL
  movlw   '1'
  movwf   txdat2
  movf    ANALOG1H,w
  movwf   txdat3
  movf    ANALOG1L,w
  movwf   txdat4
  ; cargo los valores de la alarma analogica
  movlw   ANALOG1_MAXH
  movwf   FSR
  movf    INDF,w      ; ANALOG1_MAXH
  movwf   txdat5
  incf    FSR,f
  movf    INDF,w      ; ANALOG1_MAXL
  movwf   txdat6
  incf    FSR,f
  movf    INDF,w      ; ANALOG1_MINH
  movwf   txdat7
  incf    FSR,f
  movf    INDF,w      ; ANALOG1_MINL
  movwf   txdat8
  goto    NotificarAnalogFin
; ----------------------------------------------------------------------------
NotificarAnalog2
  ; me fijo si tengo que notificar esta entrada
  movf    CountNotifA2,f
  btfsc   STATUS,Z
  goto    NotificarAnalog3
  ; decremento el contador de reintentos, si llega a cero --> chau
  decfsz  CountNotifA2,f
  goto    NotificarAnalog2Cont
  bcf     HSTONLINE   ; set off-line
  incf    Variant,f      ; incremento el contador de mensajes
  return
NotificarAnalog2Cont
  ; cargo el nuevo delay que se incrementa a medida que reintento
  movf    CountNotifA2,w
  sublw   REINT_DELAY
  movwf   DelayResend
  ; armo el mensaje
  movlw   '2'
  movwf   txdat1
  btfss   AN2_UMBRAL
  movlw   '0'
  btfsc   AN2_UMBRAL
  movlw   '1'
  movwf   txdat2
  movf    ANALOG2H,w
  movwf   txdat3
  movf    ANALOG2L,w
  movwf   txdat4
  ; cargo los valores de la alarma analogica
  movlw   ANALOG2_MAXH
  movwf   FSR
  movf    INDF,w      ; ANALOG1_MAXH
  movwf   txdat5
  incf    FSR,f
  movf    INDF,w      ; ANALOG1_MAXL
  movwf   txdat6
  incf    FSR,f
  movf    INDF,w      ; ANALOG1_MINH
  movwf   txdat7
  incf    FSR,f
  movf    INDF,w      ; ANALOG1_MINL
  movwf   txdat8
  goto    NotificarAnalogFin
; ----------------------------------------------------------------------------
NotificarAnalog3
  ; me fijo si tengo que notificar esta entrada
  movf    CountNotifA3,f
  btfsc   STATUS,Z
  goto    NotificarAnalog4
  ; decremento el contador de reintentos, si llega a cero --> chau
  decfsz  CountNotifA3,f
  goto    NotificarAnalog3Cont
  bcf     HSTONLINE   ; set off-line
  incf    Variant,f      ; incremento el contador de mensajes
  return
NotificarAnalog3Cont
  ; cargo el nuevo delay que se incrementa a medida que reintento
  movf    CountNotifA3,w
  sublw   REINT_DELAY
  movwf   DelayResend
  ; armo el mensaje
  movlw   '3'
  movwf   txdat1
  btfss   AN3_UMBRAL
  movlw   '0'
  btfsc   AN3_UMBRAL
  movlw   '1'
  movwf   txdat2
  movf    ANALOG3H,w
  movwf   txdat3
  movf    ANALOG3L,w
  movwf   txdat4
  ; cargo los valores de la alarma analogica
  movlw   ANALOG3_MAXH
  movwf   FSR
  movf    INDF,w      ; ANALOG1_MAXH
  movwf   txdat5
  incf    FSR,f
  movf    INDF,w      ; ANALOG1_MAXL
  movwf   txdat6
  incf    FSR,f
  movf    INDF,w      ; ANALOG1_MINH
  movwf   txdat7
  incf    FSR,f
  movf    INDF,w      ; ANALOG1_MINL
  movwf   txdat8
  goto    NotificarAnalogFin
; ----------------------------------------------------------------------------
NotificarAnalog4
  ; me fijo si tengo que notificar esta entrada
  movf    CountNotifA4,f
  btfsc   STATUS,Z
  return
  ; decremento el contador de reintentos, si llega a cero --> chau
  decfsz  CountNotifA4,f
  goto    NotificarAnalog4Cont
  bcf     HSTONLINE   ; set off-line
  incf    Variant,f      ; incremento el contador de mensajes
  return
NotificarAnalog4Cont
  ; cargo el nuevo delay que se incrementa a medida que reintento
  movf    CountNotifA4,w
  sublw   REINT_DELAY
  movwf   DelayResend
  ; armo el mensaje
  movlw   '4'
  movwf   txdat1
  btfss   AN4_UMBRAL
  movlw   '0'
  btfsc   AN4_UMBRAL
  movlw   '1'
  movwf   txdat2
  movf    ANALOG4H,w
  movwf   txdat3
  movf    ANALOG4L,w
  movwf   txdat4
  ; cargo los valores de la alarma analogica
  movlw   ANALOG4_MAXH
  movwf   FSR
  movf    INDF,w      ; ANALOG1_MAXH
  movwf   txdat5
  incf    FSR,f
  movf    INDF,w      ; ANALOG1_MAXL
  movwf   txdat6
  incf    FSR,f
  movf    INDF,w      ; ANALOG1_MINH
  movwf   txdat7
  incf    FSR,f
  movf    INDF,w      ; ANALOG1_MINL
  movwf   txdat8
  goto    NotificarAnalogFin
; ----------------------------------------------------------------------------
NotificarAnalogFin
  ; >>>> mensaje de notificacion >>>>
  movlw   'Q'        ; start char
  movwf   txtyp
  movlw   HSTADDR    ; dest addr
  movwf   txdst
  movf    LocAddr,w  ; local addr
  movwf   txsrc
  movf    Variant,w  ; contador
  movwf   txcnt
  movlw   '9'        ; comando - '9'
  movwf   txfcn
  ; le agrego la configuracion de la mascara de notificacion
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movf    MaskNotifAN,w
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movwf   txdat9
  ; le agrego la configuracion de seguidores
  movf    Mascara_ANS2,w
  movwf   txdat10
  ; <<<< mensaje de notificacion <<<<
  ; lo encola y termina
  PAGESEL TxEnqueue
  call    TxEnqueue
  PAGESEL $
  return
; ============================================================================
; NotificarOutput - procesa los cambios de estado de las salidas para
; transmitir
; ============================================================================
NotificarOutput
  ; inicializo el buffer
  PAGESEL ClrTx
  call    ClrTx
  PAGESEL $
; ----------------------------------------------------------------------------
NotificarOutput1
  ; me fijo si tengo que notificar esta salida
  movf    CountNotifS1,f
  btfsc   STATUS,Z
  goto    NotificarOutput2
  ; decremento el contador de reintentos, si llega a cero --> chau
  decfsz  CountNotifS1,f
  goto    NotificarOutput1Cont
  bcf     HSTONLINE   ; set off-line
  incf    Variant,f      ; incremento el contador de mensajes
  return
NotificarOutput1Cont
  ; cargo el nuevo delay que se incrementa a medida que reintento
  movf    CountNotifS1,w
  sublw   REINT_DELAY
  movwf   DelayResend
  ; armo el mensaje con los datos de la salida
  movlw   '1'
  movwf   txdat1
  movf    SALIDA1,w
  movwf   txdat2
  ; le agrego la configuracion de la mascara de notificacion
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movf    MaskNotifS1,w
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movwf   txdat3
  ; le agrego la configuracion de seguidores
  movf    Mascara_E1S1,w
  movwf   txdat4
  ; le agrego la configuracion de seguidores
  movlw   0
  movwf   txdat5
  goto    NotificarOutputFin
; ----------------------------------------------------------------------------
NotificarOutput2
  ; me fijo si tengo que notificar esta salida
  movf    CountNotifS2,f
  btfsc   STATUS,Z
  return
  ; decremento el contador de reintentos, si llega a cero --> chau
  decfsz  CountNotifS2,f
  goto    NotificarOutput2Cont
  bcf     HSTONLINE   ; set off-line
  incf    Variant,f      ; incremento el contador de mensajes
  return
NotificarOutput2Cont
  ; cargo el nuevo delay que se incrementa a medida que reintento
  movf    CountNotifS2,w
  sublw   REINT_DELAY
  movwf   DelayResend
  ; armo el mensaje con los datos de la salida
  movlw   '2'
  movwf   txdat1
  movf    SALIDA2,w
  movwf   txdat2
  ; le agrego la configuracion de la mascara de notificacion
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movf    MaskNotifS2,w
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movwf   txdat3
  ; le agrego la configuracion de seguidores
  movf    Mascara_E2S2,w
  movwf   txdat4
  ; le agrego la configuracion de seguidores
  movf    Mascara_ANS2,w
  movwf   txdat5
; ----------------------------------------------------------------------------
NotificarOutputFin
  ; >>>> mensaje de notificacion >>>>
  movlw   'Q'        ; start char
  movwf   txtyp
  movlw   HSTADDR    ; dest addr
  movwf   txdst
  movf    LocAddr,w  ; local addr
  movwf   txsrc
  movf    Variant,w  ; contador
  movwf   txcnt
  movlw   '7'        ; comando - '6'
  movwf   txfcn
  ; <<<< mensaje de notificacion <<<<
  ; lo encola y termina
  PAGESEL TxEnqueue
  call    TxEnqueue
  PAGESEL $
  return
; ============================================================================
 ERRORLEVEL +306
