//=============================================================================
//                       ARP
//                      (c) Copyright 2021
//=============================================================================
#ifndef _ANALOGINPUT_H_
#define _ANALOGINPUT_H_

  #define OUTLIER_MEDIDA    100
  #define MSB_ULTRASONIDO   800
  #define DESVIACION_CRITICA  38

  #define ANALOGINPUT_INFORMAR_DESVIACION_OK         0
  #define ANALOGINPUT_INFORMAR_DESVIACION_CRITICA   -1

  extern void AnalogInput_setup (void);
  extern int AnalogInput_loop (unsigned long t_actual);
  extern int AnalogInput_loop_traditional (unsigned long t_actual);
  extern int   AnalogInput_leer_Nivel (void);
  extern float AnalogInput_leer_Media (void);
  extern float AnalogInput_leer_Desviacion (void);

#endif    //_ANALOGINPUT_H_