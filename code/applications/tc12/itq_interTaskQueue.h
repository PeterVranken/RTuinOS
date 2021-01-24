#ifndef ITQ_INTERTASKQUEUE_INCLUDED
#define ITQ_INTERTASKQUEUE_INCLUDED
/**
 * @file itq_interTaskQueue.h
 * Definition of global interface of module itq_interTaskQueue.c
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


/*
 * Global type definitions
 */


/*
 * Global data declarations
 */


/*
 * Global prototypes
 */

void itq_writeElem(int16_t queuedElem);
int16_t itq_readElem();


#endif  /* ITQ_INTERTASKQUEUE_INCLUDED */
