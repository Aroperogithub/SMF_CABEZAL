#include <Arduino.h>
#include <Wire.h>
#include <Time.h>

#include "hardware.h"
#include "maqsec.h"
#include "esp_system.h"
#include "Lora_imp.h"
#include "datos.h"
#include "protocolo.h"
#include "AnalogInput.h"
#include "xxtea_Easy.h"

unsigned long t_actual, T_secundario;
hw_timer_t *timer = NULL;
const int wdtTimeout = TIMEOUT_WATCHDOG;

static void inicializacion_Pines (void);

void IRAM_ATTR resetModule () {
  ets_printf ("reboot\n");
  esp_restart();
}

void setup() {
  Serial.begin (115200);
  inicializacion_Pines ();
  datos_IniciarEEPROM ();
  //datos_Reset_MemoriaEEPROM ();     //DESCOMENTAR ESTA PARTE PARA LLEVAR EL EQUIPO A VINCULAR
  datos_Parametrizar_MemoriaEEPROM ();
  maqsec_IniciacionVbles ();                        //Initias all variables of State Machine.
  Lora_imp_setup ();
  protocolo_Init ();
  AnalogInput_setup ();
  xxtea_Easy_setup ();
  //WATCHDOG
  timer = timerBegin (0, 80, true);
  timerAttachInterrupt (timer, &resetModule, true);
  timerAlarmWrite (timer, wdtTimeout * 1000, false);
  timerAlarmEnable (timer);
}
 
void loop() {
  unsigned long T_actual;
  int comando_LoRa = LORA_NULL, desviacion_aux;

  timerWrite (timer, 0);
  T_actual = millis ();                             //Captura de los milisegundos transcurridos.
  desviacion_aux = AnalogInput_loop (T_actual);                      //LEE CADA X SEGUNDOS DEL ULTRASONIDOS.
  //Lora_imp_loop (T_actual);
  comando_LoRa = Lora_imp_CheckPacket ();
  maqsec_SECUNDARIA (T_actual, comando_LoRa, desviacion_aux);
}
void inicializacion_Pines (void) {
  pinMode       (led_sender,          OUTPUT);
  digitalWrite  (led_sender,          NIVEL_BAJO);        //Pulso para apagar el FOCOIRIS.
}

 
