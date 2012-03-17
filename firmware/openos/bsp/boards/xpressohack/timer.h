/**
\brief Cross-platform declaration "timer" bsp module.

\author Xavi Vilajosana <xvilajosana@eecs.berkeley.edu>, February 2012.
*/

#ifndef __TIMER_H
#define __TIMER_H

#include "stdint.h"

//=========================== define ==========================================

// TIME_INTERVALmS is used to convert a passed parameter (in ms) into a 
// compare value compatible with the timer's clock source. 
// depends on the prescaler and clock configuration TICS_PER_SECOND
#define TIME_INTERVALmS      1000
// ticks per second in the timer.
#define TICS_PER_SECOND      1000000

#define TIMER_COMPARE_REG0   0
#define TIMER_COMPARE_REG1   1
#define TIMER_COMPARE_REG2   2

#define TIMER_CAPTURE_REG0   6
#define TIMER_CAPTURE_REG1   7

#define TIMER_NUM0           0
#define TIMER_NUM1           3

//=========================== typedef =========================================

typedef void (*low_timer_hook)(uint8_t);

//=========================== variables =======================================

//=========================== prototypes ======================================

void     timer_init(uint8_t t_num);
void     timer_set_compare(uint8_t timer_num,
                           uint8_t compareReg,
                           uint32_t delayInMs);
void     timer_set_capture(uint8_t timer_num,uint8_t captureReg);
void     timer_enable( uint8_t timer_num );
void     timer_reset( uint8_t timer_num );
void     timer_disable( uint8_t timer_num );
uint32_t timer_get_current_value(uint8_t timer_num);
void     timer_reset_compare(uint8_t timer_num,uint8_t compareReg);

void     timer_set_isr_compare_hook(uint8_t timer_num, low_timer_hook cbt);
void     timer_set_isr_capture_hook(uint8_t timer_num, low_timer_hook cbt);

#endif /* TIMER_H_ */