
#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <nng/nng.h>
#include <nng/supplemental/util/platform.h>
#include <nng/mqtt/mqtt_quic.h>
#include <nng/mqtt/mqtt_client.h>
#include "msquic.h"
#include "callback.h"

#include <string>
#include <vector>
using namespace std;


class QuicClient
{

private:
    nng_socket sock;
    Callbacks *cbs;

public:
    // constructor
    QuicClient(const string& url);

    // destructor
    ~QuicClient();

    int set_callbacks(Callbacks *cbs);
    int connect(uint16_t keepalive, bool clean_session);
    int subscribe(const string& topic, uint8_t QoS);
    int publish(const string& topic, uint8_t *payload, uint32_t size, uint8_t QoS, bool dup, bool retain);

    int publish(const string& topic, const string& payload, uint8_t QoS, bool dup, bool retain);

    static int on_connected(void *rmsg, void *arg)
    {
        QuicClient *cli = static_cast<QuicClient *>(arg);
        cli->cbs->client_connected(rmsg, arg);
        return 0;
    }
    static int on_disconnected(void *rmsg, void *arg)
    {
        QuicClient *cli = static_cast<QuicClient *>(arg);
        cli->cbs->client_disconnected(rmsg, arg);
        return 0;
    }
    static int on_message_received(void *rmsg, void *arg)
    {
        QuicClient *cli = static_cast<QuicClient *>(arg);
        cli->cbs->message_received(rmsg, arg);
        return 0;
    }
    static int on_message_send(void *rmsg, void *arg)
    {
        QuicClient *cli = static_cast<QuicClient *>(arg);
        cli->cbs->message_send(rmsg, arg);
        return 0;
    }

};

#endif