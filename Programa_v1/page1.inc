; gpasm cuando se hace un salto a una posicion que no está en la página 1
;       se queja aunque el destino del salto esté en la misma página que
;       el origen
 ERRORLEVEL -306
; =.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=
;
;   PAGE 1 - PAGE 1 - PAGE 1 - PAGE 1 - PAGE 1 - PAGE 1 - PAGE 1 - PAGE 1
;
; =.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=
  org 0x0800    ; cambio de página de memoria de programa (800h-FFFh)
; ============================================================================
; ADCheck - En zona de tablas
; ============================================================================
ADCheck
  ; me fijo si estoy en linea
  btfss   SYSREADY
  return
  ; me fijo si está encendido el conversor
  btfss   ADCON0,ADON
  goto    ADEnable
  ; me fijo si no estoy esperando para activar el conversor
  movf    ConvertDelay,f
  btfss   STATUS,Z
  return
  ; me fijo si no tengo una conversión en curso
  btfsc   ADCON0,GO
  return                ; todavía está convirtiendo
ADChanTbl
  ; hago un salto según el canal que estoy leyendo
  movf    ADCON0,w
 ifdef  __16F887
  andlw   0x1C          ; filtro CHS0..CHS2
  movwf   RegTmpI
  ; borro el carry para que no se me meta en el MSB
  bcf     STATUS,C
 else
  andlw   0x38          ; filtro CHS0..CHS2
  movwf   RegTmpI
  ; borro el carry para que no se me meta en el MSB
  bcf     STATUS,C
  rrf     RegTmpI,f
 endif
  bcf     STATUS,C
  rrf     RegTmpI,f
  bcf     STATUS,C
  rrf     RegTmpI,f
  movf    RegTmpI,w
  addwf   PCL,f
  goto    ADChan1
  goto    ADChan2
  goto    ADChan3
  goto    ADChan4
; ============================================================================
; DEC2ASCII - Convierte el valor numerico en W a su numero ascii
; ============================================================================
DEC2ASCII
  addwf   PCL,f
  retlw   '0'
  retlw   '1'
  retlw   '2'
  retlw   '3'
  retlw   '4'
  retlw   '5'
  retlw   '6'
  retlw   '7'
  retlw   '8'
  retlw   '9'
  retlw   'A'
  retlw   'B'
  retlw   'C'
  retlw   'D'
  retlw   'E'
  retlw   'F'
; ============================================================================
PrintDiaSemana
  addwf   PCL,f
  goto    PrintLunes
  goto    PrintMartes
  goto    PrintMiercoles
  goto    PrintJueves
  goto    PrintViernes
  goto    PrintSabado
  goto    PrintDomingo
; ============================================================================
PrintMes
  addwf   PCL,f
  goto    PrintEnero
  goto    PrintFebrero
  goto    PrintMarzo
  goto    PrintAbril
  goto    PrintMayo
  goto    PrintJunio
  goto    PrintJulio
  goto    PrintAgosto
  goto    PrintSeptiembre
  goto    PrintOctubre
  goto    PrintNoviembre
  goto    PrintDiciembre
; ============================================================================
SelectScreen
  addwf   PCL,f
  goto    Screen0     ; Offline
  goto    Screen1     ; Default
  return
  return
  return
  return
  return
  return
  return
; ============================================================================


; ============================================================================
; FIN - Zona de TABLAS
  org     0x0900
; ============================================================================
; ============================================================================
; Interrupción del timer (cada 278 us)
; ============================================================================
TimerInt
  bcf     INTCON,T0IF     ;borro el flag para que no se redispare
  ;
  decfsz  TDIV1,f         ; divido las interrupciones
  return
  movlw   DIVISOR1
  movwf   TDIV1
  ; >> cada 10 ms
  call    ControlBeep
  call    RxTimeOutCtrl
  ;call    CheckBtn
  call    CtrlStartAD
  ; decremento el control de timeout del display
  movf    DisplayTO,f
  btfss   STATUS,Z
  decf    DisplayTO,f
  ; Lecturas analógicas
  call    ADCheck
  ; <<
  decfsz  TDIV2,f         ; divido las interrupciones
  return
  movlw   DIVISOR2
  movwf   TDIV2
  ; >> cada 1s
  btfss   SYSREADY
  call    StartCtrl
  btfsc   HSTONLINE
  call    CtrlTrafic
  call    CheckPulse
  ; Decremento si es distinto de cero
  movf    DelayResend,f
  btfss   STATUS,Z
  decf    DelayResend,f
  ; Control para refrescar el display
  call    DisplayCtrl
  ; Control para grabar config
  call    SaveEECtrl
  ; Mantenimiento del reloj local
  call    ClockMant
  ; Control de reset por soft
  call    ResetCtrl

  ; <<
  return
; ============================================================================
;  Setea el refresh del display cuando se va por time-out el mensaje que esté
; ============================================================================
DisplayCtrl
  ; Decremento si es distinto de cero
  movf    DelayDisplay,f
  btfss   STATUS,Z
  decfsz  DelayDisplay,f
  return
  bsf     DISPLAY_DATA
  return
; ============================================================================
; Reproducción de códigos infra-rojos
; ============================================================================
IRCodeInt
  bcf     PIR1,TMR2IF   ; Borro el flag de int para que no se redispare
  ;
  btfss   SEND_IR
  return
  incf    IRCount,f
  ; Controlo el start - 8,5 ms
  movf    IRCount,w
  sublw   0x10
  btfsc   STATUS,C
  goto    IRCodeStart
  ; Apagado
  bcf     IR_OUT
  ; Pausa de 4 ms
  movf    IRCount,w
  sublw   0x17
  btfsc   STATUS,C
  return
  ; Se transmite el código en las vueltas pares
  btfsc   IRCount,0
  return
  ; Transmisión del código - 40 bits
  movf    IRCount,w
  sublw   0x67
  btfsc   STATUS,C ; me fijo si pasé los 40
  goto    IRCodeSend
IRCodeStop
  bcf     SEND_IR
  clrf    IRCount
  return
; -------------------------------------
IRCodeStart
  bsf     IR_OUT
  return
; -------------------------------------
IRCodeSend ; 24 .. 103
;   inicio
; (0x18) 24, 26, 28, 30, 32, 34,  36,  38
; (0x28) 40, 42, 44, 46, 48, 50,  52,  54
; (0x38) 56, 58, 60, 62, 64, 66,  68,  70
; (0x48) 72, 74, 76, 78, 80, 82,  84,  86
; (0x58) 88, 90, 92, 94, 96, 98, 100, 102
; Si corresponde traigo un byte de código
  movf    IRCount,w
  sublw   0x18
  btfsc   STATUS,Z
  goto    IRCodeLoad1 ; debe cargar el primer byte
  movf    IRCount,w
  sublw   0x28
  btfsc   STATUS,Z
  goto    IRCodeLoad2 ; debe cargar el segundo byte
  movf    IRCount,w
  sublw   0x38
  btfsc   STATUS,Z
  goto    IRCodeLoad3 ; debe cargar el tercer byte
  movf    IRCount,w
  sublw   0x48
  btfsc   STATUS,Z
  goto    IRCodeLoad4 ; debe cargar el cuarto byte
  movf    IRCount,w
  sublw   0x58
  btfsc   STATUS,Z
  goto    IRCodeLoad5 ; debe cargar el quinto byte
  ; no tiene que cargar nada, solamente transmitir
IRCodeSendCont
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  ; muevo el bit mas significativo al carry
  rlf     IRCodeByte,f
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  btfsc   STATUS,C
  bsf     IR_OUT ; Si era 1 levanto la salida
  return
; -------------------------------------
 ERRORLEVEL -302
IRCodeLoad1
  bsf     STATUS,RP0 ; --> BANK1
  movf    IRCode1,w
  movwf   IRCodeByte
  goto    IRCodeSendCont
; -------------------------------------
IRCodeLoad2
  bsf     STATUS,RP0 ; --> BANK1
  movf    IRCode2,w
  movwf   IRCodeByte
  goto    IRCodeSendCont
; -------------------------------------
IRCodeLoad3 
  bsf     STATUS,RP0 ; --> BANK1
  movf    IRCode3,w
  movwf   IRCodeByte
  goto    IRCodeSendCont
; -------------------------------------
IRCodeLoad4
  bsf     STATUS,RP0 ; --> BANK1
  movf    IRCode4,w
  movwf   IRCodeByte
  goto    IRCodeSendCont
; -------------------------------------
IRCodeLoad5
  bsf     STATUS,RP0 ; --> BANK1
  movf    IRCode5,w
  movwf   IRCodeByte
  goto    IRCodeSendCont
 ERRORLEVEL +302
; ============================================================================
; Interrupción externa (al bajar RB0)
; ============================================================================
ExternInt


  return
; ============================================================================
; ControlBeep - Controla el tiempo que suena el Buzzer y lo apaga
; ============================================================================
ControlBeep
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  movf    BeepCount,f
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  btfsc   STATUS,Z
  ; si esta en cero me fijo si tiene repeticion
  goto    ControlBeep2
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  decf    BeepCount,f
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  btfss   STATUS,Z
  return
  ; apago el buzzer
  bcf     BUZZER
  ; seteo el silencio entre dos pitidos
  movlw   BEEP_DELAY
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  movwf   BeepDelay
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  return
ControlBeep2
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  movf    BeepRetry,f
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  btfsc   STATUS,Z
  return              ; no hay repeticiones
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  movf    BeepDelay,f
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  btfsc   STATUS,Z
  return              ; no hay delay, no hay nada
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  decf    BeepDelay,f
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  btfss   STATUS,Z
  return              ; hay delay, tengo que esperar
  ; terminó el delay, ahora va otro beep
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  decf    BeepRetry,f ; le resto una repeticion
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  ; arranco otro pitido
  bsf     BUZZER
  movlw   SHORT_BEEP
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  movwf   BeepCount
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  return
; ============================================================================
; RxTimeOutCtrl
; ============================================================================
RxTimeOutCtrl
  movf    RxTimeOut,f
  btfss   STATUS,Z
  decfsz  RxTimeOut,f
  return
  ; en caso de error de recepción tiro todo lo recibido como mensaje
  movf    RxPtrAdd,w
  movwf   RxPtrAddTmp
  clrf    RxCharCount
  return
; ============================================================================
; CheckBtn - maneja los contadores de filtro de ruido de los botones y cambia
;            los estados BOTONES al llegar a los extremos
;           Trabaja en BANK1
; ============================================================================
 ERRORLEVEL -302
CheckBtn
  bsf     STATUS,RP0    ; --> BANK1
Btn0      ; boton 0
  btfss   EXT_BOTONES,0
  goto    Btn0Off
Btn0On
  movf    ContBtn0,w    ; me fijo si llegó al maximo
  sublw   BTNFILTER
  btfsc   STATUS,Z
  goto    Btn1          ; si, llegó
  incf    ContBtn0,f    ; no llegó todavía
  movf    ContBtn0,w    ; me fijo si llegó al maximo nuevamente
  sublw   BTNFILTER
  btfsc   STATUS,Z

  call    SetBtn0On     ; lo prendo
  goto    Btn1
Btn0Off
  movf    ContBtn0,f    ; me fijo si llegó a cero
  btfsc   STATUS,Z
  goto    Btn1          ; si, llegó
  decfsz  ContBtn0,f
  goto    Btn1          ; sigue sin llegar
  call    SetBtn0Off    ; lo apago
Btn1      ; boton 1
  btfss   EXT_BOTONES,1
  goto    Btn1Off
Btn1On
  movf    ContBtn1,w    ; me fijo si llegó al maximo
  sublw   BTNFILTER
  btfsc   STATUS,Z
  goto    Btn2          ; si, llegó
  incf    ContBtn1,f    ; no llegó todavía
  movf    ContBtn1,w    ; me fijo si llegó al maximo nuevamente
  sublw   BTNFILTER
  btfsc   STATUS,Z
  call    SetBtn1On     ; lo prendo
  goto    Btn2
Btn1Off
  movf    ContBtn1,f    ; me fijo si llegó a cero
  btfsc   STATUS,Z
  goto    Btn2          ; si, llegó
  decfsz  ContBtn1,f
  goto    Btn2          ; sigue sin llegar
  call    SetBtn1Off    ; lo apago
Btn2      ; boton 2
  btfss   EXT_BOTONES,2
  goto    Btn2Off
Btn2On
  movf    ContBtn2,w    ; me fijo si llegó al maximo
  sublw   BTNFILTER
  btfsc   STATUS,Z
  goto    Btn3          ; si, llegó
  incf    ContBtn2,f    ; no llegó todavía
  movf    ContBtn2,w    ; me fijo si llegó al maximo nuevamente
  sublw   BTNFILTER
  btfsc   STATUS,Z
  call    SetBtn2On     ; lo prendo
  goto    Btn3
Btn2Off
  movf    ContBtn2,f    ; me fijo si llegó a cero
  btfsc   STATUS,Z
  goto    Btn3          ; si, llegó
  decfsz  ContBtn2,f
  goto    Btn2          ; sigue sin llegar
  call    SetBtn2Off    ; lo apago
Btn3      ; boton 3
  btfss   EXT_BOTONES,3
  goto    Btn3Off
Btn3On
  movf    ContBtn3,w    ; me fijo si llegó al maximo
  sublw   BTNFILTER
  btfsc   STATUS,Z
  goto    Btn4          ; si, llegó
  incf    ContBtn3,f    ; no llegó todavía
  movf    ContBtn3,w    ; me fijo si llegó al maximo nuevamente
  sublw   BTNFILTER
  btfsc   STATUS,Z
  call    SetBtn3On     ; lo prendo
  goto    Btn4
Btn3Off
  movf    ContBtn3,f    ; me fijo si llegó a cero
  btfsc   STATUS,Z
  goto    Btn4          ; si, llegó
  decfsz  ContBtn3,f
  goto    Btn4          ; sigue sin llegar
  call    SetBtn3Off    ; lo apago
Btn4      ; boton 4
  btfss   EXT_BOTONES,4
  goto    Btn4Off
Btn4On
  movf    ContBtn4,w    ; me fijo si llegó al maximo
  sublw   BTNFILTER
  btfsc   STATUS,Z
  goto    Btn5          ; si, llegó
  incf    ContBtn4,f    ; no llegó todavía
  movf    ContBtn4,w    ; me fijo si llegó al maximo nuevamente
  sublw   BTNFILTER
  btfsc   STATUS,Z
  call    SetBtn4On     ; lo prendo
  goto    Btn5
Btn4Off
  movf    ContBtn4,f    ; me fijo si llegó a cero
  btfsc   STATUS,Z
  goto    Btn5          ; si, llegó
  decfsz  ContBtn4,f
  goto    Btn5          ; sigue sin llegar
  call    SetBtn4Off    ; lo apago
Btn5      ; boton 5
  btfss   EXT_BOTONES,5
  goto    Btn5Off
Btn5On
  movf    ContBtn5,w    ; me fijo si llegó al maximo
  sublw   BTNFILTER
  btfsc   STATUS,Z
  goto    Btn6          ; si, llegó
  incf    ContBtn5,f    ; no llegó todavía
  movf    ContBtn5,w    ; me fijo si llegó al maximo nuevamente
  sublw   BTNFILTER
  btfsc   STATUS,Z
  call    SetBtn5On     ; lo prendo
  goto    Btn6
Btn5Off
  movf    ContBtn5,f    ; me fijo si llegó a cero
  btfsc   STATUS,Z
  goto    Btn6          ; si, llegó
  decfsz  ContBtn5,f
  goto    Btn6          ; sigue sin llegar
  call    SetBtn5Off    ; lo apago
Btn6      ; boton 6
  btfss   EXT_BOTONES,6
  goto    Btn6Off
Btn6On
  movf    ContBtn6,w    ; me fijo si llegó al maximo
  sublw   BTNFILTER
  btfsc   STATUS,Z
  goto    Btn7          ; si, llegó
  incf    ContBtn6,f    ; no llegó todavía
  movf    ContBtn6,w    ; me fijo si llegó al maximo nuevamente
  sublw   BTNFILTER
  btfsc   STATUS,Z
  call    SetBtn6On     ; lo prendo
  goto    Btn7
Btn6Off
  movf    ContBtn6,f    ; me fijo si llegó a cero
  btfsc   STATUS,Z
  goto    Btn7          ; si, llegó
  decfsz  ContBtn6,f
  goto    Btn7          ; sigue sin llegar
  call    SetBtn6Off    ; lo apago
Btn7      ; boton 7
  btfss   EXT_BOTONES,7
  goto    Btn7Off
Btn7On
  movf    ContBtn7,w    ; me fijo si llegó al maximo
  sublw   BTNFILTER
  btfsc   STATUS,Z
  goto    BtnExit       ; si, llegó
  incf    ContBtn7,f    ; no llegó todavía
  movf    ContBtn7,w    ; me fijo si llegó al maximo nuevamente
  sublw   BTNFILTER
  btfsc   STATUS,Z
  call    SetBtn7On     ; lo prendo
  goto    BtnExit
Btn7Off
  movf    ContBtn7,f    ; me fijo si llegó a cero
  btfsc   STATUS,Z
  goto    BtnExit       ; si, llegó
  decfsz  ContBtn7,f
  goto    BtnExit       ; sigue sin llegar
  call    SetBtn7Off    ; lo apago
BtnExit
  bcf     STATUS,RP0    ; --> BANK0
  return
 ERRORLEVEL +302

; ============================================================================
; SetBtn#On/Off - eventos de botones
; ============================================================================
 ERRORLEVEL -302
SetBtn0On
  bsf     BOTONES,0

  return
SetBtn0Off
  bcf     BOTONES,0

  return
SetBtn1On
  bsf     BOTONES,1

  return
SetBtn1Off
  bcf     BOTONES,1

  return
SetBtn2On
  bsf     BOTONES,2

  return
SetBtn2Off
  bcf     BOTONES,2

  return
SetBtn3On
  bsf     BOTONES,3

  return
SetBtn3Off
  bcf     BOTONES,3

  return
SetBtn4On
  bsf     BOTONES,4

  return
SetBtn4Off
  bcf     BOTONES,4

  return
SetBtn5On
  bsf     BOTONES,5

  return
SetBtn5Off
  bcf     BOTONES,5

  return
SetBtn6On
  bsf     BOTONES,6

  return
SetBtn6Off
  bcf     BOTONES,6

  return
SetBtn7On
  bsf     BOTONES,7

  return
SetBtn7Off
  bcf     BOTONES,7

  return
 ERRORLEVEL +302
; ============================================================================
; CtrlStartAD
; ============================================================================
CtrlStartAD
  movf    ConvertDelay,f
  btfss   STATUS,Z
  decfsz  ConvertDelay,f
  return
  bsf     ADCON0,GO
  return
; ============================================================================
; StartCtrl - Controla el tiempo de arranque
; ============================================================================
StartCtrl
  decfsz    DelayStart,f
  return
  bsf       SYSREADY
  return
; ============================================================================
;  CtrlTrafic - Funcion de WFT
; ============================================================================
CtrlTrafic
  ; si el contador está en cero no hago nada
  movf    CntTrafic,f
  btfsc   STATUS,Z
  return
  ; si al decrementar llega a cero mando el ping
  decfsz  CntTrafic,f
  return
  ; Reintento cada 30 Seg
  movlw   0x1E
  movwf   DelayResend
  ; Inicializo el ensaje
  PAGESEL ClrTx
  call    ClrTx
  PAGESEL $
  ; >>>> mensaje de presencia >>>>
  movlw   'Q'        ; start char
  movwf   txtyp
  movlw   HSTADDR    ; dest addr
  movwf   txdst
  movf    LocAddr,w  ; local addr
  movwf   txsrc
  movf    Variant,w  ; contador
  movwf   txcnt
  movlw   'P'        ; comando - 'P' = Aviso de presencia o ping
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
  ; <<<< mensaje de presencia <<<<
  ; lo encola y termina
  PAGESEL TxEnqueue
  call    TxEnqueue
  PAGESEL $
  return
; ============================================================================
; CheckPulse - decrementa los timers de pulso y resetea el bit al llegar a 0
; ============================================================================
 ERRORLEVEL -302
CheckPulse
  bsf     STATUS,RP0    ; --> BANK1
pb0s1
  movf    ContPulseE1B0,f
  btfsc   STATUS,Z
  goto    pb1s1
  decfsz  ContPulseE1B0,f
  goto    pb1s1
  bcf     SALIDA1,0
  bsf     CAMBIO_S1
  ; grabo en EE
  bsf     SAVETOEE
pb1s1
  movf    ContPulseE1B1,f
  btfsc   STATUS,Z
  goto    pb2s1
  decfsz  ContPulseE1B1,f
  goto    pb2s1
  bcf     SALIDA1,1
  bsf     CAMBIO_S1
  ; grabo en EE
  bsf     SAVETOEE
pb2s1
  movf    ContPulseE1B2,f
  btfsc   STATUS,Z
  goto    pb3s1
  decfsz  ContPulseE1B2,f
  goto    pb3s1
  bcf     SALIDA1,2
  bsf     CAMBIO_S1
  ; grabo en EE
  bsf     SAVETOEE
pb3s1
  movf    ContPulseE1B3,f
  btfsc   STATUS,Z
  goto    pb4s1
  decfsz  ContPulseE1B3,f
  goto    pb4s1
  bcf     SALIDA1,3
  bsf     CAMBIO_S1
  ; grabo en EE
  bsf     SAVETOEE
pb4s1
  movf    ContPulseE1B4,f
  btfsc   STATUS,Z
  goto    pb5s1
  decfsz  ContPulseE1B4,f
  goto    pb5s1
  bcf     SALIDA1,4
  bsf     CAMBIO_S1
  ; grabo en EE
  bsf     SAVETOEE
pb5s1
  movf    ContPulseE1B5,f
  btfsc   STATUS,Z
  goto    pb6s1
  decfsz  ContPulseE1B5,f
  goto    pb6s1
  bcf     SALIDA1,5
  bsf     CAMBIO_S1
  ; grabo en EE
  bsf     SAVETOEE
pb6s1
  movf    ContPulseE1B6,f
  btfsc   STATUS,Z
  goto    pb7s1
  decfsz  ContPulseE1B6,f
  goto    pb7s1
  bcf     SALIDA1,6
  bsf     CAMBIO_S1
  ; grabo en EE
  bsf     SAVETOEE
pb7s1
  movf    ContPulseE1B7,f
  btfsc   STATUS,Z
  goto    pb0s2
  decfsz  ContPulseE1B7,f
  goto    pb0s2
  bcf     SALIDA1,7
  bsf     CAMBIO_S1
  ; grabo en EE
  bsf     SAVETOEE
pb0s2
  movf    ContPulseE2B0,f
  btfsc   STATUS,Z
  goto    pb1s2
  decfsz  ContPulseE2B0,f
  goto    pb1s2
  bcf     SALIDA2,0
  bsf     CAMBIO_S2
  ; grabo en EE
  bsf     SAVETOEE
pb1s2
  movf    ContPulseE2B1,f
  btfsc   STATUS,Z
  goto    pb2s2
  decfsz  ContPulseE2B1,f
  goto    pb2s2
  bcf     SALIDA2,1
  bsf     CAMBIO_S2
  ; grabo en EE
  bsf     SAVETOEE
pb2s2
  movf    ContPulseE2B2,f
  btfsc   STATUS,Z
  goto    pb3s2
  decfsz  ContPulseE2B2,f
  goto    pb3s2
  bcf     SALIDA2,2
  bsf     CAMBIO_S2
  ; grabo en EE
  bsf     SAVETOEE
pb3s2
  movf    ContPulseE2B3,f
  btfsc   STATUS,Z
  goto    pb4s2
  decfsz  ContPulseE2B3,f
  goto    pb4s2
  bcf     SALIDA2,3
  bsf     CAMBIO_S2
  ; grabo en EE
  bsf     SAVETOEE
pb4s2
  movf    ContPulseE2B4,f
  btfsc   STATUS,Z
  goto    pb5s2
  decfsz  ContPulseE2B4,f
  goto    pb5s2
  bcf     SALIDA2,4
  bsf     CAMBIO_S2
  ; grabo en EE
  bsf     SAVETOEE
pb5s2
  movf    ContPulseE2B5,f
  btfsc   STATUS,Z
  goto    pb6s2
  decfsz  ContPulseE2B5,f
  goto    pb6s2
  bcf     SALIDA2,5
  bsf     CAMBIO_S2
  ; grabo en EE
  bsf     SAVETOEE
pb6s2
  movf    ContPulseE2B6,f
  btfsc   STATUS,Z
  goto    pb7s2
  decfsz  ContPulseE2B6,f
  goto    pb7s2
  bcf     SALIDA2,6
  bsf     CAMBIO_S2
  ; grabo en EE
  bsf     SAVETOEE
pb7s2
  movf    ContPulseE2B7,f
  btfsc   STATUS,Z
  goto    CheckPulseExit
  decfsz  ContPulseE2B7,f
  goto    CheckPulseExit
  bcf     SALIDA2,7
  bsf     CAMBIO_S2
  ; grabo en EE
  bsf     SAVETOEE
CheckPulseExit
  bcf     STATUS,RP0    ; --> BANK0
  return
 ERRORLEVEL +302
; ============================================================================
; SaveEECtrl
; ============================================================================
SaveEECtrl
  ; si ya está en cero no hago nada
  movf    SaveEECount,f
  btfsc   STATUS,Z
  return
  decfsz  SaveEECount,f
  return
  ; hora de grabar
  bsf     SAVETOEE
  return
; ============================================================================
; ClockMant - mantiene el reloj local de la placa (OJO, no hay RTC)
; ============================================================================
ClockMant
  btfss   DATE_OK
  goto    GetDateTime
; Todas las variables estan en BANK1
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  ; SEGUNDOS
  incf    LocSeg,f
  movf    LocSeg,w
  ; si llega a 60 sigue el laburito
  sublw   0x3C
  btfss   STATUS,Z
  goto    ClockMant_End
  ; MINUTOS
  ; para que actualice el display cuando pueda
  bsf     DISPLAY_CLOCK
  clrf    LocSeg
  incf    LocMin,f
  movf    LocMin,w
  ; si llega a 60 sigue el laburito
  sublw   0x3C
  btfss   STATUS,Z
  goto    ClockMant_End
  ; HORAS
  clrf    LocMin
  incf    LocHora,f
  movf    LocHora,w
  ; si llega a 24 vuelvo a 0
  sublw   0x18
  btfsc   STATUS,Z
  clrf    LocHora
ClockMant_End
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  return
; ============================================================================
; GetDateTime - Genera un mensaje de petición de actualizacion de fecha
; ============================================================================
GetDateTime
  btfss     HSTONLINE
  return
  ; Inicializo el ensaje
  PAGESEL ClrTx
  call    ClrTx
  PAGESEL $
  ; >>>> mensaje get-datetime >>>>
  movlw   'Q'        ; start char
  movwf   txtyp
  movlw   HSTADDR    ; dest addr
  movwf   txdst
  movf    LocAddr,w  ; local addr
  movwf   txsrc
  movf    Variant,w  ; contador
  movwf   txcnt
  movlw   'T'        ; comando - 'T'
  movwf   txfcn
  ; <<<< mensaje get-datetime <<<<
  ; lo encola y termina
  PAGESEL TxEnqueue
  call    TxEnqueue
  PAGESEL $
  return
; ============================================================================
; ADCheck - El resto de las funciones fuera de zona de tablas
; ============================================================================
ADEnable
  clrf    ADCON0
  ; elijo clock RC
  bsf     ADCON0,ADCS0
  bsf     ADCON0,ADCS1
  ; marco desactualizadas las conversiones
  bcf     ADC_READY
  ; enciendo el conversor
  bsf     ADCON0,ADON
  ; selecciono la entrada AN0
  ;bcf     ADCON0,CHS0 ; -
  ;bcf     ADCON0,CHS1 ;  | No es nesario, recién borró ADCON0
  ;bcf     ADCON0,CHS2 ; -
  ; cargo el delay para iniciar la conversión
  movlw   CONV_DELAY
  movwf   ConvertDelay
  return
; ----------------------------------------------------------------------------
ADChan1
  movf    ADRESH,w
  movwf   ANALOG1H
 ERRORLEVEL -302
  bsf     STATUS,RP0  ; --> BANK1
  movf    ADRESL,w
  bcf     STATUS,RP0  ; --> BANK0
 ERRORLEVEL +302
  movwf   ANALOG1L
  ; selecciono la entrada AN1
  bsf     ADCON0,CHS0
  bcf     ADCON0,CHS1
  bcf     ADCON0,CHS2
  ; cargo el delay para iniciar la conversión
  movlw   CONV_DELAY
  movwf   ConvertDelay
  goto    ADControl
; ----------------------------------------------------------------------------
ADChan2
  movf    ADRESH,w
  movwf   ANALOG2H
 ERRORLEVEL -302
  bsf     STATUS,RP0  ; --> BANK1
  movf    ADRESL,w
  bcf     STATUS,RP0  ; --> BANK0
 ERRORLEVEL +302
  movwf   ANALOG2L
  ; selecciono la entrada AN2
  bcf     ADCON0,CHS0
  bsf     ADCON0,CHS1
  bcf     ADCON0,CHS2
  ; cargo el delay para iniciar la conversión
  movlw   CONV_DELAY
  movwf   ConvertDelay
  goto    ADControl
; ----------------------------------------------------------------------------
ADChan3
  movf    ADRESH,w
  movwf   ANALOG3H
 ERRORLEVEL -302
  bsf     STATUS,RP0  ; --> BANK1
  movf    ADRESL,w
  bcf     STATUS,RP0  ; --> BANK0
 ERRORLEVEL +302
  movwf   ANALOG3L
  ; selecciono la entrada AN3
  bsf     ADCON0,CHS0
  bsf     ADCON0,CHS1
  bcf     ADCON0,CHS2
  ; cargo el delay para iniciar la conversión
  movlw   CONV_DELAY
  movwf   ConvertDelay
  goto    ADControl
; ----------------------------------------------------------------------------
ADChan4
  movf    ADRESH,w
  movwf   ANALOG4H
 ERRORLEVEL -302
  bsf     STATUS,RP0  ; --> BANK1
  movf    ADRESL,w
  bcf     STATUS,RP0  ; --> BANK0
 ERRORLEVEL +302
  movwf   ANALOG4L
  ; selecciono la entrada AN0
  bcf     ADCON0,CHS0
  bcf     ADCON0,CHS1
  bcf     ADCON0,CHS2
  ; cargo el delay para iniciar la conversión
  movlw   CONV_DELAY
  movwf   ConvertDelay
  ; marco actualizadas las conversiones
  bsf     ADC_READY
  goto    ADControl
; ----------------------------------------------------------------------------
 ERRORLEVEL -302
ADControl
  ; me fijo si todas las entradas fueron escaneadas
  btfss   ADC_READY
  return  ; no!
; ----------------------------------------------------------------------------
ADCtrl1
  ; si el byte mas significativo del maximo está en FFs el control está desactivado
  bsf     STATUS,RP0  ; --> BANK1
  movf    ANALOG1_MAXH,w
  andlw   0x04
  btfss   STATUS,Z
  goto    ADCtrl2
  ; me fijo en qué estado estaba
  btfss   AN1_UMBRAL
  goto    ADCtrl1_off
ADCtrl1_on
  ; estaba ON, me fijo si ahora tiene que estar OFF
  ;   comparo primero el byte mas significativo
  movf    ANALOG1_MINH,w
  bcf     STATUS,RP0  ; --> BANK0
  subwf   ANALOG1H,w
  btfsc   STATUS,Z
  goto    ADCtrl1_on_l      ; ANALOG?H = ANALOG?_MINH
  btfss   STATUS,C
  goto    ADCtrl1_on2off    ; ANALOG?H < ANALOG?_MINH
  ; ANALOG?H > ANALOG?_MINH --> paso al siguiente
  goto    ADCtrl2
ADCtrl1_on_l
  ; ahora comparo la parte baja del conversor
  bsf     STATUS,RP0  ; --> BANK1
  movf    ANALOG1_MINL,w
  bcf     STATUS,RP0  ; --> BANK0
  subwf   ANALOG1L,w
  btfsc   STATUS,Z
  goto    ADCtrl1_on2off    ; ANALOG?H = ANALOG1_MINL
  btfss   STATUS,C
  goto    ADCtrl1_on2off    ; ANALOG?H < ANALOG1_MINL
  goto    ADCtrl2
ADCtrl1_off
  ; estaba OFF, me fijo si ahora tiene que estar ON
  ;   comparo primero el byte mas significativo
  bsf     STATUS,RP0  ; --> BANK1
  movf    ANALOG1_MAXH,w
  bcf     STATUS,RP0  ; --> BANK0
  subwf   ANALOG1H,w
  btfsc   STATUS,Z
  goto    ADCtrl1_off_l     ; ANALOG?H = ANALOG?_MAXH
  btfsc   STATUS,C
  goto    ADCtrl1_off2on    ; ANALOG?H > ANALOG?_MAXH
  ; ANALOG?H < ANALOG?_MAXH --> paso al siguiente
  goto    ADCtrl2
ADCtrl1_off_l
  ; ahora comparo el byte menos significatico
  bsf     STATUS,RP0  ; --> BANK1
  movf    ANALOG1_MAXL,w
  bcf     STATUS,RP0  ; --> BANK0
  subwf   ANALOG1L,w
  btfsc   STATUS,Z
  goto    ADCtrl1_off2on    ; ANALOG?L = ANALOG?_MAXL
  btfsc   STATUS,C
  goto    ADCtrl1_off2on    ; ANALOG?L > ANALOG?_MAXL
  goto    ADCtrl2
ADCtrl1_on2off
  ; implemento un filtro de ruido por delay
  bsf     STATUS,RP0  ; --> BANK1
  decfsz  ADALRM1DELAY,f
  goto    ADCtrl2   ; -> todavía no es momento
  bcf     AN1_UMBRAL
  bsf     CAMBIO_AN
  btfss   MaskNotifAN,0
  goto    ADCtrl2
  movlw   REINTENTOS
  bcf     STATUS,RP0  ; --> BANK0
  movwf   CountNotifA1
  goto    ADCtrl2
ADCtrl1_off2on
  ; implemento un filtro de ruido por delay
  bsf     STATUS,RP0  ; --> BANK1
  incfsz  ADALRM1DELAY,f
  goto    ADCtrl2   ; -> todavía no es momento
  bsf     AN1_UMBRAL
  bsf     CAMBIO_AN
  btfss   MaskNotifAN,0
  goto    ADCtrl2
  movlw   REINTENTOS
  bcf     STATUS,RP0  ; --> BANK0
  movwf   CountNotifA1
; ----------------------------------------------------------------------------
ADCtrl2
  ; si el byte mas significativo del maximo está en FFs el control está desactivado
  bsf     STATUS,RP0  ; --> BANK1
  movf    ANALOG2_MAXH,w
  andlw   0x04
  btfss   STATUS,Z
  goto    ADCtrl3
  ; me fijo en qué estado estaba
  btfss   AN2_UMBRAL
  goto    ADCtrl2_off
ADCtrl2_on
  ; estaba ON, me fijo si ahora tiene que estar OFF
  ;   comparo primero el byte mas significativo
  movf    ANALOG2_MINH,w
  bcf     STATUS,RP0  ; --> BANK0
  subwf   ANALOG2H,w
  btfsc   STATUS,Z
  goto    ADCtrl2_on_1      ; ANALOG?H = ANALOG?_MINH
  btfss   STATUS,C
  goto    ADCtrl2_on2off    ; ANALOG?H < ANALOG?_MINH
  ; ANALOG?H > ANALOG?_MINH --> paso al siguiente
  goto    ADCtrl3
ADCtrl2_on_1
  ; ahora comparo la parte baja del conversor
  bsf     STATUS,RP0  ; --> BANK1
  movf    ANALOG2_MINL,w
  bcf     STATUS,RP0  ; --> BANK0
  subwf   ANALOG2L,w
  btfsc   STATUS,Z
  goto    ADCtrl2_on2off    ; ANALOG?H = ANALOG1_MINL
  btfss   STATUS,C
  goto    ADCtrl2_on2off    ; ANALOG?H < ANALOG1_MINL
  goto    ADCtrl3
ADCtrl2_off
  ; estaba OFF, me fijo si ahora tiene que estar ON
  ;   comparo primero el byte mas significativo
  bsf     STATUS,RP0  ; --> BANK1
  movf    ANALOG2_MAXH,w
  bcf     STATUS,RP0  ; --> BANK0
  subwf   ANALOG2H,w
  btfsc   STATUS,Z
  goto    ADCtrl2_off_l     ; ANALOG?H = ANALOG?_MAXH
  btfsc   STATUS,C
  goto    ADCtrl2_off2on    ; ANALOG?H > ANALOG?_MAXH
  ; ANALOG?H < ANALOG?_MAXH --> paso al siguiente
  goto    ADCtrl3
ADCtrl2_off_l
  ; ahora comparo el byte menos significatico
  bsf     STATUS,RP0  ; --> BANK1
  movf    ANALOG2_MAXL,w
  bcf     STATUS,RP0  ; --> BANK0
  subwf   ANALOG2L,w
  btfsc   STATUS,Z
  goto    ADCtrl2_off2on    ; ANALOG?L = ANALOG?_MAXL
  btfsc   STATUS,C
  goto    ADCtrl2_off2on    ; ANALOG?L > ANALOG?_MAXL
  goto    ADCtrl3
ADCtrl2_on2off
  ; implemento un filtro de ruido por delay
  bsf     STATUS,RP0  ; --> BANK1
  decfsz  ADALRM2DELAY,f
  goto    ADCtrl3   ; -> todavía no es momento
  bcf     AN2_UMBRAL
  bsf     CAMBIO_AN
  btfss   MaskNotifAN,1
  goto    ADCtrl3
  movlw   REINTENTOS
  bcf     STATUS,RP0  ; --> BANK0
  movwf   CountNotifA2
  goto    ADCtrl3
ADCtrl2_off2on
  ; implemento un filtro de ruido por delay
  bsf     STATUS,RP0  ; --> BANK1
  incfsz  ADALRM2DELAY,f
  goto    ADCtrl3   ; -> todavía no es momento
  bsf     AN2_UMBRAL
  bsf     CAMBIO_AN
  btfss   MaskNotifAN,1
  goto    ADCtrl3
  movlw   REINTENTOS
  bcf     STATUS,RP0  ; --> BANK0
  movwf   CountNotifA2
; ----------------------------------------------------------------------------
ADCtrl3
  ; si el byte mas significativo del maximo está en FFs el control está desactivado
  bsf     STATUS,RP0  ; --> BANK1
  movf    ANALOG3_MAXH,w
  andlw   0x04
  btfss   STATUS,Z
  goto    ADCtrl4
  ; me fijo en qué estado estaba
  btfss   AN3_UMBRAL
  goto    ADCtrl3_off
ADCtrl3_on
  ; estaba ON, me fijo si ahora tiene que estar OFF
  ;   comparo primero el byte mas significativo
  movf    ANALOG3_MINH,w
  bcf     STATUS,RP0  ; --> BANK0
  subwf   ANALOG3H,w
  btfsc   STATUS,Z
  goto    ADCtrl3_on_l      ; ANALOG?H = ANALOG?_MINH
  btfss   STATUS,C
  goto    ADCtrl3_on2off    ; ANALOG?H < ANALOG?_MINH
  ; ANALOG?H > ANALOG?_MINH --> paso al siguiente
  goto    ADCtrl4
ADCtrl3_on_l
  ; ahora comparo la parte baja del conversor
  bsf     STATUS,RP0  ; --> BANK1
  movf    ANALOG3_MINL,w
  bcf     STATUS,RP0  ; --> BANK0
  subwf   ANALOG3L,w
  btfsc   STATUS,Z
  goto    ADCtrl3_on2off    ; ANALOG?H = ANALOG1_MINL
  btfss   STATUS,C
  goto    ADCtrl3_on2off    ; ANALOG?H < ANALOG1_MINL
  goto    ADCtrl4
ADCtrl3_off
  ; estaba OFF, me fijo si ahora tiene que estar ON
  ;   comparo primero el byte mas significativo
  bsf     STATUS,RP0  ; --> BANK1
  movf    ANALOG3_MAXH,w
  bcf     STATUS,RP0  ; --> BANK0
  subwf   ANALOG3H,w
  btfsc   STATUS,Z
  goto    ADCtrl3_off_l     ; ANALOG?H = ANALOG?_MAXH
  btfsc   STATUS,C
  goto    ADCtrl3_off2on    ; ANALOG?H > ANALOG?_MAXH
  ; ANALOG?H < ANALOG?_MAXH --> paso al siguiente
  goto    ADCtrl4
ADCtrl3_off_l
  ; ahora comparo el byte menos significatico
  bsf     STATUS,RP0  ; --> BANK1
  movf    ANALOG3_MAXL,w
  bcf     STATUS,RP0  ; --> BANK0
  subwf   ANALOG3L,w
  btfsc   STATUS,Z
  goto    ADCtrl3_off2on    ; ANALOG?L = ANALOG?_MAXL
  btfsc   STATUS,C
  goto    ADCtrl3_off2on    ; ANALOG?L > ANALOG?_MAXL
  goto    ADCtrl4
ADCtrl3_on2off
  ; implemento un filtro de ruido por delay
  bsf     STATUS,RP0  ; --> BANK1
  decfsz  ADALRM3DELAY,f
  goto    ADCtrl4   ; -> todavía no es momento
  bcf     AN3_UMBRAL
  bsf     CAMBIO_AN
  btfss   MaskNotifAN,2
  goto    ADCtrl4
  movlw   REINTENTOS
  bcf     STATUS,RP0  ; --> BANK0
  movwf   CountNotifA3
  goto    ADCtrl4
ADCtrl3_off2on
  ; implemento un filtro de ruido por delay
  bsf     STATUS,RP0  ; --> BANK1
  incfsz  ADALRM3DELAY,f
  goto    ADCtrl4   ; -> todavía no es momento
  bsf     AN3_UMBRAL
  bsf     CAMBIO_AN
  btfss   MaskNotifAN,2
  goto    ADCtrl4
  movlw   REINTENTOS
  bcf     STATUS,RP0  ; --> BANK0
  movwf   CountNotifA3
; ----------------------------------------------------------------------------
ADCtrl4
  ; si el byte mas significativo del maximo está en FFs el control está desactivado
  bsf     STATUS,RP0  ; --> BANK1
  movf    ANALOG4_MAXH,w
  andlw   0x04
  btfss   STATUS,Z
  goto    ADCtrlFin
  ; me fijo en qué estado estaba
  btfss   AN4_UMBRAL
  goto    ADCtrl4_off
ADCtrl4_on
  ; estaba ON, me fijo si ahora tiene que estar OFF
  ;   comparo primero el byte mas significativo
  movf    ANALOG4_MINH,w
  bcf     STATUS,RP0  ; --> BANK0
  subwf   ANALOG4H,w
  btfsc   STATUS,Z
  goto    ADCtrl4_on_l      ; ANALOG?H = ANALOG?_MINH
  btfss   STATUS,C
  goto    ADCtrl4_on2off    ; ANALOG?H < ANALOG?_MINH
  ; ANALOG?H > ANALOG?_MINH --> paso al siguiente
  goto    ADCtrlFin
ADCtrl4_on_l
  ; ahora comparo la parte baja del conversor
  bsf     STATUS,RP0  ; --> BANK1
  movf    ANALOG4_MINL,w
  bcf     STATUS,RP0  ; --> BANK0
  subwf   ANALOG4L,w
  btfsc   STATUS,Z
  goto    ADCtrl4_on2off    ; ANALOG?H = ANALOG1_MINL
  btfss   STATUS,C
  goto    ADCtrl4_on2off    ; ANALOG?H < ANALOG1_MINL
  goto    ADCtrlFin
ADCtrl4_off
  ; estaba OFF, me fijo si ahora tiene que estar ON
  ;   comparo primero el byte mas significativo
  bsf     STATUS,RP0  ; --> BANK1
  movf    ANALOG4_MAXH,w
  bcf     STATUS,RP0  ; --> BANK0
  subwf   ANALOG4H,w
  btfsc   STATUS,Z
  goto    ADCtrl4_off_l     ; ANALOG?H = ANALOG?_MAXH
  btfsc   STATUS,C
  goto    ADCtrl4_off2on    ; ANALOG?H > ANALOG?_MAXH
  ; ANALOG?H < ANALOG?_MAXH --> me voy
  goto    ADCtrlFin
ADCtrl4_off_l
  ; ahora comparo el byte menos significatico
  bsf     STATUS,RP0  ; --> BANK1
  movf    ANALOG4_MAXL,w
  bcf     STATUS,RP0  ; --> BANK0
  subwf   ANALOG4L,w
  btfsc   STATUS,Z
  goto    ADCtrl4_off2on    ; ANALOG?L = ANALOG?_MAXL
  btfsc   STATUS,C
  goto    ADCtrl4_off2on    ; ANALOG?L > ANALOG?_MAXL
  goto    ADCtrlFin
ADCtrl4_on2off
  ; implemento un filtro de ruido por delay
  bsf     STATUS,RP0  ; --> BANK1
  decfsz  ADALRM4DELAY,f
  goto    ADCtrlFin   ; -> todavía no es momento
  bcf     AN4_UMBRAL
  bsf     CAMBIO_AN
  btfss   MaskNotifAN,3
  goto    ADCtrlFin
  movlw   REINTENTOS
  bcf     STATUS,RP0  ; --> BANK0
  movwf   CountNotifA4
  goto    ADCtrlFin
ADCtrl4_off2on
  ; implemento un filtro de ruido por delay
  bsf     STATUS,RP0  ; --> BANK1
  incfsz  ADALRM4DELAY,f
  goto    ADCtrlFin   ; -> todavía no es momento
  bsf     AN4_UMBRAL
  bsf     CAMBIO_AN
  btfss   MaskNotifAN,3
  goto    ADCtrlFin
  movlw   REINTENTOS
  bcf     STATUS,RP0  ; --> BANK0
  movwf   CountNotifA4
ADCtrlFin
  bcf     STATUS,RP0  ; --> BANK0
  return
 ERRORLEVEL +302
; ============================================================================
; ResetCtrl - Controla si hay que resetear el dispositivo
; ============================================================================
ResetCtrl
  ; si ya está en cero no hago nada
  movf    ResetCount,f
  btfsc   STATUS,Z
  return
  ; sino decremento, ero no hago nada hasta que llegue solo a cero
  decfsz  ResetCount,f
  return
ResetLoop
  nop
  goto ResetLoop
; ============================================================================
; Delay_x5us - produce tantos delay de 5 microsegundos como indique W
;            - se usa para el multiple sondeo de las entradas digitales
; ============================================================================
Delay_x5us
  movwf   RegTmpD
Delay_x5us_Loop
  nop
  nop
  decfsz  RegTmpD,f
  goto    Delay_x5us_Loop
  return
; ============================================================================
; Delay_1ms - produce un delay de un milisegundo
; ============================================================================
Delay_1ms
  movlw   0xFA      ; 250
  movwf   RegTmpD
Delay_1ms_Loop
  nop
  nop
  decfsz  RegTmpD,f
  goto    Delay_1ms_Loop
  return
; ============================================================================
; Delay_x1ms - produce un delay de tantos milisegundos como indique W
; ============================================================================
Delay_x1ms
  movwf   RegTmpL
Delay_x1ms_Loop
  call    Delay_1ms
  decfsz  RegTmpL,f
  goto    Delay_x1ms_Loop
  return
; ============================================================================
;  CLOCK_Refresh
;
; _Vie_01_May___15:23_
; ============================================================================
CLOCK_Refresh
  ; controlo que el display este presente
  btfss   DISPLAY_OK
  return
  ; me fijo si ya hice la inicializacion
  btfss   DISPLAY_INIT
  return
  ; me fijo si hay que refrescar el reloj
  btfss   DISPLAY_CLOCK
  return
  ;
  call    LCD_LINE1
  movlw   ' '
  call    LCD_DATOS
  ; El dia de la semama
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movf    LocDia,w
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  call    PrintDiaSemana
  ; un espacio
  movlw   ' '
  call    LCD_DATOS
  ; La fecha (dos digitos)
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movf    LocFecha,w
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  ; separo en digitos lo que hay en el byte
  call    HEX2DEC
  ; paso las decenas a string
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movf    DEC_M,w
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  call    DEC2ASCII
  ; lo mando al display
  call    LCD_DATOS
  ; paso las unidades a string
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movf    DEC_L,w
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  call    DEC2ASCII
  ; lo mando al display
  call    LCD_DATOS
  ; un espacio
  movlw   ' '
  call    LCD_DATOS
  ; El mes
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movf    LocMes,w
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  call    PrintMes
  ; tres espacion
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  ; la hora
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movf    LocHora,w
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  ; separo en digitos lo que hay en el byte
  call    HEX2DEC
  ; paso las decenas a string
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movf    DEC_M,w
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  call    DEC2ASCII
  ; lo mando al display
  call    LCD_DATOS
  ; paso las unidades a string
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movf    DEC_L,w
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  call    DEC2ASCII
  ; lo mando al display
  call    LCD_DATOS
  ; dos puntitos
  movlw   ':'
  call    LCD_DATOS
  ; los minutos
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movf    LocMin,w
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  ; separo en digitos lo que hay en el byte
  call    HEX2DEC
  ; paso las decenas a string
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movf    DEC_M,w
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  call    DEC2ASCII
  ; lo mando al display
  call    LCD_DATOS
  ; paso las unidades a string
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movf    DEC_L,w
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  call    DEC2ASCII
  ; lo mando al display
  call    LCD_DATOS
  return
; ============================================================================
; DISPLAY     DISPLAY_EN
;             DISPLAY_RS
;             DISPLAY_RW
;
; ============================================================================
DISPLAY_Refresh
  ; controlo que el display este presente
  btfss   DISPLAY_OK
  return
  ; miro si hay algo para hacer
  btfss   DISPLAY_DATA
  return
  ; me fijo si ya hice la inicializacion
  btfss   DISPLAY_INIT
  goto    LCD_INI
  ; me fijo si tengo que esperar
  movf    DelayDisplay,f
  btfss   STATUS,Z
  return
  ; despues sigo con el resto
  bcf     DISPLAY_DATA
  movf    ScrnId,w
  goto    SelectScreen
; ############################################################################
; Funciones LCD
; ############################################################################
LCD_E
  bsf     DISPLAY_EN      ;Activa señal E
  nop
  nop
  nop                     ;Espera 1uS
  bcf     DISPLAY_EN      ;Desactiva señal E
  return
; -----------------------------------------------------------------------------
LCD_BUSY
  bsf     DISPLAY_RW      ;Pone el LCD en modo lectura
 ERRORLEVEL -302
  bsf     STATUS,RP0  ; page1
  movlw   0xFF            ;Seteo el port como entrada
  movwf   TRISBUS
  bcf     STATUS,RP0  ; page0
 ERRORLEVEL +302
  bsf     DISPLAY_EN      ;Activa el LCD (Señal E)
  movlw   0xFF            ; cargo un contador para llevar un time-out para el display
  movwf   DisplayTO
L_BUSY
  clrwdt
  ; Control de presencia del display >-->
  movf    DisplayTO,f       ; este contador se decrementa en el timer
  btfsc   STATUS,Z          ; si se va por time-out es porque el display no esta
  goto    LCD_NO_PRESENT
  ; Control de presencia del display <--<
  btfsc   DATABUS,7       ;Chequea el bit BUSY
  goto    L_BUSY          ;Est  a "1" (Ocupado)
  bcf     DISPLAY_EN      ;Desactiva el LCD (Señal E)
 ERRORLEVEL -302
  bsf     STATUS,RP0  ; page1
  movlw   0x00            ;Seteo el port como salida
  movwf   TRISBUS
  bcf     STATUS,RP0  ; page0
 ERRORLEVEL +302
  bcf     DISPLAY_RW      ;Pone el LCD en modo escritura
  return
LCD_NO_PRESENT
  bcf     DISPLAY_OK
  return
; -----------------------------------------------------------------------------
; LCD_REG: Envía una instrucción al display
LCD_REG
  bcf     DISPLAY_RS      ;Desactiva RS (Modo instruccion)
  movwf   DATABUS         ;Saca el codigo de instruccion
  call    LCD_BUSY        ;Espera a que se libere el LCD
  goto    LCD_E           ;Genera pulso en señal E
; -----------------------------------------------------------------------------
; LCD_DATOS: Envía un dato al display
LCD_DATOS
  bcf     DISPLAY_RS      ;Desactiva RS (Modo instrucci¢n)
  movwf   DATABUS         ;Valor ASCII a sacar por PORTB
  call    LCD_BUSY        ;Espera a que se libere el LCD
  bsf     DISPLAY_RS      ;Activa RS (Modo dato)
  goto    LCD_E           ;Genera pulso en señal E
; -----------------------------------------------------------------------------
LCD_LINE1
  movlw   b'10000000'
  goto    LCD_REG
LCD_LINE1R
  movlw   b'10001010'
  goto    LCD_REG
LCD_LINE2
  movlw   b'11000000'
  goto    LCD_REG
LCD_LINE2R
  movlw   b'11001010'
  goto    LCD_REG
LCD_LINE3
  movlw   b'10010100'
  goto    LCD_REG
LCD_LINE3R
  movlw   b'10011110'
  goto    LCD_REG
LCD_LINE4
  movlw   b'11010100'
  goto    LCD_REG
LCD_LINE4R
  movlw   b'11011110'
  goto    LCD_REG
; -----------------------------------------------------------------------------
LCD_INI
  bcf     DISPLAY_DATA
  ;
  movlw   b'00111000'
  call    LCD_REG         ;Codigo de instruccion
  PAGESEL Delay_x1ms
  movlw   0x05            ; delay 5 ms
  call    Delay_x1ms      ;
  PAGESEL $
  movlw   b'00111000'
  call    LCD_REG         ;Codigo de instruccion
  PAGESEL Delay_x1ms
  movlw   0x05            ; delay 5 ms
  call    Delay_x1ms      ;
  PAGESEL $
  movlw   b'00111000'
  call    LCD_REG         ;Codigo de instruccion
  PAGESEL Delay_x1ms
  movlw   0x05            ; delay 5 ms
  call    Delay_x1ms      ;
  PAGESEL $
  ; ahora ya funciona el flag BUSY del display
  movlw   b'00000001'     ;Borrar LCD y Home
  call    LCD_REG
  ; aca ya puedo controlar si el display esta
  btfss   DISPLAY_OK
  return
  ;
  movlw   b'00000110'     ;
  call    LCD_REG
  movlw   b'00001100'     ;LCD On, cursor Off,Parpadeo Off
  call    LCD_REG
  movlw   0x80            ;Direccion caracter
  call    LCD_REG
  ; lo marco ya inicializado
  bsf     DISPLAY_INIT
  ;
  ; Leyenda de inicializacion
  call    LCD_LINE1
  movlw   ' '
  call    LCD_DATOS
  movlw   'G'
  call    LCD_DATOS
  movlw   'n'
  call    LCD_DATOS
  movlw   'u'
  call    LCD_DATOS
  movlw   '-'
  call    LCD_DATOS
  movlw   'M'
  call    LCD_DATOS
  movlw   'H'
  call    LCD_DATOS
  movlw   'o'
  call    LCD_DATOS
  movlw   'm'
  call    LCD_DATOS
  movlw   'e'
  call    LCD_DATOS
  ;
  call    LCD_LINE2
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   'M'
  call    LCD_DATOS
  movlw   'I'
  call    LCD_DATOS
  movlw   'O'
  call    LCD_DATOS
  movlw   '-'
  call    LCD_DATOS
  movlw   '1'
  call    LCD_DATOS
  movlw   '6'
  call    LCD_DATOS
  movlw   'x'
  call    LCD_DATOS
  movlw   '1'
  call    LCD_DATOS
  movlw   '6'
  call    LCD_DATOS
  movlw   '/'
  call    LCD_DATOS
  movlw   'A'
  call    LCD_DATOS
  movlw   'D'
  call    LCD_DATOS
  return
; ----------------------------------------------------------------------------
; RxBuffer2LCD - Mueve datos desde el buffer recibido hacia el display
; ----------------------------------------------------------------------------
RxBuffer2LCD
  ; inhibo el display por 2 minutos para que no se borre lo que puse
  movlw   0x78
  movwf   DelayDisplay
  ; muevo los caracteres al display
  movf    rxdat1,w
  call    LCD_DATOS
  movf    rxdat2,w
  call    LCD_DATOS
  movf    rxdat3,w
  call    LCD_DATOS
  movf    rxdat4,w
  call    LCD_DATOS
  movf    rxdat5,w
  call    LCD_DATOS
  movf    rxdat6,w
  call    LCD_DATOS
  movf    rxdat7,w
  call    LCD_DATOS
  movf    rxdat8,w
  call    LCD_DATOS
  movf    rxdat9,w
  call    LCD_DATOS
  movf    rxdat10,w
  call    LCD_DATOS
  return
; ============================================================================
; HEX2DEC - Convierte el valor numerico en W a tres valores decimales
; ============================================================================
HEX2DEC
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  movwf   HEXADECIMAL
  clrf    DEC_H
  clrf    DEC_M
  clrf    DEC_L
HEX2DEC_H
  movlw   D'100'          ;D'100' --> W
  subwf   HEXADECIMAL,w   ;HEXADECIMAL - D'100' --> W
  btfss   STATUS,C        ;HEXADECIMAL es menor que D'100' ?
  goto    HEX2DEC_M       ;Si
  movwf   HEXADECIMAL     ;Salva el resto
  incf    DEC_H,f       ;Incrementa el contador de centenas BCD
  goto    HEX2DEC_H       ;Vuelve por otros D'100'
;
HEX2DEC_M
  movlw   D'10'           ;D'10' --> W
  subwf   HEXADECIMAL,w   ;HEXADECIMAL - D'10' --> W
  btfss   STATUS,C        ;HEXADECIMAL es menor que D'10' ?
  goto    HEX2DEC_L       ;Si
  movwf   HEXADECIMAL     ;Salva el resto
  incf    DEC_M,F       ;Incrementa el contador de decenas BCD
  goto    HEX2DEC_M       ;Vuelve por otros D'10'
HEX2DEC_L
  movf    HEXADECIMAL,W   ;El resto pertenece
  movwf   DEC_L         ;a las unidades BCD
  clrf    HEXADECIMAL     ;Pone a cero el registro HEXADECIMAL
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  return
; ============================================================================
; MANEJO DE I/O
; -------------
;
; ENTRADA1    NIN1
; ENTRADA2    NIN2
; SALIDA1     OUT1
; SALIDA2     OUT2
;
; ============================================================================
IO_Refresh
  ; primero me aseguro que el multiplexado está inhabilitado
  bsf     NIN1
  bsf     NIN2
  bcf     OUT1
  bcf     OUT2
  ; seteo el BUS como salida
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movlw   0x00   ;tris out
  movwf   TRISBUS
  bcf     STATUS,RP0    ; --> BANK2
 ERRORLEVEL +302
  ; verifico el flag de cambio de ambas salidas
  btfsc   CAMBIO_S1
  call    WriteSal1
  btfsc   CAMBIO_S2
  call    WriteSal2
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  movlw   0xFF   ;tris in
  movwf   TRISBUS
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
; - Entradas -----------------------------------------------------------------
; verifico la entrada 1
ReadEnt1
  movlw   SENS_INPUT          ; veces que se lee cada entrada
  movwf   ReadIRetry
  bcf     NIN1                ; habilito la interface de in1
  ; un pequeño retardo de 15us
  movlw   0x03
  call    Delay_x5us
  movf    DATABUS,w           ; bus->w
  movwf   RegTmpG
; ---------------------------
ReadEnt1Loop
  clrwdt
  ; un pequeño retardo de 3ms
  movlw   0x03
  call    Delay_x1ms
  movf    DATABUS,w           ; bus->w
  subwf   RegTmpG,w
  btfss   STATUS,Z
  goto    ReadEnt1Bad          ; si las lecturas dan diferente vuelvo a empesar
  decfsz  ReadIRetry,f        ; voy decrementando la lectura si leí lo mismo
  goto    ReadEnt1Loop
; con tres lecturas identicas lo doy por bueno
ReadEnt1Ok
  ; inhabilito la interface de in1
  bsf     NIN1
  ; comparo lo del port con lo que tenia de antes
  movf    RegTmpG,w
  xorwf   ENTRADA1,w     ; dejo en w 1s donde hay diferencia
  btfsc   STATUS,Z
  goto    ReadEnt2       ; si no hay diferencia leo la otra entrada
  ; si hay diferencia me fijo si pasa la mascara
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  andwf   MaskNotifE1,w
  btfsc   STATUS,Z
  goto    ReadEnt1OkFin       ; los cambios no estaban en la mascara de noticicacion
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  ; preparo la notificación
  movlw   REINTENTOS
  movwf   CountNotifE1
ReadEnt1OkFin
  bcf     STATUS,RP0    ; --> BANK0
  movf    RegTmpG,w
  movwf   ENTRADA1
  bsf     CAMBIO_E1   ; levanto el flag que indica que cambió la entrada
  goto    ReadEnt2
; ---------------------------
ReadEnt1Bad
  ; paso los datos del bus al registro temporal
  movf    DATABUS,w           ; bus->w
  movwf   RegTmpG              ; w->f
  ; veces que se lee cada entrada
  movlw   SENS_INPUT
  movwf   ReadIRetry
  goto    ReadEnt1Loop
; ---------------------------
; verifico la entrada 2
ReadEnt2
  movlw   SENS_INPUT          ; veces que se lee cada entrada
  movwf   ReadIRetry
  bsf     NIN1       ; inhabilito la interface de in1
  bcf     NIN2       ; habilito la interface de in2
  ; un pequeno retardo de 15us
  movlw   0x03
  call    Delay_x5us
  movf    DATABUS,w           ; bus->w
  movwf   RegTmpG
; ---------------------------
ReadEnt2Loop
  clrwdt
  ; un pequeño retardo de 3ms
  movlw   0x03
  call    Delay_x1ms
  movf    DATABUS,w           ; bus->w
  subwf   RegTmpG,w
  btfss   STATUS,Z
  goto    ReadEnt2Bad          ; si las lecturas dan diferente vuelvo a empesar
  decfsz  ReadIRetry,f        ; voy decrementando la lectura si leí lo mismo
  goto    ReadEnt2Loop
; con tres lecturas identicas lo doy por bueno
ReadEnt2Ok
  ; inhabilito la interface de in2
  bsf     NIN2
  ; comparo lo del port con lo que tenia de antes
  movf    RegTmpG,w
  xorwf   ENTRADA2,w     ; dejo en w 1s donde hay diferencia
  btfsc   STATUS,Z
  goto    End_IO_Refresh      ; si no hay diferencia termino con I/O

  ; si hay diferencia me fijo si pasa la mascara
 ERRORLEVEL -302
  bsf     STATUS,RP0    ; --> BANK1
  andwf   MaskNotifE2,w
  btfsc   STATUS,Z
  goto    ReadEnt2OkFin       ; los cambios no estaban en la mascara de noticicacion
  bcf     STATUS,RP0    ; --> BANK0
 ERRORLEVEL +302
  ; preparo la notificación
  movlw   REINTENTOS
  movwf   CountNotifE2
ReadEnt2OkFin
  bcf     STATUS,RP0    ; --> BANK0
  movf    RegTmpG,w
  movwf   ENTRADA2
  bsf     CAMBIO_E2   ; levanto el flag que indica que cambió la entrada
  goto    End_IO_Refresh      ; si no hay diferencia termino con I/O
; ---------------------------
ReadEnt2Bad
  ; paso los datos del bus al registro temporal
  movf    DATABUS,w           ; bus->w
  movwf   RegTmpG              ; w->f
  ; veces que se lee cada entrada
  movlw   SENS_INPUT
  movwf   ReadIRetry
  goto    ReadEnt2Loop
; ----------------------------------------------------------------------------
; - Salidas ------------------------------------------------------------------
; refresco la salida 1 si está levantado el flag que me indica que cambió
WriteSal1
  ; me fijo si tengo que notificar los cambios
  movf    SALIDA1,w
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  xorwf   BKSALIDA1,w
  andwf   MaskNotifS1,w
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  btfsc   STATUS,Z
  goto    WriteSal1Cont   ; no tengo que notificar
  ; cargo los reintentos para notificar cambios en la salida
  movlw   REINTENTOS
  movwf   CountNotifS1
WriteSal1Cont
  bcf     CAMBIO_S1
  movf    SALIDA1,w     ;cargo dato
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  movwf   BKSALIDA1     ;dato->backup
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  movwf   DATABUS       ;dato->bus
  nop
  nop
  bsf     OUT1 ; -
  nop                   ;  |
  nop                   ;  | un pulso a la interface de I/O
  nop                   ;  |
  nop                   ;  |
  bcf     OUT1 ; -
  return
; - Salidas ------------------------------------------------------------------
; refresco la salida 2 si está levantado el flag que me indica que cambió
WriteSal2
  ; me fijo si tengo que notificar los cambios
  movf    SALIDA2,w
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  xorwf   BKSALIDA2,w
  andwf   MaskNotifS2,w
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  btfsc   STATUS,Z
  goto    WriteSal2Cont   ; no tengo que notificar
  ; cargo los reintentos para notificar cambios en la salida
  movlw   REINTENTOS
  movwf   CountNotifS2
WriteSal2Cont
  bcf     CAMBIO_S2
  movf    SALIDA2,w     ;cargo dato
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  movwf   BKSALIDA2     ;dato->backup
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  movwf   DATABUS       ;dato->bus
  nop
  nop
  bsf     OUT2 ; -
  nop                   ;  |
  nop                   ;  | un pulso a la interface de I/O
  nop                   ;  |
  nop                   ;  |
  bcf     OUT2 ; -
  return;
; - Final comun de las rutinas de refresh ------------------------------------
End_IO_Refresh
  bsf     NIN1
  bsf     NIN2
  bcf     OUT1
  bcf     OUT2
  nop
  nop
  nop
  nop
  return
; ============================================================================
PrintLunes
  movlw   'L'
  call    LCD_DATOS
  movlw   'u'
  call    LCD_DATOS
  movlw   'n'
  call    LCD_DATOS
  return
PrintMartes
  movlw   'M'
  call    LCD_DATOS
  movlw   'a'
  call    LCD_DATOS
  movlw   'r'
  call    LCD_DATOS
  return
PrintMiercoles
  movlw   'M'
  call    LCD_DATOS
  movlw   'i'
  call    LCD_DATOS
  movlw   'e'
  call    LCD_DATOS
  return
PrintJueves
  movlw   'J'
  call    LCD_DATOS
  movlw   'u'
  call    LCD_DATOS
  movlw   'e'
  call    LCD_DATOS
  return
PrintViernes
  movlw   'V'
  call    LCD_DATOS
  movlw   'i'
  call    LCD_DATOS
  movlw   'e'
  call    LCD_DATOS
  return
PrintSabado
  movlw   'S'
  call    LCD_DATOS
  movlw   'a'
  call    LCD_DATOS
  movlw   'b'
  call    LCD_DATOS
  return
PrintDomingo
  movlw   'D'
  call    LCD_DATOS
  movlw   'o'
  call    LCD_DATOS
  movlw   'm'
  call    LCD_DATOS
  return
; ============================================================================
PrintEnero
  movlw   'E'
  call    LCD_DATOS
  movlw   'n'
  call    LCD_DATOS
  movlw   'e'
  call    LCD_DATOS
  return
PrintFebrero
  movlw   'F'
  call    LCD_DATOS
  movlw   'e'
  call    LCD_DATOS
  movlw   'b'
  call    LCD_DATOS
  return
PrintMarzo
  movlw   'M'
  call    LCD_DATOS
  movlw   'a'
  call    LCD_DATOS
  movlw   'r'
  call    LCD_DATOS
  return
PrintAbril
  movlw   'A'
  call    LCD_DATOS
  movlw   'b'
  call    LCD_DATOS
  movlw   'r'
  call    LCD_DATOS
  return
PrintMayo
  movlw   'M'
  call    LCD_DATOS
  movlw   'a'
  call    LCD_DATOS
  movlw   'y'
  call    LCD_DATOS
  return
PrintJunio
  movlw   'J'
  call    LCD_DATOS
  movlw   'u'
  call    LCD_DATOS
  movlw   'n'
  call    LCD_DATOS
  return
PrintJulio
  movlw   'J'
  call    LCD_DATOS
  movlw   'u'
  call    LCD_DATOS
  movlw   'l'
  call    LCD_DATOS
  return
PrintAgosto
  movlw   'A'
  call    LCD_DATOS
  movlw   'g'
  call    LCD_DATOS
  movlw   'o'
  call    LCD_DATOS
  return
PrintSeptiembre
  movlw   'S'
  call    LCD_DATOS
  movlw   'e'
  call    LCD_DATOS
  movlw   'p'
  call    LCD_DATOS
  return
PrintOctubre
  movlw   'O'
  call    LCD_DATOS
  movlw   'c'
  call    LCD_DATOS
  movlw   't'
  call    LCD_DATOS
  return
PrintNoviembre
  movlw   'N'
  call    LCD_DATOS
  movlw   'o'
  call    LCD_DATOS
  movlw   'v'
  call    LCD_DATOS
  return
PrintDiciembre
  movlw   'D'
  call    LCD_DATOS
  movlw   'i'
  call    LCD_DATOS
  movlw   'c'
  call    LCD_DATOS
  return
; ============================================================================
Screen0
  call    LCD_LINE4
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   'S'
  call    LCD_DATOS
  movlw   'I'
  call    LCD_DATOS
  movlw   'S'
  call    LCD_DATOS
  movlw   'T'
  call    LCD_DATOS
  movlw   'E'
  call    LCD_DATOS
  movlw   'M'
  call    LCD_DATOS
  movlw   'A'
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   'I'
  call    LCD_DATOS
  movlw   'N'
  call    LCD_DATOS
  movlw   'I'
  call    LCD_DATOS
  movlw   'C'
  call    LCD_DATOS
  movlw   'I'
  call    LCD_DATOS
  movlw   'A'
  call    LCD_DATOS
  movlw   'D'
  call    LCD_DATOS
  movlw   'O'
  call    LCD_DATOS
  return
; ============================================================================
; Screen1
;
; ============================================================================
Screen1
  bsf     DISPLAY_CLOCK
  call    CLOCK_Refresh
  ;
  ; Linea 2
  ;
  call    LCD_LINE2
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  ;
  ; Linea 3
  ;
  ; Leyenda de inicializacion
  call    LCD_LINE3
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   'G'
  call    LCD_DATOS
  movlw   'n'
  call    LCD_DATOS
  movlw   'u'
  call    LCD_DATOS
  movlw   '-'
  call    LCD_DATOS
  movlw   'M'
  call    LCD_DATOS
  movlw   'H'
  call    LCD_DATOS
  movlw   'o'
  call    LCD_DATOS
  movlw   'm'
  call    LCD_DATOS
  movlw   'e'
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  ;
  ; Linea 4
  ;
  call    LCD_LINE4
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   'M'
  call    LCD_DATOS
  movlw   'I'
  call    LCD_DATOS
  movlw   'O'
  call    LCD_DATOS
  movlw   '-'
  call    LCD_DATOS
  movlw   '1'
  call    LCD_DATOS
  movlw   '6'
  call    LCD_DATOS
  movlw   'x'
  call    LCD_DATOS
  movlw   '1'
  call    LCD_DATOS
  movlw   '6'
  call    LCD_DATOS
  movlw   '/'
  call    LCD_DATOS
  movlw   'A'
  call    LCD_DATOS
  movlw   'D'
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS
  movlw   ' '
  call    LCD_DATOS

  return
; ============================================================================
 ERRORLEVEL +306
