/*
 * FreeRTOS SMP Kernel V202110.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 * 1 tab == 4 spaces!
 */

// TODO Guard this so it only arrives with the PICO
#include "pico_support.h"

#ifndef PORTMACRO_H
    #define PORTMACRO_H

    #ifdef __cplusplus
        extern "C" {
    #endif

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
    #define portCHAR          char
    #define portFLOAT         float
    #define portDOUBLE        double
    #define portLONG          long
    #define portSHORT         short
    #define portSTACK_TYPE    uint32_t
    #define portBASE_TYPE     long

    typedef portSTACK_TYPE   StackType_t;
    typedef long             BaseType_t;
    typedef unsigned long    UBaseType_t;

    #if ( configUSE_16_BIT_TICKS == 1 )
        typedef uint16_t     TickType_t;
        #define portMAX_DELAY              ( TickType_t ) 0xffff
    #else
        typedef uint32_t     TickType_t;
        #define portMAX_DELAY              ( TickType_t ) 0xffffffffUL

/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
 * not need to be guarded with a critical section. */
        #define portTICK_TYPE_IS_ATOMIC    1
    #endif
/*-----------------------------------------------------------*/

/* Architecture specifics. */
    #define portSTACK_GROWTH      ( -1 )
    #define portTICK_PERIOD_MS    ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
    #define portBYTE_ALIGNMENT    8
    #define portDONT_DISCARD      __attribute__( ( used ) )

    // TODO SMP this is in tasks.c & the xmos port
    #define portCRITICAL_NESTING_IN_TCB   1

/*-----------------------------------------------------------*/


/* Scheduler utilities. */
    extern void vPortYield( void );
    #define portNVIC_INT_CTRL_REG     ( *( ( volatile uint32_t * ) 0xe000ed04 ) )
    #define portNVIC_PENDSVSET_BIT    ( 1UL << 28UL )
    #define portYIELD()                                 vPortYield()
    #define portEND_SWITCHING_ISR( xSwitchRequired )    if( xSwitchRequired ) portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT
    #define portYIELD_FROM_ISR( x )                     portEND_SWITCHING_ISR( x )
/*-----------------------------------------------------------*/

/* SMP utilities */
    #define portGET_CORE_ID() (portSIE_CPUID)
// TODO SMP Implement this
    #define portYIELD_CORE(a) 

// TODO SMP NEW.  Restore the maskable interrupt state to the valued provided.
    #define portRESTORE_INTERRUPTS(ulState) 

// TODO SMP NEW. Return a true if we are currently in an ISR and false if we are NOT in an ISR.
    #define portCHECK_IF_IN_ISR() (pdFALSE)
/*-----------------------------------------------------------*/

/* Critical section management. */
    extern void vPortEnterCritical( void );
    extern void vPortExitCritical( void );
    extern uint32_t ulSetInterruptMaskFromISR( void ) __attribute__( ( naked ) );
    extern void vClearInterruptMaskFromISR( uint32_t ulMask )  __attribute__( ( naked ) );

    #define portSET_INTERRUPT_MASK_FROM_ISR()         ulSetInterruptMaskFromISR()
    #define portCLEAR_INTERRUPT_MASK_FROM_ISR( x )    vClearInterruptMaskFromISR( x )
    // TODO SMP XMOS modified DISABLE_INTERRUPTS to return a value
    #define portDISABLE_INTERRUPTS()                  ({ __asm volatile ( " cpsid i " ::: "memory" ); 0;})
    #define portENABLE_INTERRUPTS()                   __asm volatile ( " cpsie i " ::: "memory" )
    #define portENTER_CRITICAL()                      vPortEnterCritical()
    #define portEXIT_CRITICAL()                       vPortExitCritical()

/* PICO port will use the HW spinlocks for these */
/* Read Page 32 of the RP2040 datasheet to see how the spinlocks wrok */
/* using SPINLOCK0 for the ISR & SPINLOCK1 for the task */
#define portGET_ISR_LOCK()      do{ while(!portSIE_SPINLOCK0); }while(0)
#define portRELEASE_ISR_LOCK()  do{ portSIE_SPINLOCK0 = 1; }while(0)
#define portGET_TASK_LOCK()     do{ while(!portSIE_SPINLOCK1); }while(0)
#define portRELEASE_TASK_LOCK() do{ portSIE_SPINLOCK1 = 1; }while(0)

/*-----------------------------------------------------------*/

/* Tickless idle/low power functionality. */
    #ifndef portSUPPRESS_TICKS_AND_SLEEP
        extern void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime );
        #define portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime )    vPortSuppressTicksAndSleep( xExpectedIdleTime )
    #endif
/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
    #define portTASK_FUNCTION_PROTO( vFunction, pvParameters )    void vFunction( void * pvParameters )
    #define portTASK_FUNCTION( vFunction, pvParameters )          void vFunction( void * pvParameters )

    #define portNOP()

    #define portMEMORY_BARRIER()    __asm volatile ( "" ::: "memory" )

    #ifdef __cplusplus
        }
    #endif

#endif /* PORTMACRO_H */