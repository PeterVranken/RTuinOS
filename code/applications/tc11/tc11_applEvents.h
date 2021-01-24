#ifndef TC11_APPLEVENTS_INCLUDED
#define TC11_APPLEVENTS_INCLUDED
/**
 * @file tc11_applEvents.h
 * Definition of application events. The application events are managed in a
 * central file to avoid inconistencies and accidental double usage.
 *
 * Copyright (C) 2012-2013 Peter Vranken (mailto:Peter_Vranken@Yahoo.de)
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

/** The resource is owned by the task, which received this event of kind mutex. */
#define EVT_MUTEX_OWNING_RESOURCE (RTOS_EVT_MUTEX_00)

/** This event is used as start condition for task T0_C0. */
#define EVT_START_TASK_T0_C0 (RTOS_EVT_EVENT_01)

/** This event is used as start condition for task T1_C0. */
#define EVT_START_TASK_T1_C0 (RTOS_EVT_EVENT_02)

/** This event is used as start condition for task T2_C0. */
#define EVT_START_TASK_T2_C0 (RTOS_EVT_EVENT_03)


/*
 * Global type definitions
 */


/*
 * Global data declarations
 */


/*
 * Global prototypes
 */



#endif  /* TC11_APPLEVENTS_INCLUDED */
