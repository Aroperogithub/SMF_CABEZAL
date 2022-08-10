//=============================================================================
//                       ARP
//                      (c) Copyright 2021
//=============================================================================
#ifndef _MQSEC_H_
#define _MQSEC_H_

/*******************************************************************************
*  Fichero: mqprinc.h       Módulo que integra la estructura de la máquina de  *
*                           estados.                                           *
********************************************************************************
*  Descripcion:  En este módulo se definen las funciones de la máquina de estados*
*   utilizadas después por las diferentes máquinas.                *
********************************************************************************
*  Creacion del modulo: 19 de Sep de 2017                                      *
*  Ultima modificacion:                                                        *
*  Nombre del creador del modulo:  Ángel Ropero                                *
*  Software Arduino                                                              *
*  Links con otros ficheros.                                                   *
*  Empresa. IMPOREXO                                                           *
*******************************************************************************/
#define N_MAXIMO 500
#define FONDO 0xFFFF

#define TIMEOUT_ESTADOS             2000
#define TIMEOUT_SEND_ACK            1000
#define TIMEOUT_ENVIO_LORA_NIVELES  15000
#define TIMEOUT_REENVIO_NIVELES     5000
#define NUMERO_MAXIMO_REENVIOS      5
#define TIMEOUT_REENVIOS_PING       3000
#define NUMERO_MAXIMO_REENVIOS_PING 5
#define TIMEOUT_VINCULACION         15000

#define NO_HAY_GRABADO_NINGUN_SNRELES 0
#define SI_HAY_GRABADO_ALGUN_SNRELES  1

typedef volatile struct _MQSEC_FLAGS {
    unsigned bit7             : 1; // BIT 7 - UNUSED
    unsigned bit6             : 1; //     6 - UNUSED
    unsigned bit5             : 1; //     5 - UNUSED
    unsigned bit4             : 1; //     4 - UNUSED
    unsigned flag_desviacion  : 1; //     3 - UNUSED
    unsigned flag_vincular    : 1; //     2 - UNUSED
    unsigned flag_ACK         : 1; //     1 - UNUSED
    unsigned niveles          : 1; //     0 - UNUSED
} __attribute__((packed)) MQSEC_FLAGS;

/*------------------- ESTADOS DE FUNCIONAMIENTO ---------------------*/


  extern short mqsec_EstINICIAL   (unsigned long T_actual);
  extern short mqsec_EstREPOSO    (unsigned long T_actual, int comando_LoRa, int desviacion_aux);
  extern short mqsec_EstVINCULAR  (unsigned long T_actual, int comando_LoRa);
  extern short mqsec_EstPING      (unsigned long T_actual);
  extern short mqsec_EstACK (unsigned long T_actual, int comando_LoRa);

#endif //_MQSEC_H_
