#ifndef __PID_H
#define __PID_H

#include "main.h"

//一个开关周期数字量 
#define PERIOD 30000

void PID_Init(void);
void BuckBoostVILoopCtlPID(void);

#endif
