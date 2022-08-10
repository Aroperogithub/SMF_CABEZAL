/********************************************************************************
*  Fichero: datos.c  Modulo que integra la estructura de datos del programa     *
*********************************************************************************
*  Descripcion: En este módulo se definen las funciones que manejan la EEPROM   *
*              usada por las diferentes funciones en el programa.               *
*********************************************************************************
*  Creacion del módulo: 30 de Septiembre de 2017                                *
*  Last modification:                                                           *
*  Name of creator of module: Ángel Ropero                                      *
*  Software Arduino.                                                            *
*  Enlaza con otros ficheros: mqprinc.c                                         *
*  Company: IMPOREXO                                                            *
********************************************************************************/
#include <advancedSerial.h>
#include <EEPROM.h>     //Incluye la librería de memoria EEPROM de la placa de ARDUINO UNO.
#include <string.h>
#include <stdlib.h>
#include <Wire.h>

#include "datos.h"
#include "hardware.h"

//#define DEBUG(a) Serial.println(a);

//DEFINICIÓN DE VARIABLES GLOBALES

//DEFINICIÓN DE FUNCIONES LOCALES AL MÓDULO

int datos_IniciarEEPROM (void) {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println(F("Failed to initialise EEPROM")); 
    return EEPROM_KO;
  }
  
  Serial.println (F("EEPROM Initialized properly"));
  return EEPROM_OK;
}

unsigned int datos_EEPROM_Read (int tipo) {
  //int datos = 0;
  
  int datos = (int) (((byte)EEPROM.read(tipo) << 8) | (byte)EEPROM.read(tipo + 1));
  //Serial.print ("TIPO-R: ");
  //Serial.println (tipo);
  //Serial.print ("DATOS MSB-R: ");
  //Serial.println ((byte) (EEPROM.read(tipo)), HEX);
  //Serial.print ("DATOS LSB-R: ");
  //Serial.println ((byte) (EEPROM.read(tipo + 1)), HEX);
  return datos;
}

void datos_EEPROM_Read_Memoria (int inicioEEPROM, int finalEEPROM)  {
  byte dato;

  Serial.println ("Datos EEPROM: ");
  for (int i = inicioEEPROM; i <= finalEEPROM; i++) {
    dato = 0;
    dato = (byte) EEPROM.read(i);
    Serial.print (i);
    Serial.print (": ");
    Serial.print (dato, HEX);
    Serial.print (" ");
    if ((i % 4) == 0) Serial.println ();
  }
  Serial.println ();
}

void datos_Parametrizar_MemoriaEEPROM (void) {
  if (EEPROM.read (DAT_MAX_DATOS) == 0xAA) {
    Serial.println ("Memoria EEPROM ya parametrizada");
    return;
  }
  Serial.println ("Parametrizacion Memoria EEPROM");
  for (int i = DAT_SN_SENDER; i < DAT_MAX_DATOS; i++) {
    EEPROM.write(i, 0x00);
  }
  EEPROM.write (DAT_MAX_DATOS, 0xAA);
  EEPROM.commit();
}

void datos_Reset_MemoriaEEPROM (void) {
  EEPROM.write (DAT_MAX_DATOS, 0x00);
  EEPROM.commit();
  Serial.println (F("--- MEMORIA RESETEADA - DAT_MAX_DATOS = 0x00. ---"));
}
void datos_Reset_Parcial_MemoriaEEPROM (int INI_DATOS, int FIN_DATOS) {
  Serial.print ("PARCIAL MEM EEPROM: ");
  for (int i = INI_DATOS; i < FIN_DATOS; i++) {
    Serial.print (i);
    Serial.print (", ");
    EEPROM.write(i, 0x00);
  }
  Serial.println ();
  EEPROM.commit();
}

/**********************************************************************************
 * FUNCIONES DE ESCRITURA/LECTURA DE MEMORIA POR TIPOS DE DATOS: BYTE, INT, LONG
 **********************************************************************************/
byte datos_EEPROM_Read_BYTE (int tipo) {
  byte dato = 0;

  dato = (byte) EEPROM.read(tipo);
  return dato;
}
void datos_EEPROM_Write_BYTE (byte datos, int tipo) {
  EEPROM.write (tipo, datos);
  EEPROM.commit();
}
int datos_EEPROM_Read_INT (int num) {
  int unitId = 0;

  unitId = (int)EEPROM.read(num + 0) << 8 | (int)EEPROM.read(num + 1);
  return unitId;
}
void datos_EEPROM_Write_INT (int datos, int tipo) {
  //Serial.print ("TIPO-W: ");
  //Serial.println (tipo);
  //Serial.print ("DATOS MSB-W: ");
  //Serial.println ((byte) (datos >> 8), HEX);
  //Serial.print ("DATOS LSB-W: ");
  //Serial.println ((byte) datos, HEX);
  EEPROM.write (tipo, (byte) (datos >> 8));
  EEPROM.write (tipo + 1,(byte) datos);
  EEPROM.commit();
}
long datos_EEPROM_Read_LONG (int num) {
  long unitId = 0;

  unitId = (long)EEPROM.read(num + 0) << 24 | (long)EEPROM.read(num + 1) << 16 | (long)EEPROM.read(num + 2) << 8 | (long)EEPROM.read(num + 3);
  return unitId;
}
void datos_EEPROM_Write_LONG (long datos, int tipo) {
    EEPROM.write(tipo + 0, (byte) (datos >> 24));
    EEPROM.write(tipo + 1, (byte) (datos >> 16));
    EEPROM.write(tipo + 2, (byte) (datos >> 8));
    EEPROM.write(tipo + 3, (byte) datos);
    EEPROM.commit();
}

void datos_EEPROM_Borrar_SNRELES (void) {
  for (int i = 0; i < DATOS_NUMERO_SERIE; i++) {
    datos_EEPROM_Write_BYTE (0, DAT_SN_RECEIVER + i);
  }
  Serial.println (F("////***** BORRADO EL SN_RELES (RECEIVER) *****//////"));
}
