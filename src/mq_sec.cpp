/*******************************************************************************
   File   : mqprin.c  Module that integers the functions of machine states
********************************************************************************
   Description:In this module is called the functions of machine states.
********************************************************************************
   Creation of module: 25 of Apr of 2006
   Last modification:
   Name of creator of module: Ángel Ropero
   Software P2100
   Links with another files. mqprin.c.
   Company: HYC
*******************************************************************************/
/*** Includes ***/
#include <string.h>
#include <stdlib.h>
#include <advancedSerial.h>

#include "mq_sec.h"
#include "maqsec.h"
#include "datos.h"
#include "hardware.h"
#include "protocolo.h"
#include "Lora_imp.h"
#include "AnalogInput.h"

#define VACIO     0

//-------------------- GLOBAL VARIABLES AL MODULO ------------------------------
extern MAQFLAGSEC maqsec_bits;
_MQSEC_FLAGS maqflags;
unsigned long T_anterior, timeout_ACK = 0, T_reenvio_niveles, T_reenvios_PING = 0, timeout_VINCULAR = 0;
int Last_status = INICIAL;
float nivel_anterior, media_anterior, desviacion_anterior;
int n_reenvios = 0, contador_reenvios_PING = 0;
int Last_command = LORA_NULL;

//-------------------- GLOBAL FUNCTIONS ----------------------------------------
static int mqsec_Chech_SN_PCBRELES (void);
static void mqsec_Enviar_COMANDO_LORA (int Comando_Enviar);
//------------------------------------------------------------------------------
//------------------- STATES OF FUNCIONALITIES ---------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FUNCTION: short mqsec_EstINICIAL_SEC (void)
// DESCRIPTION: Function that executes the functionalities of INICIAL state.
// PARAMETERS: Void.
// RETURNS:  0.
//------------------------------------------------------------------------------
short mqsec_EstINICIAL (unsigned long T_actual) {          //Este estado le dejamos ahí por si hiciera falta
  
  if (maqsec_bits.flag_1A_vez == false) {   //First time that is entried to state.
    maqsec_SetFlag1A ();               //Set the flag of first time.
    T_anterior = T_actual;
    Serial.println (F("ESTADO INICIAL"));
    Last_status = INICIAL;
  }
  //------------------------------------ PROCCESS --------------------------------
  int result = mqsec_Chech_SN_PCBRELES ();
  if (result == 0)  maqsec_ActivarCondicion (CS1);   //Enviar al estado de VINCULAR.
  else              maqsec_ActivarCondicion (CS0);   //Enviar al estado de REPOSO.
  return 0;
}

//------------------------------------------------------------------------------
// FUNCTION: short mqsec_EstESPERA (void)
// DESCRIPTION: Function that executes the functionalities of REPOSO state.
// PARAMETERS: Void.
// RETURNS:  0.
//------------------------------------------------------------------------------
short mqsec_EstREPOSO (unsigned long T_actual, int comando_LoRa, int desviacion_aux) {
  //byte valor_EEPROM;

  if (maqsec_bits.flag_1A_vez == false) {   //First time that is entried to state.
    maqsec_SetFlag1A ();               //Set the flag of first time.
    T_anterior = T_actual;
    Serial.println (F("ESTADO REPOSO: "));
    Last_status = REPOSO;
    Last_command = LORA_NULL;
    n_reenvios = 0;
    maqflags.flag_vincular = false;
    maqflags.flag_ACK = false;
    maqflags.flag_desviacion = false;
  }
  //------------------------------------ PROCCESS --------------------------------
  if (comando_LoRa == LORA_VINCULACION_FORZADA) {
    Last_command = LORA_VINCULACION_FORZADA;
    maqflags.flag_ACK = true;
    timeout_ACK = T_actual;
  }
  /********************************************************************************
   ***** RECEPCIÓN DE UN COMANDO PING, AL QUE HAY QUE RESPONDER CON UN ACK  *******
   ********************************************************************************/
  if (comando_LoRa == LORA_PING) {
    maqflags.flag_ACK = true;
    timeout_ACK = T_actual;
    T_reenvio_niveles = T_actual;
    maqflags.niveles = false;
  }
  if (maqflags.flag_ACK == true && (T_actual - timeout_ACK) > TIMEOUT_SEND_ACK) {
    maqflags.flag_ACK = false;
    mqsec_Enviar_COMANDO_LORA (LORA_ACK);
    if (Last_command == LORA_VINCULACION_FORZADA) {
      datos_EEPROM_Borrar_SNRELES ();
      maqsec_ActivarCondicion (CS1);   //Enviar al estado de VINCULAR.
    }
  }

  if (comando_LoRa == LORA_ACK) {
    maqflags.niveles = false;
    n_reenvios = 0;
    maqflags.flag_vincular = false;
    contador_reenvios_PING = 0;
  }
  if (comando_LoRa == LORA_TEST_NIVELES) {
    T_anterior = T_actual - (TIMEOUT_ENVIO_LORA_NIVELES - 1000);
    T_reenvio_niveles = T_actual;
    media_anterior = 0;           //Para que cuando entre vea que la media ha cambiado.
  }
  if (T_actual > T_reenvio_niveles) { //Esto es para evitar que el valor negativo afecte a la comprobación.
    if ((T_actual - T_reenvio_niveles) > TIMEOUT_REENVIO_NIVELES && maqflags.niveles == true) {
      n_reenvios++;
      if (n_reenvios >= NUMERO_MAXIMO_REENVIOS) {
        n_reenvios = 0;
        maqflags.niveles = false;
        mqsec_Enviar_COMANDO_LORA (LORA_ALARM_ERROR_RECEPCION);   //SE INDICA QUE NO SE RECIBE EL COMANDO DE VUELTA.
        Serial.println (F("---- ERROR EN RECEPCIÓN DE COMANDO ACK ----"));
        return 0;
      }
      T_reenvio_niveles = (T_actual + (unsigned long) (random (2, 5)*1000));
      T_anterior = (T_actual + (unsigned long) (random (2, 5)*1000));
      mqsec_Enviar_COMANDO_LORA (LORA_NIVELES);   //SE ENVÍA EL COMANDO DE ENVÍO DE NIVELES.
      Serial.println (F("---- REENVIO: AQUÍ SE TRANSMITEN LOS NIVELES ----"));
    }
    if (desviacion_aux == ANALOGINPUT_INFORMAR_DESVIACION_CRITICA && maqflags.flag_desviacion == false) {
      mqsec_Enviar_COMANDO_LORA (LORA_NIVELES);   //SE ENVÍA EL COMANDO DE ENVÍO DE NIVELES.
      maqflags.niveles = maqflags.flag_desviacion = true;
      T_reenvio_niveles = T_actual;
    } else if (desviacion_aux == ANALOGINPUT_INFORMAR_DESVIACION_OK) {
      maqflags.flag_desviacion = false;
    }
    if ((T_actual - T_anterior) > TIMEOUT_ENVIO_LORA_NIVELES && maqflags.niveles == false) {
      T_anterior = T_actual;
      if ((int) AnalogInput_leer_Media () != (int) media_anterior ||
                AnalogInput_leer_Desviacion () > (float) DESVIACION_CRITICA) {
        media_anterior = AnalogInput_leer_Media ();
        mqsec_Enviar_COMANDO_LORA (LORA_NIVELES);   //SE ENVÍA EL COMANDO DE ENVÍO DE NIVELES.
        //Serial.println (F("---- AQUÍ TIMEOUT_ENVIO_LORA_NIVELES ----"));
        maqflags.niveles = true;
        T_reenvio_niveles = T_actual;
      }
    }
  }
  return 0;
}
//------------------------------------------------------------------------------
// FUNCTION: short mqsec_EstCALLADO (void)
// DESCRIPTION: Function that executes the functionalities of REPOSO state.
// PARAMETERS: Void.
// RETURNS:  0.
//------------------------------------------------------------------------------
short mqsec_EstVINCULAR (unsigned long T_actual, int comando_LoRa) {

  if (maqsec_bits.flag_1A_vez == false) {   //First time that is entried to state.
    maqsec_SetFlag1A ();               //Set the flag of first time.
    T_anterior = T_actual;
    Serial.println (F("ESTADO VINCULAR"));
    Last_status = VINCULAR;
    timeout_VINCULAR = T_actual;
  }
  if ((T_actual - timeout_VINCULAR) > TIMEOUT_VINCULACION) {
    timeout_VINCULAR = T_actual;
    mqsec_Enviar_COMANDO_LORA (LORA_ALARM_ERROR_RECEPCION);   //SE INDICA QUE NO SE RECIBE EL COMANDO DE VUELTA.
    Serial.println (F("---- VINCULAR: ERROR EN RECEPCIÓN DE COMANDO ACK ----"));
  }
  if (comando_LoRa == LORA_VINCULAR) {
    maqflags.flag_vincular = true;
    int result = mqsec_Chech_SN_PCBRELES ();
    if (result == NO_HAY_GRABADO_NINGUN_SNRELES) {
      Serial.println (F("¡¡¡¡ GRABAR SN_RECEIVER !!!!"));
      lora_imp_GRABAR_SNRECEIVER ();
      maqflags.flag_ACK = true;
      timeout_ACK = T_actual;
    } else {
      Serial.println (F("¡¡¡¡ HACER EL PING AL RECEIVER ALMACENADO !!!!"));
    }
  }
  if (comando_LoRa == LORA_PING) {
    maqflags.flag_ACK = true;
    timeout_ACK = T_actual;
  }
  if (maqflags.flag_ACK == true && (T_actual - timeout_ACK) > TIMEOUT_SEND_ACK) {
    maqflags.flag_ACK = false;
    mqsec_Enviar_COMANDO_LORA (LORA_ACK);
    maqsec_ActivarCondicion (CS0);
  }
  return 0;
}
//------------------------------------------------------------------------------
// FUNCTION: short mqsec_EstPING (void)
// DESCRIPTION: Estado que sirve para que cuando se reciba un PING se responda un ACK
//              teniendo en cuenta que no se puede enviar inmediatamente.
// PARAMETERS: Recibe el T_actual y el comando LoRa.
// RETURNS:  .
//------------------------------------------------------------------------------
short mqsec_EstPING (unsigned long T_actual) {

  if (maqsec_bits.flag_1A_vez == false) {   //First time that is entried to state.
    maqsec_SetFlag1A ();               //Set the flag of first time.
    T_anterior = T_actual;
    Serial.println (F("ESTADO PING"));
  }
  //------------------------------------ PROCCESS --------------------------------
  return 0;
}
//------------------------------------------------------------------------------
// FUNCTION: short mqsec_EstALARMAS (void)
// DESCRIPTION: Function that executes the functionalities of REPOSO state.
// PARAMETERS: Void.
// RETURNS:  0.
//------------------------------------------------------------------------------
short mqsec_EstACK (unsigned long T_actual, int comando_LoRa) {

  if (maqsec_bits.flag_1A_vez == false) {   //First time that is entried to state.
    maqsec_SetFlag1A ();               //Set the flag of first time.
    T_anterior = T_actual;
    Serial.println (F("ESTADO ACK"));
    timeout_ACK = T_actual;
  }
  //------------------------------------ PROCCESS --------------------------------
  if ((T_actual - timeout_ACK) > TIMEOUT_SEND_ACK) {
    timeout_ACK = T_actual;
    mqsec_Enviar_COMANDO_LORA (LORA_ACK);
    switch (Last_status) {
      case REPOSO:
        maqsec_ActivarCondicion (CS0);    //Devolver al estado de REPOSO.
        Serial.println (F("SALIR PARA REPOSO"));
        break;
      case VINCULAR:
        maqsec_ActivarCondicion (CS1);    //Devolver al estado de VINCULACIÓN.
        Serial.println (F("SALIR PARA VINCULAR"));
        break;
      default:
        Serial.println (F("SALIR PARA DEFAULT"));
        maqsec_ActivarCondicion (CS0);
        break;
    }
  }
  return 0;
}

int mqsec_Chech_SN_PCBRELES (void) {
  byte valor_EEPROM[11];

  memset (valor_EEPROM, 0, sizeof(valor_EEPROM));
  Serial.print (F("El S/N de la PCB_RELES es: "));
  for (int i = 0; i < DATOS_NUMERO_SERIE; i++) {
    valor_EEPROM[i] = datos_EEPROM_Read_BYTE (DAT_SN_RECEIVER + i);
    Serial.print ((char) valor_EEPROM[i]);
  }
  Serial.println ();
  if (valor_EEPROM[0] != 'S' || valor_EEPROM[1] != 'M' || valor_EEPROM[2] != 'F' || valor_EEPROM[3] != 'R') {
    Serial.println (F("¡¡¡¡¡NO HAY S/N PCB RELES!!!!!"));
    return NO_HAY_GRABADO_NINGUN_SNRELES;
  } else {
    return SI_HAY_GRABADO_ALGUN_SNRELES;
  }
}

void mqsec_Enviar_COMANDO_LORA (int Comando_Enviar) {
  char comando[N_MAXIMO_CARACTERES_MENSAJE];
  char mensaje_local[N_MAXIMO_CARACTERES_MENSAJE];
  char nivel_char[4], media_char[10], desviacion_char[10];
  int nivel_local;
  float media_local, desviacion_local;

  memset (comando, 0, sizeof(comando));
  memset (mensaje_local, 0, sizeof(mensaje_local));
  switch (Comando_Enviar) {
  case LORA_ACK:
    strcpy (comando, "#ACK*");
    break;
  case LORA_PING:
    strcpy (comando, "#PING*");
    break;
  case LORA_ALARM_ERROR_RECEPCION: 
    strcpy (comando, "#ALARM_RECEPCION*");
    break;
  case LORA_NIVELES:
    memset (nivel_char,       0, sizeof(nivel_char));
    memset (media_char,       0, sizeof(media_char));
    memset (desviacion_char,  0, sizeof(desviacion_char));
    //NIVEL
    strcpy (comando, "#NIVEL:");
    nivel_local = AnalogInput_leer_Nivel ();
    sprintf(nivel_char, "%i", nivel_local);
    strcat (comando, nivel_char);
    //MEDIA
    strcat (comando, "#MEDIA:");
    media_local = AnalogInput_leer_Media ();
    //dtostrf(media_local,10,2,media_char); //Llamada a la función
    sprintf (media_char, "%.2f", media_local);
    strcat (comando, media_char);
    //DESVIACIÓN TÍPICA
    strcat (comando, "#D_T:");
    desviacion_local = AnalogInput_leer_Desviacion ();
    //dtostrf(desviacion_local,10,2,desviacion_char); //Llamada a la función
    sprintf (desviacion_char, "%.2f", desviacion_local);
    strcat (comando, desviacion_char);
    strcat (comando, "*");
    break;
  default:
    break;
  }
  int total_lenght = protocolo_Mensaje_pruebas (comando, strlen(comando), mensaje_local);
  Lora_imp_SendPacket (mensaje_local, total_lenght);
}
