/*  =========================================================================
    azpoller - Email transport

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

/*
@header
    azpoller - Email transport
@discuss
@end
*/
#include "azpoller_classes.h"

#include <qpid/messaging/Address.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Session.h>
#include <qpid/messaging/Receiver.h>


//  Structure of our class

struct _az_poller_t {
    zpoller_t *mlm_poller;
    msg::Session amqp_session;
};


//  --------------------------------------------------------------------------
//  Create a new az_poller

az_poller_t *
az_poller_new (msg::Connection connection, ...)
{
    az_poller_t *self = new az_poller_t;
    assert (self);
    //  Initialize class properties here
    va_list zreaders;
    va_start (zreaders, connection);
    self->mlm_poller = zpoller_new (zreaders);
    self->amqp_session = connection.createSession ();
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the az_poller

void
az_poller_destroy (az_poller_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        az_poller_t *self = *self_p;
        //  Free class properties here
        zpoller_destroy (&self->mlm_poller);
        delete &self->amqp_session;
        //  Free object itself
        delete self;
        *self_p = NULL;
    }
}

void
az_poller_actor (zsock_t *pipe, void *session) {
    zsock_signal (pipe, 0);
    zpoller_t *pipepoller = zpoller_new (pipe, NULL);

    qpid::messaging::Session *QS = (qpid::messaging::Session *)session;

    while (!zsys_interrupted) {
        void *which = zpoller_wait (pipepoller, 0);
        if (which) {
            // pipe command
            zmsg_t *msg = zmsg_recv (pipe);
            if (msg) {
                char *cmd = zmsg_popstr (msg);
                if (cmd) {
                    if (streq (cmd, "$TERM")) {
                        zstr_free (&cmd);
                        zmsg_destroy (&msg);
                        break;
                    }
                    zstr_free (&cmd);
                }
                zmsg_destroy (&msg);
            }
        }
        if (zsys_interrupted) {
            break;
        }
        if (QS) {
            // solve Amqp
            try {
                qpid::messaging::Receiver R;
                if (QS->nextReceiver (R, qpid::messaging::Duration::SECOND)) {
                    qpid::messaging::Message M = R.get();
                    // do something
                }
            } catch (...) {
                zclock_sleep (1000);
            }
        }
    }
}

//  --------------------------------------------------------------------------
//  Self test of this class

void
az_poller_test (bool verbose)
{
    printf (" * az_poller: ");

    //  @selftest
    //  Simple create/destroy test

    // Note: If your selftest reads SCMed fixture data, please keep it in
    // src/selftest-ro; if your test creates filesystem objects, please
    // do so under src/selftest-rw. They are defined below along with a
    // usecase (asert) to make compilers happy.
    const char *SELFTEST_DIR_RO = "src/selftest-ro";
    const char *SELFTEST_DIR_RW = "src/selftest-rw";
    assert (SELFTEST_DIR_RO);
    assert (SELFTEST_DIR_RW);
    // Uncomment these to use C++ strings in C++ selftest code:
    //std::string str_SELFTEST_DIR_RO = std::string(SELFTEST_DIR_RO);
    //std::string str_SELFTEST_DIR_RW = std::string(SELFTEST_DIR_RW);
    //assert ( (str_SELFTEST_DIR_RO != "") );
    //assert ( (str_SELFTEST_DIR_RW != "") );
    // NOTE that for "char*" context you need (str_SELFTEST_DIR_RO + "/myfilename").c_str()

    az_poller_t *self = az_poller_new (msg::Connection ());
    assert (self);
    az_poller_destroy (&self);
    //  @end
    printf ("OK\n");
}
