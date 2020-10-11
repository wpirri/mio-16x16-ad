; ############################################################################
; ZONA TX
; ############################################################################
; ============================================================================
; ClrTx - limpia el area donde se arma el mensaje a enviar
; ============================================================================
ClrTx
  ; blanqueo los datos
  movlw   ' '
  movwf   txtyp
  movwf   txdst
  movwf   txcnt
  movwf   txfcn
  movwf   txdat1
  movwf   txdat2
  movwf   txdat3
  movwf   txdat4
  movwf   txdat5
  movwf   txdat6
  movwf   txdat7
  movwf   txdat8
  movwf   txdat9
  movwf   txdat10
  ; inicializo el origen
  movf    LocAddr,w
  movwf   txsrc
  return
; ============================================================================
; TxEnqueue - se llama para cargar el registro armado para transmitir en la
;             cola de salida
; ============================================================================
TxEnqueue
  btfsc   TXBUFFULL
  return  ; el buffer está lleno, no se puede hacer nada
  bsf     STATUS,IRP ; pasa a banco 2/3 para direccionamiento indirecto
  movf    TxPtrAdd,w ; cargo la dirección donde escribir en el buffer
  movwf   FSR
  clrf    txchksum  ; inicializo el checksum
  ; voy moviendo los datos y calculando el checksum mientras tanto
  movf    txtyp,w
  movwf   INDF
  addwf   txchksum,f
  incf    FSR,f
  call    TxEnqueueCtrlVuelta
  movf    txdst,w 
  movwf   INDF
  addwf   txchksum,f
  incf    FSR,f
  call    TxEnqueueCtrlVuelta
  movf    txsrc,w
  movwf   INDF
  addwf   txchksum,f
  incf    FSR,f
  call    TxEnqueueCtrlVuelta
  movf    txcnt,w
  movwf   INDF
  addwf   txchksum,f
  incf    FSR,f
  call    TxEnqueueCtrlVuelta
  movf    txfcn,w
  movwf   INDF
  addwf   txchksum,f
  incf    FSR,f
  call    TxEnqueueCtrlVuelta
  movf    txdat1,w
  movwf   INDF
  addwf   txchksum,f
  incf    FSR,f
  call    TxEnqueueCtrlVuelta
  movf    txdat2,w
  movwf   INDF
  addwf   txchksum,f
  incf    FSR,f
  call    TxEnqueueCtrlVuelta
  movf    txdat3,w
  movwf   INDF
  addwf   txchksum,f
  incf    FSR,f
  call    TxEnqueueCtrlVuelta
  movf    txdat4,w
  movwf   INDF
  addwf   txchksum,f
  incf    FSR,f
  call    TxEnqueueCtrlVuelta
  movf    txdat5,w
  movwf   INDF
  addwf   txchksum,f
  incf    FSR,f
  call    TxEnqueueCtrlVuelta
  movf    txdat6,w
  movwf   INDF
  addwf   txchksum,f
  incf    FSR,f
  call    TxEnqueueCtrlVuelta
  movf    txdat7,w
  movwf   INDF
  addwf   txchksum,f
  incf    FSR,f
  call    TxEnqueueCtrlVuelta
  movf    txdat8,w
  movwf   INDF
  addwf   txchksum,f
  incf    FSR,f
  call    TxEnqueueCtrlVuelta
  movf    txdat9,w
  movwf   INDF
  addwf   txchksum,f
  incf    FSR,f
  call    TxEnqueueCtrlVuelta
  movf    txdat10,w
  movwf   INDF
  addwf   txchksum,f
  incf    FSR,f
  call    TxEnqueueCtrlVuelta
  movf    txchksum,w
  movwf   INDF
  ; un incremento mas para que quede apuntando al siguiente
  incf    FSR,f
  call    TxEnqueueCtrlVuelta
  ; actualizo el puntero de escritura de la cola circular
  movf    FSR,w
  movwf   TxPtrAdd
;  bcf     STATUS,IRP    ; vuelvo a banco 0/1 de direccionamiento indirecto
TxEnqueueCtrlBuffer
  ; controlo si el buffer está lleno (comparo puntero de escritura y lectura)
  movf    TxPtrAdd,w
  subwf   TxPtrSnd,w
  btfsc   STATUS,Z
  bsf     TXBUFFULL  ; está lleno
TxEnqueueEnd
  ;incf    Variant,f
  return
; ============================================================================
; TxEnqueueCtrlVuelta
; ============================================================================
TxEnqueueCtrlVuelta
  movf    FSR,w
  sublw   TXMAXBUFFER
  btfsc   STATUS,Z      ; (w == TXMAXBUFFER)
  goto    TxEnqueueCtrlVueltaSi
  btfss   STATUS,C      ; (w < TXMAXBUFFER)
  goto    TxEnqueueCtrlVueltaSi
  return
;------------------------------------------------
TxEnqueueCtrlVueltaSi
  ; vuelvo al inicio de la cola circular
  movlw   TXINITBUFFER
  movwf   FSR
  return
; ============================================================================
; TxDequeue
; ============================================================================
TxDequeueCtrl
  ; me fijo si el buffer está lleno
  btfsc     TXBUFFULL
  goto      TxDequeueCtrlUp     ; está lleno, activo la transmisión
  ; me fijo si hay algo para transmitir por
  ; comparacion de inicio y fin del buffer
  movf    TxPtrAdd,w
  subwf   TxPtrSnd,w
  btfss   STATUS,Z
  goto    TxDequeueCtrlUp
  return  ; nada che!
TxDequeueCtrlUp
  ; habilito la interrupción de Tx
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  btfsc   PIE1,TXIE
  goto    TxDequeueCtrlEnd ; si está activada la int no hago nada mas
  bsf     PIE1,TXIE
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  call    TxInt
TxDequeueCtrlEnd
  bcf     STATUS,RP0 ; --> BANK0
  return
; ============================================================================
; TxInt - se llama desde el programa para iniciar el envío de un mensaje y
;         desde una interrupcion para continuarlo
; ============================================================================
TxInt
  bcf     STATUS,RP0  ; --> BANK0
  bcf     STATUS,RP1  ; \_ por si vengo de INT
  btfss   PIR1,TXIF
  return  ; el buffer está ocupado, cuando se vacíe la interrupcion se encarga
  ; el buffer está vacío o hubo interrupción
  ; me fijo si hay algo para transmitir
  ; por comparacion de inicio y fin del buffer
  movf    TxPtrAdd,w
  subwf   TxPtrSnd,w
  btfss   STATUS,Z
  goto    TxIntCont
  ; inicio y fin son iguales => no hay mensajes o el buffer está lleno
  btfsc   TXBUFFULL
  return
  ; no hay nada, inhabilito la interrupcion
 ERRORLEVEL -302
  bsf     STATUS,RP0 ; --> BANK1
  bcf     PIE1,TXIE
  bcf     STATUS,RP0 ; --> BANK0
 ERRORLEVEL +302
  return  
TxIntCont
  bsf     STATUS,IRP ; pasa a banco 2/3 para direccionamiento indirecto
  movf    TxPtrSnd,w
  movwf   FSR
  movf    INDF,w
  movwf   TXREG
  incf    FSR,f
  ; ahora controlo si lleguó al final del buffer
  movf    FSR,w
  sublw   TXMAXBUFFER
  btfsc   STATUS,Z      ; (w == TXMAXBUFFER)
  goto    TxIntResetBuffer
  btfss   STATUS,C      ; (w < TXMAXBUFFER)
  goto    TxIntResetBuffer
  ; cargo la dirección del puntero
  movf    FSR,w
  movwf   TxPtrSnd
  goto    TxIntEnd
TxIntResetBuffer
  ; vuelvo al principio del buffer
  movlw   TXINITBUFFER
  movwf   TxPtrSnd
TxIntEnd
;  bcf     STATUS,IRP    ; vuelvo a BANK 0/1 de direccionamiento indirecto
  bcf     TXBUFFULL ; borro el flag por si el buffer estaba lleno
  return
; ############################################################################
; ZONA RX
; ############################################################################
; ============================================================================
; RxInt - interrupcion por caracter recibido
; ============================================================================
RxInt
  movf    RCREG,w
  btfsc   RXBUFFULL
  goto    RxIntErr  ; el buffer de recepción está lleno, descarto lo recibido
  movwf   RxChar
  ; Controlo el dato recibido según la cantidad de bytes que tengo del mensaje
  bcf     RXCHARERR
  bcf     RXCHAREND
  call    ControlRxByte
  btfsc   RXCHARERR
  goto    RxIntErr    ; el caracter recibido no sirve
  btfsc   RXCHAREND
  goto    RxIntEnd    ; el caracter recibido fue el ultimo (ChkSum)
  ; el caracter recibido está OK
  ; incremento el contador de caracteres para este mensaje
  incf    RxCharCount,f
  movf    RxChar,w
  addwf   RxCharSum,f     ; mantengo el checksum
  return
;------------------------------------------------
RxIntErr
  ; en caso de error de recepción tiro todo lo recibido como mensaje
  movf    RxPtrAdd,w
  movwf   RxPtrAddTmp
  clrf    RxCharCount
  return
;------------------------------------------------
RxIntEnd
  movf    RxPtrAddTmp,w
  movwf   RxPtrAdd      ; adelanto el puntero del buffer
  clrf    RxCharCount   ; reseteo el contador de caracteres
  return
; ============================================================================
; ControlRxByte - Control de los bytes recibidos
; ============================================================================
ControlRxByte
  bcf     RXCHARERR ; reseteo el flag de error
  movf    RxCharCount,w
  btfsc   STATUS,Z
  goto    ControlRxStart    ; rxtyp   control del primer byte
  ;
  movf    RxCharCount,w
  sublw   0x02
  btfsc   STATUS,Z
  goto    ControlSrc        ; rxsrc
  ;
  movf    RxCharCount,w
  sublw   0x0F
  btfsc   STATUS,Z
  goto    ControlChkSum     ; rxchksum
  ;
  goto    RxAddBuffer
; ============================================================================
; Funciones de control de byte recibido
; - Lo controlan según su orden, lo mueven según corresponda
; * Se mueve a la cola circular:
;     rxtyp   
;     rxdst - para saber si es a mi a a todos   
;     rxsrc   
;     rxcnt   
;     rxfcn   
;     rxdat1  
;     rxdat2  
;     rxdat3  
;     rxdat4  
;     rxdat5  
;     rxdat6  
;     rxdat7  
;     rxdat8  
;     rxdat9  
;     rxdat10 
;
; ============================================================================
ControlRxStart
  movf    RxChar,w
  sublw   'Q'
  btfsc   STATUS,Z
  goto    ControlRxStartOk
  movf    RxChar,w
  sublw   'R'
  btfsc   STATUS,Z
  goto    ControlRxStartOk
  movf    RxChar,w
  sublw   'E'
  btfsc   STATUS,Z
  goto    ControlRxStartOk
  bsf     RXCHARERR ; si no es ninguno de los anteriores hay un error
  return
ControlRxStartOk
  clrf    RxCharSum   ; reseteo el checksum
  call    RxAddBuffer
  ; cargo un time-out de 250ms para que termine de llegar el mensaje
  movlw   0x19
  movwf   RxTimeOut
  return
; -----------------
ControlSrc
  movf    RxChar,f
  btfss   STATUS,Z
  bsf     RXCHARERR ;si el origen no era el host (unico origen permitido)
  btfsc   STATUS,Z
  call    RxAddBuffer
  return
; -----------------
ControlChkSum
  movf    RxCharSum,w
  subwf   RxChar,w
  btfss   STATUS,Z
  bsf     RXCHARERR  ;el checksum no dió
  btfss   STATUS,Z
  return
  ; si el checksum dió OK tengo un mensaje !
  bsf     RXCHAREND
  return
; ============================================================================
;  CheckRxOverflow - Verifica los flags de overflow y paridad
; ============================================================================
CheckRxOverflow
  btfss   RCSTA,OERR
  return
  bcf     RCSTA,CREN    ; reseteo la recepción
  bsf     RCSTA,CREN
  call    LongBeep
  return
; ============================================================================
; ClrTx - limpia 
; ============================================================================
ClrRx
  movlw   ' '
  movwf   rxtyp
  movwf   rxdst
  movwf   rxsrc
  movwf   rxcnt
  movwf   rxfcn
  movwf   rxdat1
  movwf   rxdat2
  movwf   rxdat3
  movwf   rxdat4
  movwf   rxdat5
  movwf   rxdat6
  movwf   rxdat7
  movwf   rxdat8
  movwf   rxdat9
  movwf   rxdat10
  movwf   rxchksum
  return
; ============================================================================
; RxAddBuffer - agrega el contenido del registro RxChar al buffer de recepcion
; ============================================================================
RxAddBuffer
  btfsc   RXBUFFULL
  return    ; no hay lugar!!!
  bsf     STATUS,IRP ; pasa a banco 2/3 para direccionamiento indirecto
  movf    RxPtrAddTmp,w ; cargo la dirección donde escribir en el buffer
  movwf   FSR
  ; muevo el dato recibido
  movf    RxChar,w
  movwf   INDF
  ; incremento mas para que quede apuntando al siguiente
  incf    FSR,f
;  bcf     STATUS,IRP    ; vuelvo a banco 0/1 de direccionamiento indirecto
  ; ahora controlo si llegué al final del buffer
  movf    FSR,w
  sublw   RXMAXBUFFER   ; (RXMAXBUFFER - w)
  btfsc   STATUS,Z      ; (w == RXMAXBUFFER)
  goto    RxAddBufferVuelta
  btfss   STATUS,C      ; (w < RXMAXBUFFER)
  goto    RxAddBufferVuelta
;------------------------------------------------
RxAddBufferSetAddr
  ; cargo la dirección del puntero
  movf    FSR,w
  movwf   RxPtrAddTmp
  goto    RxAddBufferCtrlFull
;------------------------------------------------
RxAddBufferVuelta
  ; llegué al final de la cola circular y pego la vuelta
  movlw   RXINITBUFFER
  movwf   RxPtrAddTmp
;------------------------------------------------
RxAddBufferCtrlFull
  ; controlo si el buffer está lleno
  movf    RxPtrAddTmp,w
  subwf   RxPtrRcv,w
  btfsc   STATUS,Z
  bsf     RXBUFFULL  ; está lleno
  return
; ============================================================================
; RxDequeue - saca un mensaje de la cola de recepción, lo coloca en el registro
;             de recepción y llama a ProcessCmd
; ============================================================================
RxDequeue
  ; si el buffer está lleno no tengo nada mas que mirar
  btfsc   RXBUFFULL
  goto    RxDequeueMsg
  ; si el buffer no está lleno comparo los punteros
  movf    RxPtrAdd,w
  subwf   RxPtrRcv,w
  btfsc   STATUS,Z
  return  ; son iguales, no hay nada en el buffer
;------------------------------------------------
RxDequeueMsg
  ; muevo los datos al registro de adentro
  movf    RxPtrRcv,w
  movwf   FSR
  bsf     STATUS,IRP ; pasa a banco 2/3 para direccionamiento indirecto
  movf    INDF,w
  movwf   rxtyp
  incf    FSR,f
  call    RxDequeueCtrlVuelta
  movf    INDF,w
  movwf   rxdst
  incf    FSR,f
  call    RxDequeueCtrlVuelta
  movf    INDF,w
  movwf   rxsrc
  incf    FSR,f
  call    RxDequeueCtrlVuelta
  movf    INDF,w
  movwf   rxcnt
  incf    FSR,f
  call    RxDequeueCtrlVuelta
  movf    INDF,w
  movwf   rxfcn
  incf    FSR,f
  call    RxDequeueCtrlVuelta
  movf    INDF,w
  movwf   rxdat1
  incf    FSR,f
  call    RxDequeueCtrlVuelta
  movf    INDF,w
  movwf   rxdat2
  incf    FSR,f
  call    RxDequeueCtrlVuelta
  movf    INDF,w
  movwf   rxdat3
  incf    FSR,f
  call    RxDequeueCtrlVuelta
  movf    INDF,w
  movwf   rxdat4
  incf    FSR,f
  call    RxDequeueCtrlVuelta
  movf    INDF,w
  movwf   rxdat5
  incf    FSR,f
  call    RxDequeueCtrlVuelta
  movf    INDF,w
  movwf   rxdat6
  incf    FSR,f
  call    RxDequeueCtrlVuelta
  movf    INDF,w
  movwf   rxdat7
  incf    FSR,f
  call    RxDequeueCtrlVuelta
  movf    INDF,w
  movwf   rxdat8
  incf    FSR,f
  call    RxDequeueCtrlVuelta
  movf    INDF,w
  movwf   rxdat9
  incf    FSR,f
  call    RxDequeueCtrlVuelta
  movf    INDF,w
  movwf   rxdat10
  ; un incremento mas para dejarlo en el siguiente
  incf    FSR,f
  call    RxDequeueCtrlVuelta
;  bcf     STATUS,IRP    ; vuelvo a banco 0/1 de direccionamiento indirecto
  ; actualizo el puntero de recepción con el valor
  ; del registro de direccionamiento indirecto
  movf    FSR,w
  movwf   RxPtrRcv
  ; borro el flag de buffer lleno
  bcf     RXBUFFULL  ; por si estaba lleno lleno
  ; voy a procesar el comando
  goto    ProcessCmd           ; proceso lo recibido
;------------------------------------------------
; controla si tiene que volver al principio de la cola circular
RxDequeueCtrlVuelta
  movf    FSR,w
  sublw   RXMAXBUFFER
  btfsc   STATUS,Z      ; (w == RXMAXBUFFER)
  goto    RxDequeueCtrlVueltaSi
  btfss   STATUS,C      ; (w < RXMAXBUFFER)
  goto    RxDequeueCtrlVueltaSi
  return
;------------------------------------------------
RxDequeueCtrlVueltaSi
  ; vuelvo al inicio de la cola circular
  movlw   RXINITBUFFER
  movwf   FSR
  return
