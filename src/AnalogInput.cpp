/*
  Analog input, analog output, serial output

  Reads an analog input pin, maps the result to a range from 0 to 255 and uses
  the result to set the pulse width modulation (PWM) of an output pin.
  Also prints the results to the Serial Monitor.

  The circuit:
  - potentiometer connected to analog pin 0.
    Center pin of the potentiometer goes to the analog pin.
    side pins of the potentiometer go to +5V and ground
  - LED connected from digital pin 9 to ground through 220 ohm resistor

  created 29 Dec. 2008
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogInOutSerial
*/
#include <advancedSerial.h>

#include "AnalogInput.h"
#include "hardware.h"


int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

int p_nivel = 0, p_desviacion = 0;
int array_niveles[N_NIVELES], nivel = 0, nivel_outlier;
long media_llenado = 0;
float media = 0, desviacion_tipica = 255;
float media_outlier = 0, desviacion_tipica_outlier = 255;
unsigned long timeout_lectura = 0;

short contador_lecturas = 0;
const short n_filas = 40, n_columnas = 16;
short n_f, n_c;
short  media_OUTLIER_OUT [n_filas][n_columnas] = {
{147,149,151,146,148,154,149,147,151,150,150,152,149,150,151,149},
{325,149,151,146,148,154,149,147,151,150,150,152,149,150,151,149},
{325,151,151,146,148,154,149,147,151,150,150,152,149,150,151,149},
{325,151,327,146,148,154,149,147,151,150,150,152,149,150,151,149},
{325,151,327,149,148,154,149,147,151,150,150,152,149,150,151,149},
{325,151,327,149,323,154,149,147,151,150,150,152,149,150,151,149},
{325,151,327,149,323,149,149,147,151,150,150,152,149,150,151,149},
{325,151,327,149,323,154,152,147,151,150,150,152,149,150,151,149},
{325,151,327,149,323,154,152,326,151,150,150,152,149,150,151,149},
{325,151,327,149,323,154,152,326,324,150,150,152,149,150,151,149},
{325,151,327,149,323,154,152,326,324,326,150,152,149,150,151,149},
{325,151,327,149,323,154,152,326,324,326,150,152,149,150,151,149},
{325,151,327,149,323,154,152,326,324,326,150,327,149,150,151,149},
{325,151,327,149,323,154,152,326,324,326,150,327,325,150,151,149},
{325,151,327,149,323,154,152,326,324,326,150,327,325,324,151,149},
{325,151,327,149,323,154,152,326,324,326,150,327,325,324,326,149},
{325,151,327,149,323,154,152,326,324,326,150,327,325,324,326,325},
{323,151,327,149,323,154,152,326,324,326,150,327,325,324,326,325},
{323,325,327,149,323,154,152,326,324,326,150,327,325,324,326,325},
{323,325,322,149,323,154,152,326,324,326,150,327,325,324,326,325},
{323,325,322,324,323,154,152,326,324,326,150,327,325,324,326,325},
{323,325,322,324,323,154,152,326,324,326,150,327,325,324,326,325},
{323,325,322,324,323,322,152,326,324,326,150,327,325,324,326,325},
{323,325,322,324,323,322,326,326,324,326,150,327,325,324,326,325},
{323,325,322,324,323,322,326,324,324,326,150,327,325,324,326,325},
{323,325,322,324,323,322,326,324,326,326,150,327,325,324,326,325},
{323,325,322,324,323,322,326,324,326,326,150,327,325,324,326,325},
{323,325,322,324,323,322,326,324,326,326,329,327,325,324,326,325},
{323,325,322,324,323,322,326,324,326,326,329,322,325,324,326,325},
{323,325,322,324,323,322,326,324,326,326,329,322,325,324,326,325},
{323,325,322,324,323,322,326,324,326,326,329,322,325,324,326,325},
{323,325,322,324,323,322,326,324,326,326,329,322,325,324,326,325},
{323,325,322,324,323,322,326,324,326,326,329,322,325,324,326,325},
{323,325,322,324,323,322,326,324,326,326,329,322,325,324,326,325},
{323,325,322,324,323,322,326,324,326,326,329,322,325,324,326,325},
{323,325,322,324,323,322,326,324,326,326,329,322,325,324,326,325},
{323,325,322,324,323,322,326,324,326,326,329,322,325,324,326,325},
{323,325,322,324,323,322,326,324,326,326,329,322,325,324,326,325},
{323,325,322,324,323,322,326,324,326,326,329,322,325,324,326,325},
{323,325,322,324,323,322,326,324,326,326,329,322,325,324,326,325}
};

short media_OUTLIER_EQU[n_filas][n_columnas] = {
{147,149,151,146,148,154,149,147,151,150,150,152,149,150,151,149},
{325,149,151,146,148,154,149,147,151,150,150,152,149,150,151,149},
{325,151,151,146,148,154,149,147,151,150,150,152,149,150,151,149},
{325,151,327,146,148,154,149,147,151,150,150,152,149,150,151,149},
{325,151,327,149,148,154,149,147,151,150,150,152,149,150,151,149},
{325,151,327,149,323,154,149,147,151,150,150,152,149,150,151,149},
{325,151,327,149,323,149,149,147,151,150,150,152,149,150,151,149},
{325,151,327,149,323,154,152,147,151,150,150,152,149,150,151,149},
{325,151,327,149,323,154,152,326,151,150,150,152,149,150,151,149},
{325,151,327,149,323,154,152,326,324,150,150,152,149,150,151,149},
{325,151,327,149,323,154,152,326,324,149,150,152,149,150,151,149},
{325,151,327,149,323,154,152,326,324,149,150,152,149,150,151,149},
{325,151,327,149,323,154,152,326,324,149,150,327,149,150,151,149},
{325,151,327,149,323,154,152,326,324,149,150,327,148,150,151,149},
{325,151,327,149,323,154,152,326,324,149,150,327,148,151,151,149},
{325,151,327,149,323,154,152,326,324,149,150,327,148,151,149,149},
{325,151,327,149,323,154,152,326,324,149,150,327,148,151,149,325},
{325,321,327,149,323,154,152,326,324,149,150,327,148,151,149,325},
{325,321,327,333,323,154,152,326,324,149,150,327,148,151,149,325},
{325,321,327,333,323,332,152,326,324,149,150,327,148,151,149,325},
{325,321,327,333,323,332,329,326,324,149,150,327,148,151,149,325},
{325,321,327,333,323,332,329,326,324,339,150,327,148,151,149,325},
{325,321,327,333,323,332,329,326,324,339,150,327,148,151,149,325},
{325,321,327,333,323,332,329,326,324,339,350,327,148,151,149,325},
{325,321,327,333,323,332,329,326,324,339,350,327,148,151,149,325},
{325,321,327,333,323,332,329,326,324,339,350,327,148,151,149,325},
{149,146,327,149,323,154,152,326,324,149,150,327,348,351,349,325},
{149,146,148,149,323,154,152,326,324,149,150,327,148,151,149,325},
{149,146,148,145,323,154,152,326,324,149,150,327,148,151,149,325},
{149,146,148,145,147,154,152,326,324,149,150,327,148,151,149,325},
{149,146,148,145,147,149,152,326,324,149,150,327,148,151,149,325},
{149,146,148,145,147,149,145,326,324,149,150,327,148,151,149,325},
{149,146,148,145,147,149,145,148,324,149,150,327,148,151,149,325},
{149,146,148,145,147,149,145,148,147,149,150,327,148,151,149,325},
{149,146,148,145,147,149,145,148,147,149,150,327,148,151,149,325},
{149,146,148,145,147,149,145,148,147,149,150,144,148,151,149,325},
{149,146,148,145,147,149,145,148,147,149,150,144,148,151,149,325},
{149,146,148,145,147,149,145,148,147,149,150,144,148,151,147,151},
{149,146,148,145,147,149,145,148,147,149,150,144,148,151,147,151},
{149,146,148,145,147,149,145,148,147,149,150,144,148,151,147,151}
};

static void AnalogInput_CalcularMEDIA (int medida, char mostrar);
static void AnalogInput_CalcularMEDIA_INICIAL (int medida, char mostrar);
static void AnalogInput_CalcularMEDIA_OUTLIER_INICIAL (void);
static int AnalogInput_Analizar_Medida (int medida);
static void AnalogInput_CalcularMEDIA_OUTLIER (void);

void AnalogInput_setup (void) {
  // Stablish the resolution bits.
  analogReadResolution(12);
  sensorValue = analogRead(analogInPin);              // read the analog in value:
  outputValue = map(sensorValue, 0, 4095, 800, 70);   // map it to the range of the analog out:
  Serial.print("Medida Inicial Nivel = ");
  Serial.print(sensorValue);
  Serial.print("\t output = ");
  Serial.println(outputValue);
  AnalogInput_CalcularMEDIA_INICIAL (outputValue, 1);
  //AnalogInput_CalcularMEDIA_OUTLIER_INICIAL ();
}

int AnalogInput_loop_traditional (unsigned long t_actual) {
  float tension = 0;
  float fondoEscala = 3.3;
  float aux = 0;

  if ((t_actual - timeout_lectura) > TIMEOUT_LECTURA_NIVEL) {
    timeout_lectura = t_actual;                         //Se resetea el TIMEOUT.
    analogReadResolution(12);                           // Stablish the resolution bits.
    sensorValue = analogRead(analogInPin);              // read the analog in value:
    Serial.print("sensor = ");
    Serial.print(sensorValue);
    aux = (float)((fondoEscala*sensorValue)/4096);
    Serial.print("\t Auxilia r = ");
    Serial.print(aux);
    tension = (float)(800-((730*aux)/fondoEscala));
    Serial.print("\t Tension = ");
    Serial.print(tension);
    AnalogInput_CalcularMEDIA (tension, 1);
  }
  float desviacion_aux = AnalogInput_leer_Desviacion ();
  if (desviacion_aux >= DESVIACION_CRITICA) {
    return ANALOGINPUT_INFORMAR_DESVIACION_CRITICA;
  }
  return ANALOGINPUT_INFORMAR_DESVIACION_OK;
}


int AnalogInput_loop (unsigned long t_actual) {

  if ((t_actual - timeout_lectura) > TIMEOUT_LECTURA_NIVEL) {
    timeout_lectura = t_actual;                         //Se resetea el TIMEOUT.
    analogReadResolution(12);                           // Stablish the resolution bits.
    sensorValue = analogRead(analogInPin);              // read the analog in value:
    outputValue = map(sensorValue, 0, 4095, 800, 70);   // map it to the range of the analog out:
    // print the results to the Serial Monitor:
    //Serial.print("sensor = ");
    //Serial.print(sensorValue);
    //Serial.print("\t output = ");
    //Serial.println(outputValue);
/***********************************************************/
/**** SOLO PARA PRUEBAS ************************************/
/***********************************************************/
    //if (contador_lecturas <= N_NIVELES) {
    //if (contador_lecturas <= 3) {
      AnalogInput_CalcularMEDIA (outputValue, 1);
    //} else {
    //  AnalogInput_CalcularMEDIA_OUTLIER ();
    //}
    //contador_lecturas++;
  }
  float desviacion_aux = AnalogInput_leer_Desviacion ();
  if (desviacion_aux >= DESVIACION_CRITICA) {
    return ANALOGINPUT_INFORMAR_DESVIACION_CRITICA;
  }
  return ANALOGINPUT_INFORMAR_DESVIACION_OK;
}
int AnalogInput_leer_Nivel (void) {
  return nivel;
}
float AnalogInput_leer_Media (void) {
  return media;
}
float AnalogInput_leer_Desviacion (void) {
  return desviacion_tipica;
}
void AnalogInput_CalcularMEDIA_OUTLIER_INICIAL (void) {
  float media_inter = 0, resta, cuadrado=0, varianza_inter=0;

  contador_lecturas = 0;
  n_f = n_c = 0;
  for (int i = 0; i < N_NIVELES; i++) {
    //media_inter += media_OUTLIER_EQU[n_f][i];
    media_inter += media_OUTLIER_OUT[n_f][i];
  }
  media_outlier = media_inter / N_NIVELES;
  Serial.print (F("****---- MEDIA_OUTLIER: "));
  Serial.println (media_outlier);
  for (int i = 0; i < N_NIVELES; i++) {
    //resta = (float)media_OUTLIER_EQU[n_f][i] - media_outlier;
    resta = (float)media_OUTLIER_OUT[n_f][i] - media_outlier;
    cuadrado = pow (resta, 2);
    varianza_inter += cuadrado;
  }
  float varianza_outlier = (float) varianza_inter / (float) (N_NIVELES - 1);   //ESTO ES PORQUE ES n-1.
  desviacion_tipica_outlier = sqrt (varianza_outlier);
  Serial.print (F("***---DESVIACIÓN_OUTLIER: "));
  Serial.println (desviacion_tipica_outlier);
  Serial.println ();
  n_f++;
}
void AnalogInput_CalcularMEDIA_OUTLIER (void) { 
  float media_inter = 0, resta, cuadrado=0, varianza_inter=0;

  Serial.print (F("****---- NIVEL_OUTLIER: "));
  //nivel_outlier = media_OUTLIER_EQU[n_f][n_c];
  nivel_outlier = media_OUTLIER_OUT[n_f][n_c];
  Serial.print (nivel_outlier);
  Serial.print (F("\tFila: "));
  Serial.print (n_f);
  Serial.print (F("\t\tColumna: "));
  Serial.println (n_c);
  for (int i = 0; i < N_NIVELES; i++) {
    //media_inter += media_OUTLIER_EQU[n_f][i];
    media_inter += media_OUTLIER_OUT[n_f][i];
  }
  media_outlier = media_inter / N_NIVELES;
  Serial.print (F("****---- MEDIA_OUTLIER: "));
  Serial.println (media_outlier);
  for (int i = 0; i < N_NIVELES; i++) {
    //resta = (float)media_OUTLIER_EQU[n_f][i] - media_outlier;
    resta = (float)media_OUTLIER_OUT[n_f][i] - media_outlier;
    cuadrado = pow (resta, 2);
    varianza_inter += cuadrado;
  }
  float varianza_outlier = (float) varianza_inter / (float) (N_NIVELES - 1);   //ESTO ES PORQUE ES n-1.
  desviacion_tipica_outlier = sqrt (varianza_outlier);
  Serial.print (F("****---- DESVIACIÓN_OUTLIER: "));
  Serial.println (desviacion_tipica_outlier);
  Serial.println ();
  n_c++;
  n_f++;
  if (n_c >= n_columnas) n_c = 0;
  if (n_f >= n_filas) {
    AnalogInput_CalcularMEDIA_OUTLIER_INICIAL ();
  }
/***********************************************************/
/**** SOLO PARA PRUEBAS ************************************/
/***********************************************************/
  nivel = nivel_outlier;
  media = media_outlier;
  desviacion_tipica = desviacion_tipica_outlier;
}
/************************************************************************************************
 * FUNCIÓN: void mqprin_CalcularNivel (int medida, char mostrar)
 * DESCRIPCIÓN: Se encarga de calcular la MEDIA y la DESVIACIÓN TÍPICA
 * ARGUMENTOS DE ENTRADA:
 *    - MEDIDA: Es el nivel instantaneo del agua.
 *    - MOSTRAR:
 *        > 0: No muestra nada.
 *        > 1: Muestra el nivel, la media y la desviación típica.
 *************************************************************************************************/
void AnalogInput_CalcularMEDIA (int medida, char mostrar) {
  float media_inter = 0, varianza_inter = 0, cuadrado = 0, resta = 0;
  
  if (p_nivel >= N_NIVELES) {                //El array de niveles será cíclico.
    p_nivel = 0;                      //Se resetea el puntero de niveles.
  }
  //ESTO ES POR SI SE ENCUENTRA UN OUTLIER - NOS QUEDAMOS CON LA MEDIA
  //if (abs(medida - media) >= OUTLIER_MEDIDA && medida != MSB_ULTRASONIDO) {
  //  medida = media;
  //}
  int media_local = AnalogInput_Analizar_Medida (medida);
  nivel = media_local;
  array_niveles[p_nivel++] = medida;    //Se introduce en el array de niveles la medida recien tomada.
  //Serial.print (F("ARRAY: "));
  //for (int i = 0; i < N_NIVELES; i++) {
  //  Serial.print (array_niveles[i]);
  //  Serial.print (",");
  //}
  //Serial.println();
  
  if (mostrar == 1) {
    Serial.print (F("***--- DISTANCIA: "));
    Serial.print (medida);
  }
  for (int i = 0; i < N_NIVELES; i++) {
    media_inter += array_niveles[i];
  }
  media = media_inter / N_NIVELES;
  if (mostrar == 1) {
    Serial.print (F("\t***---Media: "));
    Serial.print (media);
  }
  for (int i = 0; i < N_NIVELES; i++) {
    resta = (float)array_niveles[i] - media;
    cuadrado = pow (resta, 2);
    varianza_inter += cuadrado;
  }
  float varianza = (float) varianza_inter / (float) (N_NIVELES - 1);   //ESTO ES PORQUE ES n-1.
  desviacion_tipica = sqrt (varianza);
  if (mostrar == 1) {
    Serial.print (F("\t***---Desviacion Tipica: "));
    Serial.println (desviacion_tipica);
  }
  Serial.println ();
}

void AnalogInput_CalcularMEDIA_INICIAL (int medida, char mostrar) {
  float media_inter = 0, varianza_inter = 0, cuadrado = 0, resta = 0;
  
  nivel = medida;
  if (mostrar == 1) {
    Serial.print (F("***--- NIVEL: "));
    Serial.println (medida);
  }
  Serial.print (F("ARRAY: "));
  for (int i = 0; i < N_NIVELES; i++) {
    array_niveles[i] = medida;    //Se introduce en el array de niveles la medida recien tomada.
    Serial.print (array_niveles[i]);
    Serial.print (",");
  }
  Serial.println();
  for (int i = 0; i < N_NIVELES; i++) {
    media_inter += array_niveles[i];
  }
  media = media_inter / N_NIVELES;
  if (mostrar == 1) {
    Serial.print (F("***--- Media: "));
    Serial.println (media);
  }
  for (int i = 0; i < N_NIVELES; i++) {
    resta = (float)array_niveles[i] - media;
    cuadrado = pow (resta, 2);
    varianza_inter += cuadrado;
  }
  float varianza = (float) varianza_inter / (float) (N_NIVELES - 1);   //ESTO ES PORQUE ES n-1.
  desviacion_tipica = sqrt (varianza);
  if (mostrar == 1) {
    Serial.print (F("***--- Desviacion Tipica: "));
    Serial.println (desviacion_tipica);
  }
}

/*************************************************************************
 * *** Función para analizar la medida recien tomada, ya que a veces hay picos
 * *** de tensión, pero hay que tener en cuenta que de repente se estropee el
 * *** Ultrasonidos y de como medida 800 o que de repente se arregle y vuelva
 * *** a medir bien, con lo cual la medida bajará hasta un valor lógico y coherente.
 * *** HAY QUE PENSARLA MUY BIEN.
 * **********************************************************************/
int AnalogInput_Analizar_Medida (int medida) {
  return medida;
}