#include <main/php.h>
#include <azure_c_shared_utility/tickcounter.h>
#include <azure_c_shared_utility/tlsio.h>
#include <azure_c_shared_utility/platform.h>
#include <azure_uamqp_c/connection.h>
#include <azure_uamqp_c/saslclientio.h>
#include <azure_uamqp_c/sasl_mssbcbs.h>
#include <azure_uamqp_c/sasl_plain.h>
#include <azure_uamqp_c/session.h>
#include "uamqp_connection.h"

struct uamqp_connection create_uamqp_connection(char *host, int port, char *policyName, char *policyKey)
{
    struct uamqp_connection connection;

    const IO_INTERFACE_DESCRIPTION* tlsio_interface;
    TLSIO_CONFIG tls_io_config = { host, port };
    SASLCLIENTIO_CONFIG sasl_io_config;
    SASL_PLAIN_CONFIG sasl_plain_config = {policyName, policyKey, NULL };
    TICK_COUNTER_HANDLE tickCounterHandle;

    if (platform_init() != 0) {
        // not sure what to do when
    }

    // Create TLS IO
    tlsio_interface = platform_get_default_tlsio();
    connection.tls_io = xio_create(tlsio_interface, &tls_io_config);

    // Create SASL IO
    connection.sasl_mechanism_handle  = saslmechanism_create(saslplain_get_interface(), &sasl_plain_config);
    sasl_io_config.underlying_io = connection.tls_io;
    sasl_io_config.sasl_mechanism = connection.sasl_mechanism_handle;
    connection.sasl_io = xio_create(saslclientio_get_interface_description(), &sasl_io_config);

    // create connection
    tickCounterHandle = tickcounter_create();
    tickcounter_destroy(tickCounterHandle);

    connection.connection = connection_create(connection.sasl_io, host, "php-uaqmp-binding", NULL, NULL);
    connection_set_trace(connection.connection, 1);

//    connection_open(connection.connection);

    return connection;
}

struct uamqp_session create_uamqp_session(struct uamqp_connection connection)
{
    struct uamqp_session session;

    session.session =  session_create(connection.connection, NULL, NULL);
    session_set_incoming_window(session.session, 2147483647);
    session_set_outgoing_window(session.session, 65536);

    return session;
}

void destroy_connection(struct uamqp_connection *uamqp_connection, struct uamqp_session *uamqp_session)
{
    session_destroy(uamqp_session->session);
    connection_destroy(uamqp_connection->connection);
    xio_destroy(uamqp_connection->sasl_io);
    xio_destroy(uamqp_connection->tls_io);
    saslmechanism_destroy(uamqp_connection->sasl_mechanism_handle);

    uamqp_connection->connection = NULL;
    uamqp_connection->tls_io = NULL;
    uamqp_connection->sasl_io = NULL;
    uamqp_connection->sasl_mechanism_handle = NULL;
    uamqp_session->session = NULL;

    platform_deinit();
}

