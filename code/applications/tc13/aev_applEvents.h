#ifndef AEV_APPLEVENTS_INCLUDED
#define AEV_APPLEVENTS_INCLUDED
/**
 * @file aev_applEvents.h
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

#define SEM_0                           (RTOS_EVT_SEMAPHORE_00)
#define SEM_1                           (RTOS_EVT_SEMAPHORE_01)
                                        
#define MTX_0                           (RTOS_EVT_MUTEX_02)
#define MTX_1                           (RTOS_EVT_MUTEX_03)
                                        
#define EVT_0                           (RTOS_EVT_EVENT_04)
#define EVT_1                           (RTOS_EVT_EVENT_05)

/** This event is sent by the control task to trigger the next step. The three events need
    to be of neighboured indexes, as they are used by index, like
    (\a EVT_TRIGGER_TASK << \a idxTask). */
#define EVT_TRIGGER_TASK                (RTOS_EVT_EVENT_06)
#define EVT_TRIGGER_T0                  (RTOS_EVT_EVENT_06)
#define EVT_TRIGGER_T1                  (RTOS_EVT_EVENT_07)
#define EVT_TRIGGER_T2                  (RTOS_EVT_EVENT_08)


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
