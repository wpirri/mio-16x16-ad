; ============================================================================
; ProcessCmd - procesa un comando si es otra cosa lo deriva
; ============================================================================
ProcessCmd
  ; Reseteo el WFT
  movlw   0xFF
  movwf   CntTrafic
  ; me fijo si es para mi, broadcast u otro
  movf    rxdst,w      ; comparo con mi direccion
  subwf   LocAddr,w
  btfsc   STATUS,Z
  goto    ProcessCmdOk
  movf    rxdst,w      ; comparo con la direccion de broadcast
  sublw   BSTADDR
  btfss   STATUS,Z
  return               ; no era para mi ni broadcast
ProcessCmdOk
  ; limpio los datos de respuesta
  call    ClrTx
  ; llegó un mensaje, me fijo qué es
  movf    rxtyp,w
  sublw   'Q'
  btfss   STATUS,Z
  goto    ProcessResp
  ; es un requerimiento
  ; Controlo si es repetido
  bcf     DUPMSG
  movf    RxLastCnt,w
  subwf   rxcnt,w
  btfsc   STATUS,Z
  bsf     DUPMSG
  movf    rxcnt,w
  movwf   RxLastCnt
  ; salto por funcion
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'P'       ; -  Funcion 'P'
  btfsc   STATUS,Z  ; -     PING
  goto    FcnQP     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   '3'       ; -  Funcion '3'
  btfsc   STATUS,Z  ; -    SETBIT
  goto    FcnQ3     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   '4'       ; -  Funcion '4'
  btfsc   STATUS,Z  ; -    IRCODE
  goto    FcnQ4     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   '5'       ; -  Funcion '5'
  btfsc   STATUS,Z  ; -    PULSE
  goto    FcnQ5     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   '6'       ; -  Funcion '6'
  btfsc   STATUS,Z  ; -    GETIN
  goto    FcnQ6     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   '7'       ; -  Funcion '7'
  btfsc   STATUS,Z  ; -    GETOUT
  goto    FcnQ7     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   '8'       ; -  Funcion '8'
  btfsc   STATUS,Z  ; -   GETANALOG
  goto    FcnQ8     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'L'       ; -  Funcion 'L'
  btfsc   STATUS,Z  ; -    SETADDR
  goto    FcnQL     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'T'       ; -  Funcion 'T'
  btfsc   STATUS,Z  ; -    SETTIME
  goto    FcnQT     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'I'       ; -  Funcion 'I'
  btfsc   STATUS,Z  ; -    INVERT
  goto    FcnQI     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'R'       ; -  Funcion 'R'
  btfsc   STATUS,Z  ; -    RESET
  goto    FcnQR     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'B'       ; -  Funcion 'B'
  btfsc   STATUS,Z  ; -   BAUDRATE
  goto    FcnQB     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'F'       ; -  Funcion 'F'
  btfsc   STATUS,Z  ; -   Seguidores
  goto    FcnQF     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'O'       ; -  Funcion 'O'
  btfsc   STATUS,Z  ; -   Mascaras de notificacion
  goto    FcnQO     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'A'       ; -  Funcion 'A'
  btfsc   STATUS,Z  ; -   ALARM A/D
  goto    FcnQA     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'C'       ; -  Funcion 'C'
  btfsc   STATUS,Z  ; -   Play Beep
  goto    FcnQC     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'D'       ; -  Funcion 'D'
  btfsc   STATUS,Z  ; -   MSG-1L
  goto    FcnQD     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'E'       ; -  Funcion 'E'
  btfsc   STATUS,Z  ; -   MSG-1R
  goto    FcnQE     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'G'       ; -  Funcion 'G'
  btfsc   STATUS,Z  ; -   MSG-2L
  goto    FcnQG     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'H'       ; -  Funcion 'H'
  btfsc   STATUS,Z  ; -   MSG-2R
  goto    FcnQH     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'J'       ; -  Funcion 'J'
  btfsc   STATUS,Z  ; -   MSG-3L
  goto    FcnQJ     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'K'       ; -  Funcion 'K'
  btfsc   STATUS,Z  ; -   MSG-3R
  goto    FcnQK     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'M'       ; -  Funcion 'M'
  btfsc   STATUS,Z  ; -   MSG-4L
  goto    FcnQM     ; -
  movf    rxfcn,w   ; - - - - - - - -
  sublw   'N'       ; -  Funcion 'N'
  btfsc   STATUS,Z  ; -   MSG-4R
  goto    FcnQN     ; -
  movf    rxfcn,w  ; ----------------
  sublw   'Q'      ; -  Funcion 'Q'
  btfsc   STATUS,Z ; -    Display Config
  goto    FcnQQ    ; -
  ; comando desconocido, no hace nada
  return
; ============================================================================
; ProcessResp - procesa una respuesta si es otra cosa lo deriva
; ============================================================================
ProcessResp
  ; me fijo qué es
  movf    rxtyp,w
  sublw   'R'
  btfss   STATUS,Z
  goto    ProcessError
  clrf    DelayResend    ; una vez recibida respuesta no espero si tengo que 
  incf    Variant,f      ; incremento el contador de mensajes
  ; --> salto por función
  movf    rxfcn,w  ; ----------------
  sublw   'P'      ; -  Funcion 'P'
  btfsc   STATUS,Z ; -     PING
  goto    FcnRP    ; -
  movf    rxfcn,w  ; ----------------
  sublw   '6'      ; -  Funcion '6'
  btfsc   STATUS,Z ; -    INFO-IN
  goto    FcnR6    ; -
  movf    rxfcn,w  ; ----------------
  sublw   '7'      ; -  Funcion '7'
  btfsc   STATUS,Z ; -    INFO-OUT
  goto    FcnR7    ; -
  movf    rxfcn,w  ; ----------------
  sublw   '9'      ; -  Funcion '9'
  btfsc   STATUS,Z ; -    AD-ALARM
  goto    FcnR9    ; -
  movf    rxfcn,w  ; ----------------
  sublw   'T'      ; -  Funcion 'T'
  btfsc   STATUS,Z ; -    TIME
  goto    FcnRT    ; -
  ; para el resto por ahora no hago nada, despues veo que pongo acá
  return
; ============================================================================
; ProcessError - procesa un error si es otra cosa está mal
; ============================================================================
ProcessError
  ; llegó un error, me fijo qué es
  movf    rxtyp,w
  sublw   'E'
  btfss   STATUS,Z
  return  ; mensaje de tipo desconocido
  clrf    DelayResend    ; una vez recibida respuesta no espero si tengo que 
  incf    Variant,f      ; incremento el contador de mensajes

  ; es un error --> por ahora no hago nada, despues veo si pongo algo acá

  ; Algún dia voy a poner algo acá

  return

; ----------------------------------------------------------------------------
; FcnQOk - Respuesta de solo OK
; ----------------------------------------------------------------------------
FcnQOk
  ; si era un comando broadcast no contesto
  movf    rxdst,w
  sublw   BSTADDR
  btfsc   STATUS,Z
  return  ; era un mensaje de broadcast
  movlw   'R'        ; start char
  movwf   txtyp
  movf    rxsrc,w    ; dest addr
  movwf   txdst
  movf    rxcnt,w    ; contador
  movwf   txcnt
  movf    rxfcn,w    ; comando - el mismo recibido
  movwf   txfcn
  ; lo encola y termina
  goto    TxEnqueue

; ----------------------------------------------------------------------------
; FcnQError - Respuesta generica de error
; ----------------------------------------------------------------------------
FcnQError
  movlw   'E'        ; start char
  movwf   txtyp
  movf    rxsrc,w    ; dest addr
  movwf   txdst
  movf    rxcnt,w    ; contador
  movwf   txcnt
  movf    rxfcn,w    ; comando - el mismo recibido
  movwf   txfcn
  movf    ErrCde1,w  ; primer digito de respuesta
  movwf   txdat1
  ; lo encola y termina
  goto    TxEnqueue
; ============================================================================
; ============================================================================
; Tratamiento de requerimientos
; ----------------------------------------------------------------------------
; FcnQP - Ping
; ----------------------------------------------------------------------------
FcnQP
  ; el host está (LOGGED-ON)
  btfsc   HSTONLINE
  goto    FcnQP1       ; ya estaba logged-on
  ; tres pitidos
  call    Short3Beep
  ; flag logged-on
  bsf     HSTONLINE
  ; cambio la pantalla para que muestre la Screen1 (fecha/hora)
  movlw   0x01
  movwf   ScrnId
  bsf     DISPLAY_DATA
FcnQP1
  ; armo la respuesta al ping
  movlw   'R'        ; start char
  movwf   txtyp
  movf    rxsrc,w    ; dest addr
  movwf   txdst
  movf    rxcnt,w    ; contador
  movwf   txcnt
  movf    rxfcn,w    ; comando - el mismo recibido
  movwf   txfcn
  movlw   HARTYPE    ; tipo de dispositivo
  movwf   txdat1
  movlw   HARDVER    ; version de dispositivo
  movwf   txdat2
  movlw   SOFTVER    ; version de software
  movwf   txdat3
  movlw   PROCESSOR_VER ; micro procesador
  movwf   txdat4
  movlw   0             ; presencia de display
  btfsc   DISPLAY_OK
  movlw   0x50 ; display de 80 caracteres
  movwf   txdat5
  ; lo encola y termina
  goto    TxEnqueue
; ----------------------------------------------------------------------------
; FcnQ3 - SETBIT
; ----------------------------------------------------------------------------
FcnQ3
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
  ; limpio el error
  clrf    ErrCde1
  bcf     STATUS,IRP ; pasa a banco 0/1 para direccionamiento indirecto
  ; me abro por nro de salida
  ; salida 1
  movf    rxdat1,w
  sublw   '1'
  btfsc   STATUS,Z
  goto    FcnQ3s1
  ; salida 2
  movf    rxdat1,w
  sublw   '2'
  btfsc   STATUS,Z
  goto    FcnQ3s2
  ; si no es ninguna de las contempladas es un error
  goto    FcnQError
FcnQ3s1
  ; cargo la direccion de la salida 1 en el registro de direccionamiento ind
  movlw   SALIDA1
  movwf   FSR
  ; levanto el flag de modificación
  bsf     CAMBIO_S1
  goto    FcnQ3Set
FcnQ3s2
  ; cargo la direccion de la salida 2 en el registro de direccionamiento ind
  movlw   SALIDA2
  movwf   FSR
  ; levanto el flag de modificación
  bsf     CAMBIO_S2
; goto    FcnQ3Set
FcnQ3Set
  ; voy incrementando ErrCde1 para que en caso
  ; de error pueda saber en que etapa se produjo
  incf    ErrCde1,f
  ; calculo el salto en función del bit a encender o apagar
  movlw   '0'
  subwf   rxdat2,w
  movwf   RegTmpG
  ; verifico que no sea mayor a 7
  andlw   0xF8
  btfss   STATUS,Z
  goto    FcnQError
  ; voy incrementando ErrCde1 para que en caso
  ; de error pueda saber en que etapa se produjo
  incf    ErrCde1,f
  ; multiplico por 4 para provocar un salto del cuadruple
  ; borro el carry para que no se me meta en el LSB
  bcf     STATUS,C
  rlf     RegTmpG,f
  bcf     STATUS,C
  rlf     RegTmpG,f
  ; ahora me fijo si es encender o apagar
  movlw   '0'
  subwf   rxdat3,w
  btfsc   STATUS,Z
  goto    FcnQ3SetA ; si es apagar no hay que calcular mas nada
  movlw   '1'
  subwf   rxdat3,w
  btfss   STATUS,Z
  goto    FcnQError ; si no es ni '0' ni '1' es un error
  movlw   0x02
  addwf   RegTmpG,f
FcnQ3SetA
; FcnQ3SetB - movido a la zona de tablas
  call    FcnQ3SetB
  ; listo, grabo en EE en 5 segundos
  movlw   0x05
  movwf   SaveEECount
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQ4 - IRCODE
; ----------------------------------------------------------------------------
FcnQ4
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
  ; limpio el error
  clrf    ErrCde1
  ; muevo el código al buffer infra-rojo
 ERRORLEVEL -302
  movf    rxdat1,w
  bsf     STATUS,RP0 ; --> BANK1
  movwf   IRCode1
  bcf     STATUS,RP0 ; --> BANK0
  movf    rxdat2,w
  bsf     STATUS,RP0 ; --> BANK1
  movwf   IRCode2
  bcf     STATUS,RP0 ; --> BANK0
  movf    rxdat3,w
  bsf     STATUS,RP0 ; --> BANK1
  movwf   IRCode3
  bcf     STATUS,RP0 ; --> BANK0
  movf    rxdat4,w
  bsf     STATUS,RP0 ; --> BANK1
  movwf   IRCode4
  bcf     STATUS,RP0 ; --> BANK0
  movf    rxdat5,w
  bsf     STATUS,RP0 ; --> BANK1
  movwf   IRCode5
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  bsf     SEND_IR    ; para que transmita
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQ5 - PULSE
; ----------------------------------------------------------------------------
FcnQ5
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
  ; limpio el error
  clrf    ErrCde1
  bcf     STATUS,IRP ; pasa a banco 0/1 para direccionamiento indirecto
  ; calculo el nro de bit a modificar
  movlw   '0'
  subwf   rxdat2,w
  ; lo guardo en un temporal (lo voy a usar varias veces)
  movwf   RegTmpG
  ; verifico que no sea mayor a 7
  andlw   0xF8
  btfss   STATUS,Z
  goto    FcnQError
  ; voy incrementando ErrCde1 para que en caso
  ; de error pueda saber en qué etapa se produjo
  incf    ErrCde1,f
  ; me abro por nro de salida
  movf    rxdat1,w
  sublw   '1'
  btfsc   STATUS,Z
  goto    FcnQ5s1
  movf    rxdat1,w
  sublw   '2'
  btfsc   STATUS,Z
  goto    FcnQ5s2
  ; si no es ni '1' ni '2' es un error
  goto    FcnQError
FcnQ5s1
  ; seteo el contador para el retorno del pulso con el valor del 3er dato
  movf    RegTmpG,w
  call    GetPulseCountS1
  movwf   FSR
  movf    rxdat3,w
  movwf   INDF
  ; cargo la direccion de la salida 1 en el registro de direccionamiento ind
  movlw   SALIDA1
  movwf   FSR
  ; levanto el flag de modificación
  bsf     CAMBIO_S1
  goto    FcnQ5Set
FcnQ5s2
  ; seteo el contador para el retorno del pulso con el valor del 3er dato
  movf    RegTmpG,w
  call    GetPulseCountS2
  movwf   FSR
  movf    rxdat3,w
  movwf   INDF
  ; cargo la direccion de la salida 2 en el registro de direccionamiento ind
  movlw   SALIDA2
  movwf   FSR
  ; levanto el flag de modificación
  bsf     CAMBIO_S2
FcnQ5Set
  ; levanto el bit seleccionado
  movf    RegTmpG,w
  call    SetWBitW
  iorwf   INDF,f
  ; listo, grabo en EE en 5 segundos
  movlw   0x05
  movwf   SaveEECount
  ; contesto
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQ6 - GETIN
; ----------------------------------------------------------------------------
FcnQ6
  clrf    ErrCde1
  ; preparo la respuesta
  movf    rxdat1,w
  movwf   txdat1
  ; me abro por nro de entrada
  movf    rxdat1,w
  sublw   '1'
  btfsc   STATUS,Z
  goto    FcnQ6e1
  movf    rxdat1,w
  sublw   '2'
  btfsc   STATUS,Z
  goto    FcnQ6e2
  ; si no es ni '1' ni '2' es un error
  goto    FcnQError
FcnQ6e1
  ; cargo el valor de la entrada
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
  goto    FcnQOk
FcnQ6e2
  ; cargo el valor de la entrada
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
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQ7 - GETOUT
; ----------------------------------------------------------------------------
FcnQ7
  clrf    ErrCde1
  ; preparo la respuesta
  movf    rxdat1,w
  movwf   txdat1
  ; me abro por nro de salida
  movf    rxdat1,w
  sublw   '1'
  btfsc   STATUS,Z
  goto    FcnQ7s1
  movf    rxdat1,w
  sublw   '2'
  btfsc   STATUS,Z
  goto    FcnQ7s2
  ; si no es ni '1' ni '2' es un error
  goto    FcnQError
FcnQ7s1
  ; cargo el valor de la salida
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
  goto    FcnQOk
FcnQ7s2
  ; cargo el valor de la salida
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
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQ8 - GETANALOG
; ----------------------------------------------------------------------------
FcnQ8
  clrf    ErrCde1
  bcf     STATUS,IRP ; pasa a banco 0/1 para direccionamiento indirecto
  ; preparo la respuesta
  movf    rxdat1,w
  movwf   txdat1
  ; me abro por nro de salida
  movf    rxdat1,w
  sublw   '1'
  btfsc   STATUS,Z
  goto    FcnQ8e1
  movf    rxdat1,w
  sublw   '2'
  btfsc   STATUS,Z
  goto    FcnQ8e2
  movf    rxdat1,w
  sublw   '3'
  btfsc   STATUS,Z
  goto    FcnQ8e3
  movf    rxdat1,w
  sublw   '4'
  btfsc   STATUS,Z
  goto    FcnQ8e4
  ; si no está entre 1 y 4 está mal
  goto    FcnQError
FcnQ8e1
  ; cargo los valores de la entrada analogica
  movf    ANALOG1H,w
  movwf   txdat2
  movf    ANALOG1L,w
  movwf   txdat3
  ; cargo los valores de la alarma analogica
  movlw   ANALOG1_MAXH
  movwf   FSR
  movf    INDF,w      ; ANALOG1_MAXH
  movwf   txdat4
  incf    FSR,f
  movf    INDF,w      ; ANALOG1_MAXL
  movwf   txdat5
  incf    FSR,f
  movf    INDF,w      ; ANALOG1_MINH
  movwf   txdat6
  incf    FSR,f
  movf    INDF,w      ; ANALOG1_MINL
  movwf   txdat7
  btfss   AN1_UMBRAL
  movlw   '0'
  btfsc   AN1_UMBRAL
  movlw   '1'
  movwf   txdat8
  goto    FcnQ8Fin
FcnQ8e2
  ; cargo los valores de la entrada analogica
  movf    ANALOG2H,w
  movwf   txdat2
  movf    ANALOG2L,w
  movwf   txdat3
  ; cargo los valores de la alarma analogica
  movlw   ANALOG2_MAXH
  movwf   FSR
  movf    INDF,w      ; ANALOG2_MAXH
  movwf   txdat4
  incf    FSR,f
  movf    INDF,w      ; ANALOG2_MAXL
  movwf   txdat5
  incf    FSR,f
  movf    INDF,w      ; ANALOG2_MINH
  movwf   txdat6
  incf    FSR,f
  movf    INDF,w      ; ANALOG2_MINL
  movwf   txdat7
  btfss   AN2_UMBRAL
  movlw   '0'
  btfsc   AN2_UMBRAL
  movlw   '1'
  movwf   txdat8
  goto    FcnQ8Fin
FcnQ8e3
  ; cargo los valores de la entrada analogica
  movf    ANALOG3H,w
  movwf   txdat2
  movf    ANALOG3L,w
  movwf   txdat3
  ; cargo los valores de la alarma analogica
  movlw   ANALOG3_MAXH
  movwf   FSR
  movf    INDF,w      ; ANALOG3_MAXH
  movwf   txdat4
  incf    FSR,f
  movf    INDF,w      ; ANALOG3_MAXL
  movwf   txdat5
  incf    FSR,f
  movf    INDF,w      ; ANALOG3_MINH
  movwf   txdat6
  incf    FSR,f
  movf    INDF,w      ; ANALOG3_MINL
  movwf   txdat7
  btfss   AN3_UMBRAL
  movlw   '0'
  btfsc   AN3_UMBRAL
  movlw   '1'
  movwf   txdat8
  goto    FcnQ8Fin
FcnQ8e4
  ; cargo los valores de la entrada analogica
  movf    ANALOG4H,w
  movwf   txdat2
  movf    ANALOG4L,w
  movwf   txdat3
  ; cargo los valores de la alarma analogica
  movlw   ANALOG4_MAXH
  movwf   FSR
  movf    INDF,w      ; ANALOG4_MAXH
  movwf   txdat4
  incf    FSR,f
  movf    INDF,w      ; ANALOG4_MAXL
  movwf   txdat5
  incf    FSR,f
  movf    INDF,w      ; ANALOG4_MINH
  movwf   txdat6
  incf    FSR,f
  movf    INDF,w      ; ANALOG4_MINL
  movwf   txdat7
  btfss   AN4_UMBRAL
  movlw   '0'
  btfsc   AN4_UMBRAL
  movlw   '1'
  movwf   txdat8
FcnQ8Fin
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
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQL - SETADDR
; ----------------------------------------------------------------------------
FcnQL
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
  ; actualizo la direccion local
  movf    rxdat1,w
  movwf   LocAddr
  ; listo, grabo en EE en 5 segundos
  movlw   0x05
  movwf   SaveEECount
  ; cargo el timer para el reset
  movlw   0x0A         ; 10 segundos
  movwf   ResetCount
  ; contesto Ok generico
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQT - SETDATETIME
; ----------------------------------------------------------------------------
FcnQT
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
  ; actualizo la hora
  movf    rxdat1,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   LocHora
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movf    rxdat2,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   LocMin
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movf    rxdat3,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   LocSeg
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movf    rxdat4,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   LocDia
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movf    rxdat5,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   LocFecha
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movf    rxdat6,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   LocMes
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movf    rxdat7,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   LocAnio
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  ; para que actualice el display cuando pueda
  bsf     DISPLAY_DATA
  bsf     DATE_OK
  ; contesto Ok generico
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQR - RESET
; ----------------------------------------------------------------------------
FcnQR
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
  ; cargo el timer para el reset
  movlw   0x0A         ; 10 segundos
  movwf   ResetCount
  ; contesto Ok generico
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQI - INVERT
; ----------------------------------------------------------------------------
FcnQI
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
  ; limpio el error
  clrf    ErrCde1
  bcf     STATUS,IRP ; pasa a banco 0/1 para direccionamiento indirecto
  ; me abro por nro de salida
  ; salida 1
  movf    rxdat1,w
  sublw   '1'
  btfsc   STATUS,Z
  goto    FcnQIs1
  ; salida 2
  movf    rxdat1,w
  sublw   '2'
  btfsc   STATUS,Z
  goto    FcnQIs2
  ; si no es ninguna de las contempladas es un error
  goto    FcnQError
FcnQIs1
  ; cargo la direccion de la salida 1 en el registro de direccionamiento ind
  movlw   SALIDA1
  movwf   FSR
  ; levanto el flag de modificación
  bsf     CAMBIO_S1
  goto    FcnQIOper
FcnQIs2
  ; cargo la direccion de la salida 2 en el registro de direccionamiento ind
  movlw   SALIDA2
  movwf   FSR
  ; levanto el flag de modificación
  bsf     CAMBIO_S2
; goto    FcnQIOper
FcnQIOper
  ; voy incrementando ErrCde1 para que en caso
  ; de error pueda saber en qué etapa se produjo
  incf    ErrCde1,f
  ; calculo bit a encender o apagar
  movlw   '0'
  subwf   rxdat2,w
  movwf   RegTmpG
  ; verifico que no sea mayor a 7
  andlw   0xF8
  btfss   STATUS,Z
  goto    FcnQError
  ; voy incrementando ErrCde1 para que en caso
  ; de error pueda saber en qué etapa se produjo
  incf    ErrCde1,f
  movf    RegTmpG,w
  call    SetWBitW
  xorwf   INDF,f      ; invierto el bit seleccionado
  ; listo, grabo en EE en 5 segundos
  movlw   0x05
  movwf   SaveEECount
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQB - BAUDRATE
; ----------------------------------------------------------------------------
FcnQB
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
  ; actualizo el Baudrate (se activa al resetear)
  movf    rxdat1,w
  movwf   BaudRate
  ; listo, grabo en EE en 5 segundos
  movlw   0x05
  movwf   SaveEECount
  ; cargo el timer para el reset
  movlw   0x0A         ; 10 segundos
  movwf   ResetCount
  ; contesto Ok generico
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQA - Alarmas A/D
; ----------------------------------------------------------------------------
FcnQA
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
  ; limpio el error
  clrf    ErrCde1
  bcf     STATUS,IRP ; pasa a banco 0/1 para direccionamiento indirecto
  ; me abro por nro de salida
  movf    rxdat1,w
  sublw   '1'
  btfsc   STATUS,Z
  goto    FcnQA1
  movf    rxdat1,w
  sublw   '2'
  btfsc   STATUS,Z
  goto    FcnQA2
  movf    rxdat1,w
  sublw   '3'
  btfsc   STATUS,Z
  goto    FcnQA3
  movf    rxdat1,w
  sublw   '4'
  btfsc   STATUS,Z
  goto    FcnQA4
  ; si no está entre 1 y 5 está mal
  goto    FcnQError
; -----------------------
FcnQA1
  movlw   ANALOG1_MAXH
  movwf   FSR
  goto    FcnQASet
FcnQA2
  movlw   ANALOG2_MAXH
  movwf   FSR
  goto    FcnQASet
FcnQA3
  movlw   ANALOG3_MAXH
  movwf   FSR
  goto    FcnQASet
FcnQA4
  movlw   ANALOG4_MAXH
  movwf   FSR
  goto    FcnQASet
; -----------------------
FcnQASet
  movf    rxdat2,w
  movwf   INDF
  incf    FSR,f
  movf    rxdat3,w
  movwf   INDF
  incf    FSR,f
  movf    rxdat4,w
  movwf   INDF
  incf    FSR,f
  movf    rxdat5,w
  movwf   INDF
  ; listo, grabo en EE en 5 segundos
  movlw   0x05
  movwf   SaveEECount
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQF - Seguidores
; ----------------------------------------------------------------------------
FcnQF
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
  ; limpio el error
  clrf    ErrCde1
  ; me abro por nro de entrada
  ; entrada 1
  movf    rxdat1,w
  sublw   '1'
  btfsc   STATUS,Z
  goto    FcnQFe1
  ; entrada 2
  movf    rxdat1,w
  sublw   '2'
  btfsc   STATUS,Z
  goto    FcnQFe2
  ; entrada analogica
  movf    rxdat1,w
  sublw   'A'
  btfsc   STATUS,Z
  goto    FcnQFeA
  ; si no es ninguna de las contempladas es un error
  goto    FcnQError
FcnQFe1
  ; muevo el dato a a mascara
  movf    rxdat2,w
  movwf   Mascara_E1S1
  bsf     CAMBIO_E1
  goto    FcnQFEnd
FcnQFe2
  ; muevo el dato a a mascara
  movf    rxdat2,w
  movwf   Mascara_E2S2
  bsf     CAMBIO_E2
  goto    FcnQFEnd
FcnQFeA
  ; muevo el dato a a mascara
  movf    rxdat2,w
  andlw   0x0F        ; para analogicos solamente los primeros 4 bits
  movwf   Mascara_ANS2
  bsf     CAMBIO_AN
;  goto    FcnQFEnd
FcnQFEnd
  ; listo, grabo en EE en 5 segundos
  movlw   0x05
  movwf   SaveEECount
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQO - Mascara de notificación
; ----------------------------------------------------------------------------
FcnQO
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
  ; limpio el error
  clrf    ErrCde1
  ; me abro por nro de entrada
  ; entrada 1
  movf    rxdat1,w
  sublw   '1'
  btfsc   STATUS,Z
  goto    FcnQOe1
  ; entrada 2
  movf    rxdat1,w
  sublw   '2'
  btfsc   STATUS,Z
  goto    FcnQOe2
  ; salida 1
  movf    rxdat1,w
  sublw   '3'
  btfsc   STATUS,Z
  goto    FcnQOs1
  ; salida 2
  movf    rxdat1,w
  sublw   '4'
  btfsc   STATUS,Z
  goto    FcnQOs2
  ; entrada analogica
  movf    rxdat1,w
  sublw   'A'
  btfsc   STATUS,Z
  goto    FcnQOeA
  ; si no es ninguna de las contempladas es un error
  goto    FcnQError
FcnQOe1
  ; muevo el dato a a mascara
  movf    rxdat2,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   MaskNotifE1
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  goto    FcnQOEnd
FcnQOe2
  ; muevo el dato a a mascara
  movf    rxdat2,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   MaskNotifE2
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  goto    FcnQOEnd
FcnQOs1
  ; muevo el dato a a mascara
  movf    rxdat2,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   MaskNotifS1
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  goto    FcnQOEnd
FcnQOs2
  ; muevo el dato a a mascara
  movf    rxdat2,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   MaskNotifS2
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  goto    FcnQOEnd
FcnQOeA
  ; muevo el dato a a mascara
  movf    rxdat2,w
  andlw   0x0F        ; para analogicos solamente los primeros 4 bits
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   MaskNotifAN
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
;  goto    FcnQOEnd
FcnQOEnd
  ; listo, grabo en EE en 5 segundos
  movlw   0x05
  movwf   SaveEECount
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQC - Produce beeps por el buzzer on-board
; ----------------------------------------------------------------------------
FcnQC
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
  ; si el primer parametro es cero es un beep largo
  ; si es mayor es la cantidad de beeps cortos
  movlw   '0'
  subwf   rxdat1,w
  btfsc   STATUS,Z
  goto    FcnQC_LongBeep
  call    NumBeep       ; en W ya esta la cantidad de beeps
  goto    FcnQOk
FcnQC_LongBeep
  call    LongBeep
  goto    FcnQOk

; ----------------------------------------------------------------------------
; >> Mensajes en el display
;    DDDDDDDDDDEEEEEEEEEE
;    GGGGGGGGGGHHHHHHHHHH
;    JJJJJJJJJJKKKKKKKKKK
;    MMMMMMMMMMNNNNNNNNNN
; ----------------------------------------------------------------------------
; FcnQD - Mueve lo recibido a la primera mitad de la linea 1 del LCD
; ----------------------------------------------------------------------------
FcnQD
  ; Si el display no está es un error
  clrf    ErrCde1
  btfss   DISPLAY_OK
  goto    FcnQError
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
 ERRORLEVEL -306
  PAGESEL LCD_LINE1
  call    LCD_LINE1
  call    RxBuffer2LCD
  PAGESEL $
 ERRORLEVEL +306
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQE - Mueve lo recibido a la segunda mitad de la linea 1 del LCD
; ----------------------------------------------------------------------------
FcnQE
  ; Si el display no está es un error
  clrf    ErrCde1
  btfss   DISPLAY_OK
  goto    FcnQError
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
 ERRORLEVEL -306
  PAGESEL LCD_LINE1R
  call    LCD_LINE1R
  call    RxBuffer2LCD
  PAGESEL $
 ERRORLEVEL +306
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQG - Mueve lo recibido a la primera mitad de la linea 2 del LCD
; ----------------------------------------------------------------------------
FcnQG
  ; Si el display no está es un error
  clrf    ErrCde1
  btfss   DISPLAY_OK
  goto    FcnQError
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
 ERRORLEVEL -306
  PAGESEL LCD_LINE2
  call    LCD_LINE2
  call    RxBuffer2LCD
  PAGESEL $
 ERRORLEVEL +306
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQH - Mueve lo recibido a la segunda mitad de la linea 2 del LCD
; ----------------------------------------------------------------------------
FcnQH
  ; Si el display no está es un error
  clrf    ErrCde1
  btfss   DISPLAY_OK
  goto    FcnQError
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
 ERRORLEVEL -306
  PAGESEL LCD_LINE2R
  call    LCD_LINE2R
  call    RxBuffer2LCD
  PAGESEL $
 ERRORLEVEL +306
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQJ - Mueve lo recibido a la primera mitad de la linea 3 del LCD
; ----------------------------------------------------------------------------
FcnQJ
  ; Si el display no está es un error
  clrf    ErrCde1
  btfss   DISPLAY_OK
  goto    FcnQError
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
 ERRORLEVEL -306
  PAGESEL LCD_LINE3
  call    LCD_LINE3
  call    RxBuffer2LCD
  PAGESEL $
 ERRORLEVEL +306
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQK - Mueve lo recibido a la segunda mitad de la linea 3 del LCD
; ----------------------------------------------------------------------------
FcnQK
  ; Si el display no está es un error
  clrf    ErrCde1
  btfss   DISPLAY_OK
  goto    FcnQError
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
 ERRORLEVEL -306
  PAGESEL LCD_LINE3R
  call    LCD_LINE3R
  call    RxBuffer2LCD
  PAGESEL $
 ERRORLEVEL +306
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQM - Mueve lo recibido a la primera mitad de la linea 4 del LCD
; ----------------------------------------------------------------------------
FcnQM
  ; Si el display no está es un error
  clrf    ErrCde1
  btfss   DISPLAY_OK
  goto    FcnQError
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
 ERRORLEVEL -306
  PAGESEL LCD_LINE4
  call    LCD_LINE4
  call    RxBuffer2LCD
  PAGESEL $
 ERRORLEVEL +306
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQN - Mueve lo recibido a la segunda mitad de la linea 4 del LCD
; ----------------------------------------------------------------------------
FcnQN
  ; Si el display no está es un error
  clrf    ErrCde1
  btfss   DISPLAY_OK
  goto    FcnQError
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
 ERRORLEVEL -306
  PAGESEL LCD_LINE4R
  call    LCD_LINE4R
  call    RxBuffer2LCD
  PAGESEL $
 ERRORLEVEL +306
  goto    FcnQOk
; ----------------------------------------------------------------------------
; FcnQQ - Display config
; ----------------------------------------------------------------------------
FcnQQ
  ; si es duplicada contesto sin hacer nada
  btfsc   DUPMSG
  goto    FcnQOk
  ; habilitacion del display
  movlw   '0'
  subwf   rxdat1,w
  btfsc   STATUS,Z
  bcf     DISPLAY_OK
  btfss   STATUS,Z
  bsf     DISPLAY_OK
  ; etc.


  ; listo, grabo en EE en 5 segundos
  movlw   0x05
  movwf   SaveEECount
  ; contesto Ok generico
  goto    FcnQOk
; ============================================================================
; ============================================================================
; Tratamiento de respuestas
; ----------------------------------------------------------------------------
; FcnRP - Ping
; ----------------------------------------------------------------------------
FcnRP
  ; es una respuesta
  ; el host está (LOGGED-ON)
  btfsc   HSTONLINE
  return
  ; tres pitidos
  call    Short3Beep
  ; flag logged-on
  bsf     HSTONLINE
  ; cambio la pantalla para que muestre la Screen1 (fecha/hora)
  movlw   0x01
  movwf   ScrnId
  bsf     DISPLAY_DATA
  return
; ----------------------------------------------------------------------------
; FcnR6 - INFO-IN
; ----------------------------------------------------------------------------
FcnR6
  ; me abro por nro de entrada
  movf    rxdat1,w
  sublw   '1'
  btfsc   STATUS,Z
  goto    FcnR6e1
  movf    rxdat1,w
  sublw   '2'
  btfsc   STATUS,Z
  goto    FcnR6e2
  return
FcnR6e1
  clrf    CountNotifE1
  ; comparo los datos de la respuesta con el estado actual de la entrada
  movf    rxdat2,w
  subwf   ENTRADA1,w
  btfsc   STATUS,Z
  return
  ; si hay diferencia notifico de nuevo
  movlw   REINTENTOS
  movwf   CountNotifE1
  return
FcnR6e2
  clrf    CountNotifE2
  ; comparo los datos de la respuesta con el estado actual de la entrada
  movf    rxdat2,w
  subwf   ENTRADA2,w
  btfsc   STATUS,Z
  return
  ; si hay diferencia notifico de nuevo
  movlw   REINTENTOS
  movwf   CountNotifE2
  return
; ----------------------------------------------------------------------------
; FcnR7 - INFO-OUT
; ----------------------------------------------------------------------------
FcnR7
  ; me abro por nro de salida
  movf    rxdat1,w
  sublw   '1'
  btfsc   STATUS,Z
  goto    FcnR7s1
  movf    rxdat1,w
  sublw   '2'
  btfsc   STATUS,Z
  goto    FcnR7s2
  return
FcnR7s1
  clrf    CountNotifS1
  ; comparo los datos de la respuesta con el estado actual de la entrada
  movf    rxdat2,w
  subwf   SALIDA1,w
  btfsc   STATUS,Z
  return
  ; si hay diferencia notifico de nuevo
  movlw   REINTENTOS
  movwf   CountNotifS1
  return
FcnR7s2
  clrf    CountNotifS2
  ; comparo los datos de la respuesta con el estado actual de la entrada
  movf    rxdat2,w
  subwf   SALIDA2,w
  btfsc   STATUS,Z
  return
  ; si hay diferencia notifico de nuevo
  movlw   REINTENTOS
  movwf   CountNotifS2
  return
; ----------------------------------------------------------------------------
; FcnR9 - NOTIF ANALOG
; ----------------------------------------------------------------------------
FcnR9
  ; me abro por nro de entrada
  movf    rxdat1,w
  sublw   '1'
  btfsc   STATUS,Z
  goto    FcnR9a1
  movf    rxdat1,w
  sublw   '2'
  btfsc   STATUS,Z
  goto    FcnR9a2
  movf    rxdat1,w
  sublw   '3'
  btfsc   STATUS,Z
  goto    FcnR9a3
  movf    rxdat1,w
  sublw   '4'
  btfsc   STATUS,Z
  goto    FcnR9a4
  return
FcnR9a1
  clrf    CountNotifA1
  ; comparo los datos de la respuesta con el estado actual de la alarma
  btfss   AN1_UMBRAL
  movlw   '0'
  btfsc   AN1_UMBRAL
  movlw   '1'
  subwf   rxdat2,w
  btfsc   STATUS,Z
  return
  ; si hay diferencia notifico de nuevo
  movlw   REINTENTOS
  movwf   CountNotifA1
  return
FcnR9a2
  clrf    CountNotifA2
  ; comparo los datos de la respuesta con el estado actual de la alarma
  btfss   AN2_UMBRAL
  movlw   '0'
  btfsc   AN2_UMBRAL
  movlw   '1'
  subwf   rxdat2,w
  btfsc   STATUS,Z
  return
  ; si hay diferencia notifico de nuevo
  movlw   REINTENTOS
  movwf   CountNotifA2
  return
FcnR9a3
  clrf    CountNotifA3
  ; comparo los datos de la respuesta con el estado actual de la alarma
  btfss   AN3_UMBRAL
  movlw   '0'
  btfsc   AN3_UMBRAL
  movlw   '1'
  subwf   rxdat2,w
  btfsc   STATUS,Z
  return
  ; si hay diferencia notifico de nuevo
  movlw   REINTENTOS
  movwf   CountNotifA3
  return
FcnR9a4
  clrf    CountNotifA4
  ; comparo los datos de la respuesta con el estado actual de la alarma
  btfss   AN4_UMBRAL
  movlw   '0'
  btfsc   AN4_UMBRAL
  movlw   '1'
  subwf   rxdat2,w
  btfsc   STATUS,Z
  return
  ; si hay diferencia notifico de nuevo
  movlw   REINTENTOS
  movwf   CountNotifA4
  return
; ----------------------------------------------------------------------------
; FcnRT - SETDATETIME
; ----------------------------------------------------------------------------
FcnRT
  ; actualizo la hora
  movf    rxdat1,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   LocHora
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movf    rxdat2,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   LocMin
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movf    rxdat3,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   LocSeg
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movf    rxdat4,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   LocDia
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movf    rxdat5,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   LocFecha
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movf    rxdat6,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   LocMes
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  movf    rxdat7,w
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movwf   LocAnio
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  ; para que actualice el display cuando pueda
  bsf     DISPLAY_DATA
  bsf     DATE_OK
  return
; ============================================================================
