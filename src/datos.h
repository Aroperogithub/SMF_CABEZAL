//=============================================================================
//                       IMPOREXO
//                      (c) Copyright 2017
//=============================================================================
#ifndef _DATOS_H_
#define _DATOS_H_

  #define EEPROM_SIZE                 512
  #define EEPROM_OK                   0
  #define EEPROM_KO                   -1

  //DATOS EN LA EEPROM
  #define DAT_SN_SENDER     0           //9 BYTES PARA EL NÚMERO DE SERIE DE ESTE DISPOSITIVO.
  #define DAT_SN_RECEIVER   10          //5 PUBLISHERs o MOVILES.
  #define DAT_PUBLISHER2    20
  #define DAT_PUBLISHER3    30
  #define DAT_PUBLISHER4    40
  #define DAT_PUBLISHER5    50
  #define DAT_BROKER1       60          //5 BROKERs o REPETIDORES.
  #define DAT_BROKER2       70
  #define DAT_BROKER3       80
  #define DAT_BROKER4       90
  #define DAT_BROKER5       100
  #define DAT_SUBSCRIBER1   110         //8 SUBSCRIBERs o DISPOSITIVOS.
  #define DAT_SUBSCRIBER2   120
  #define DAT_SUBSCRIBER3   130
  #define DAT_SUBSCRIBER4   140
  #define DAT_SUBSCRIBER5   150
  #define DAT_SUBSCRIBER6   160
  #define DAT_SUBSCRIBER7   170
  #define DAT_SUBSCRIBER8   180
  #define DAT_MAX_DATOS     190

  //LA DISPOSICIÓN DE TODOS LOS DATOS DE LA EEPROM ESTÁ EN EL FICHERO HARDWARE.H, donde tiene más sentido, ya que dependerá del equipo en sí y no de las rutinas del módulo Datos.cpp/h.

  extern int datos_IniciarEEPROM (void);
  extern unsigned int datos_EEPROM_Read (int tipo);
  extern void datos_EEPROM_Read_Memoria (int inicioEEPROM, int finalEEPROM);
  extern void datos_Parametrizar_MemoriaEEPROM (void);
  extern void datos_Reset_MemoriaEEPROM (void);

  extern byte datos_EEPROM_Read_BYTE (int tipo);
  extern void datos_EEPROM_Write_BYTE (byte datos, int tipo);
  extern int  datos_EEPROM_Read_INT (int num);
  extern void datos_EEPROM_Write_INT (int datos, int tipo);
  extern long datos_EEPROM_Read_LONG (int num);
  extern void datos_EEPROM_Write_LONG (long datos, int tipo);

  extern void datos_EEPROM_Borrar_SNRELES (void);
#endif // _DATOS_H_
