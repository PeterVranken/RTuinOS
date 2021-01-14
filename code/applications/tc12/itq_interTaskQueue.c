/**
 * @file itq_interTaskQueue.c
 *   Implementation of a queue for inter task communication. Such a queue is thread safe; a
 * task writing to the one end will never corrupt data because of another task reading or
 * writing at the same time. Read and write are atomic operations and can be invoked at any
 * time by any task.\n
 *   A related semaphore indicates at any time a lower bound of the number of contained
 * elements. A task acquiring the semaphore (or one count of it respectively) can be sure to
 * find at least one element in the queue. (As writing to the task and incrementing
 * the semaphore are not atomic, the number of queued data elements can temporarily differ
 * from the number signaled by the semaphore, which doesn't matter because of the strong
 * statement made before.)
 *   @remark The semaphore, which is related to the queue, is defined in tc12_applEvents.h.
 * Neither the relation to the semaphore nor the interface of the queue is designed generic
 * for sake of code reusability; the main intention is to demonstrate the synchronization
 * features of RTuinOS.
 *
 * Copyright (C) 2013 Peter Vranken (mailto:Peter_Vranken@Yahoo.de)
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/* Module interface
 *   itq_writeElem
 *   itq_readElem
 * Local functions
 */

/*
 * Include files
 */

#include <Arduino.h>
#include "rtos.h"
#include "rtos_assert.h"
#include "itq_interTaskQueue.h"
#include "aev_applEvents.h"


/*
 * Defines
 */
 
 
/*
 * Local type definitions
 */
 
 
/*
 * Local prototypes
 */
 
 
/*
 * Data definitions
 */

/** The implementation of the queue is most simple using a ring buffer. The size needs to be
    256 as all cyclically incremented index are uint8_t without further masking or modulo
    operations. Therefore we avoid using a macro here. */
static int16_t _ringBuf[256];

/** Incremented after writing. FIFO full if new position is equal to _readPos. This is
    considered an error. */
static uint8_t _writePos = 0;   

/** Incremented after reading. FIFO empty if equal to _writePos. */
static uint8_t _readPos = _writePos;

 
/*
 * Function implementation
 */


/**
 * Append an element to the queue. The implementation has a fixed maximum number of
 * elements. An assertion fires if this limit would be exceeded by this call of the
 * function. (Error handling is out of scope of this sample.)\n
 *   After appending the element the related semaphore (see tc12_applEvents.h) is
 * incremented.\n
 *   The function can be invoked by any task at any time. The implementation uses a
 * critical section. This function must not be used inside a critical section of the
 * caller, that outer critical section would be broken. (Critical sections in RTuinOS are
 * not implemented recursively.)
 *   @param queuedElem
 * The queue stored integer values. The passed value is appended to the queue.
 *   @see int16_t itq_readElem(void)
 */

void itq_writeElem(int16_t queuedElem)
{
    rtos_enterCriticalSection();    
    {
        _ringBuf[_writePos++] = queuedElem;
        
        /* A productive implementation would probably deny writing and return an error
           indication. We don't need such code as it simply is a requirement of our test
           case that an overrun must never happen. */
        ASSERT(_writePos != _readPos);
    }
    rtos_leaveCriticalSection();
    
    /* Only now, after completion of the actual queuing the semaphore is incremented to
       signal the new data to possibly waiting consumer tasks. Queuing and signaling is
       not atomic, we already left the critical section. This is however not dangerous. The
       meaning of the semaphore is not to precisely indicate the number of queued elements
       but to give a guarantee that there's at least one element in case I could acquire
       the semaphore.
         By the way, incrementing the semaphore by a call of rtos_sendEvent implicitly ends
       a critical section. We could implement write and increment as atomic by simply
       omitting the call of rtos_leaveCriticalSection - this would however make the code
       less readable and is useless. */
    rtos_sendEvent(EVT_SEMAPHORE_ELEM_IN_QUEUE);
    
} /* End of itq_writeElem */




/**
 * Read next element from the queue. The caller of the function needs to have the related
 * semaphore acquired; this guarantees the availability of at least one element in the
 * queue. An assertion fires if not.
 *   @return
 * The element read from the queue is returned.
 *   @see void itq_writeElem(int16_t)
 */

int16_t itq_readElem()
{
    int16_t elem;
    
    rtos_enterCriticalSection();    
    {
        /* Even a productive implementation would place the assertion here - this situation
           is different to the similar assertion in the counterpart itq_writeElem! */
        ASSERT(_writePos != _readPos);
        
        elem = _ringBuf[_readPos++];
    }
    rtos_leaveCriticalSection();
    
    return elem;

} /* End of itq_readElem */




