/*  =========================================================================
    az_poller - class description

    Copyright (C) 2014 - 2017 Eaton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    =========================================================================
*/

#ifndef AZ_POLLER_H_INCLUDED
#define AZ_POLLER_H_INCLUDED

#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Session.h>

namespace msg = qpid::messaging;

//  @interface
//  Create a new az_poller
AZPOLLER_PRIVATE az_poller_t *
    az_poller_new (msg::Connection connection);

// Unwrap the actor
AZPOLLER_PRIVATE zactor_t *
    az_poller_actor (az_poller_t *self);

//  Destroy the az_poller
AZPOLLER_PRIVATE void
    az_poller_destroy (az_poller_t **self_p);

//  Self test of this class
AZPOLLER_PRIVATE void
    az_poller_test (bool verbose);

//  @end

#endif
