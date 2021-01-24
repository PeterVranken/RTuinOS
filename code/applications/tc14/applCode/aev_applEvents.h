#ifndef AEV_APPLEVENTS_INCLUDED
#define AEV_APPLEVENTS_INCLUDED
/**
 * @file tc14/applCode/aev_applEvents.h
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
 
#include "rtos.h"


/*
 * Defines
 */

/** A mutex is applied to share the display between different tasks. */
#define EVT_MUTEX_LCD                       (RTOS_EVT_MUTEX_00)

/** An ordinary event is used to trigger the idle-follower task, which is capable to
    acquire the display for displaying the results of the idle task. */
#define EVT_TRIGGER_IDLE_FOLLOWER_TASK      (RTOS_EVT_EVENT_01)

/** An ordinary event is used to trigger the button evaluation task. */
#define EVT_TRIGGER_TASK_BUTTON             (RTOS_EVT_EVENT_02)

/** An ordinary event is used to trigger the ADC result display task. */
#define EVT_TRIGGER_TASK_DISPLAY_VOLTAGE    (RTOS_EVT_EVENT_03)

/** A simple event is used to signal a new ADC conversion result. */
#define EVT_ADC_CONVERSION_COMPLETE         (RTOS_EVT_ISR_USER_00)


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
