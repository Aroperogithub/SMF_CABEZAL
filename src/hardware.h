//=============================================================================
//                       IMPOREXO
//                      (c) Copyright 2017
//=============================================================================
#ifndef _HARDWARE_H_
#define _HARDWARE_H_

  #define TIMEOUT_WATCHDOG      5000
  #define N_NIVELES             16
  //DATOS
  #define DATOS_NUMERO_SERIE    10
  //ANALOGINPUT
  #define TIMEOUT_LECTURA_NIVEL 5000
  //LORA_IMP
  #define TIMEOUT_SEND_LORA     15000
  #define N_MAXIMO_CARACTERES_MENSAJE   250
  #define N_SERIALNUMBER                20
  #define COMIENZO_PAYLOAD              24    //20 SN_SENDER + 20 SN_RECEIVER + 8 LONGITUD

//PINES DE ENTRADA
  const int analogInPin = A0;

  #define NSS 17
  #define RESET 16
  #define DIO0  15
  //PINES DE SALIDA

  #define led_sender 2

  #define NIVEL_BAJO  0
  #define NIVEL_ALTO  1


#endif //_HARDWARE_H_
