#ifndef AEV_APPLEVENTS_INCLUDED
#define AEV_APPLEVENTS_INCLUDED
/**
 * @file tc12/aev_applEvents.h
 * Definition of application events. The application events are managed in a
 * central file to avoid inconistencies and accidental double usage.
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

/*
 * Include files
 */


/*
 * Defines
 */

/** A semaphore is used to signal the number of elements in the shared queue. */
#define EVT_SEMAPHORE_ELEM_IN_QUEUE     (RTOS_EVT_SEMAPHORE_00)

/** A mutex is used synchronize the access to the terminal output for reporting. */
#define EVT_MUTEX_SERIAL                (RTOS_EVT_MUTEX_01)

/** This event is sent by the irregular idle task to trigger the queue reading task. */
#define EVT_TRIGGER_CONSUMER_TASK       (RTOS_EVT_EVENT_02)


/*
 * Global type definitions
 */


/*
 * Global data declarations
 */


/*
 * Global prototypes
 */



#endif  /* AEV_APPLEVENTS_INCLUDED */
