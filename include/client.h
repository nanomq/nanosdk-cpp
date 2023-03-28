
#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <nng/nng.h>
#include <nng/supplemental/util/platform.h>
#include <nng/mqtt/mqtt_quic.h>
#include <nng/mqtt/mqtt_client.h>
#include "msquic.h"
#include "callback.h"

#include <iostream>

using namespace std;

#define CLIENT_SEND_Q_SZ 4

class QuicClient
{

private:
    nng_socket sock;
    Callbacks *cbs;

public:
    // constructor
    QuicClient(const char *url)
    {
        int rv;
        if ((rv = nng_mqtt_quic_client_open(&this->sock, url)) != 0)
        {
            cerr << "Error in Quic client open. Return code: " << rv << endl;
        }
    }

    // destructor
    ~QuicClient()
    {
        nng_close(this->sock);
    }

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

    int set_callbacks(Callbacks *cbs)
    {
        this->cbs = std::move(cbs);
        if (0 != nng_mqtt_quic_set_connect_cb(&this->sock, on_connected, (void *)this) 
        || 0 != nng_mqtt_quic_set_disconnect_cb(&this->sock, on_disconnected, (void *)this) 
        || 0 != nng_mqtt_quic_set_msg_recv_cb(&this->sock, on_message_received, (void *)this) 
        || 0 != nng_mqtt_quic_set_msg_send_cb(&this->sock, on_message_send, (void *)this))
        {
            cerr << "Quic client cb set error.\n";
            return -1;
        }
        return 0;
    }

    int connect(uint16_t keepalive, bool clean_session)
    {
        nng_msg *msg;
        int rc = 0;
        nng_mqtt_msg_alloc(&msg, 0);
        nng_mqtt_msg_set_packet_type(msg, NNG_MQTT_CONNECT);
        nng_mqtt_msg_set_connect_keep_alive(msg, keepalive);
        nng_mqtt_msg_set_connect_clean_session(msg, clean_session);

        if ((rc = nng_sendmsg(this->sock, msg, NNG_FLAG_ALLOC) != 0))
        {
            cerr << "MQTT_NNG publish error: " << nng_strerror(rc) << endl;
        }
        return rc;
    }

    int subscribe(char *topic, uint8_t QoS)
    {
        nng_msg *msg;
        int rc = 0;
        nng_mqtt_msg_alloc(&msg, 0);
        nng_mqtt_msg_set_packet_type(msg, NNG_MQTT_SUBSCRIBE);

        int count = 1;

        nng_mqtt_topic_qos subscriptions[] = {
            {.topic = {
                 .length = strlen(topic),
                 .buf = (uint8_t *)topic},
             .qos = QoS}};

        nng_mqtt_msg_set_subscribe_topics(msg, subscriptions, count);
        rc = nng_sendmsg(this->sock, msg, NNG_FLAG_ALLOC);
        if (rc != 0)
        {
            cerr << "MQTT_NNG subscribe error: " << nng_strerror(rc) << endl;
        }
        return rc;
    }

    int publish(char *topic, char *payload, uint8_t QoS, bool dup, bool retain)
    {
        nng_msg *msg;
        int rc = 0;
        nng_mqtt_msg_alloc(&msg, 0);
        nng_mqtt_msg_set_packet_type(msg, NNG_MQTT_PUBLISH);
        nng_mqtt_msg_set_publish_dup(msg, dup);
        nng_mqtt_msg_set_publish_qos(msg, QoS);
        nng_mqtt_msg_set_publish_retain(msg, retain);
        nng_mqtt_msg_set_publish_topic(msg, topic);
        nng_mqtt_msg_set_publish_payload(
            msg, (uint8_t *)payload, strlen(payload));
        rc = nng_sendmsg(this->sock, msg, NNG_FLAG_ALLOC);
        if (rc != 0)
        {
            cerr << "MQTT_NNG publish error: " << nng_strerror(rc) << endl;
        }
        return rc;
    }
};

#endif