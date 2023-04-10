#include "client.h"
#include <iostream>

QuicClient::QuicClient(const string &url, uint8_t version)
{

    int rv;
    switch (version)
    {
    case MQTT_PROTOCOL_VERSION_v311:
    case MQTT_PROTOCOL_VERSION_v31:
        if ((rv = nng_mqtt_quic_client_open(&this->sock, url.c_str())) != 0)
        {
            cerr << "Error in Quic client open. Return code: " << rv << endl;
        }
        break;
    case MQTT_PROTOCOL_VERSION_v5:
        if ((rv = nng_mqttv5_quic_client_open(&this->sock, url.c_str())) != 0)
        {
            cerr << "Error in Quic client open. Return code: " << rv << endl;
        }
        break;
    default:
        cerr << "Error client version." << endl;
        break;
    }
    ver = version;
}

QuicClient::~QuicClient()
{
    nng_close(this->sock);
}

int QuicClient::set_callbacks(Callbacks *cbs)
{
    this->cbs = std::move(cbs);
    if (0 != nng_mqtt_quic_set_connect_cb(&this->sock, on_connected, (void *)this) || 0 != nng_mqtt_quic_set_disconnect_cb(&this->sock, on_disconnected, (void *)this) || 0 != nng_mqtt_quic_set_msg_recv_cb(&this->sock, on_message_received, (void *)this) || 0 != nng_mqtt_quic_set_msg_send_cb(&this->sock, on_message_send, (void *)this))
    {
        cerr << "Quic client cb set error.\n";
        return -1;
    }
    return 0;
}

int QuicClient::connect(uint16_t keepalive, bool clean_session)
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

int QuicClient::connect(const ConnMessage &cmsg)
{
    int rc = 0;
    nng_msg *msg = cmsg.get_message();
    nng_mqtt_msg_set_connect_proto_version(msg, ver);
    if ((rc = nng_sendmsg(this->sock, cmsg.get_message(), NNG_FLAG_ALLOC) != 0))
    {
        cerr << "MQTT_NNG publish error: " << nng_strerror(rc) << endl;
    }
    return rc;
}

int QuicClient::subscribe(const string &topic, uint8_t QoS)
{
    nng_msg *msg;
    int rc = 0;
    nng_mqtt_msg_alloc(&msg, 0);
    nng_mqtt_msg_set_packet_type(msg, NNG_MQTT_SUBSCRIBE);

    int count = 1;

    nng_mqtt_topic_qos subscriptions[] = {
        {.topic = {
             .length = (uint32_t)topic.length(),
             .buf = (uint8_t *)topic.c_str()},
         .qos = QoS}};

    nng_mqtt_msg_set_subscribe_topics(msg, subscriptions, count);
    rc = nng_sendmsg(this->sock, msg, NNG_FLAG_ALLOC);
    if (rc != 0)
    {
        cerr << "MQTT_NNG subscribe error: " << nng_strerror(rc) << endl;
    }
    return rc;
}

int QuicClient::subscribe(const SubMessage &smsg)
{
    int rc = nng_sendmsg(this->sock, smsg.get_message(), NNG_FLAG_ALLOC);
    if (rc != 0)
    {
        cerr << "MQTT_NNG subscribe error: " << nng_strerror(rc) << endl;
    }
    return rc;
}

int QuicClient::publish(const string &topic, uint8_t *payload, uint32_t size, uint8_t qos, bool dup, bool retain)
{
    nng_msg *msg;
    int rc = 0;
    nng_mqtt_msg_alloc(&msg, 0);
    nng_mqtt_msg_set_packet_type(msg, NNG_MQTT_PUBLISH);
    nng_mqtt_msg_set_publish_dup(msg, dup);
    nng_mqtt_msg_set_publish_qos(msg, qos);
    nng_mqtt_msg_set_publish_retain(msg, retain);
    nng_mqtt_msg_set_publish_topic(msg, topic.c_str());
    nng_mqtt_msg_set_publish_payload(
        msg, payload, size);
    rc = nng_sendmsg(this->sock, msg, NNG_FLAG_ALLOC);
    if (rc != 0)
    {
        cerr << "MQTT_NNG publish error: " << nng_strerror(rc) << endl;
    }
    return rc;
}

int QuicClient::publish(const PubMessage &pmsg)
{
    int rc = nng_sendmsg(this->sock, pmsg.get_message(), NNG_FLAG_ALLOC);
    if (rc != 0)
    {
        cerr << "MQTT_NNG publish error: " << nng_strerror(rc) << endl;
    }
    return rc;
}

int QuicClient::publish(const string &topic, const string &payload, uint8_t qos, bool dup, bool retain)
{
    return publish(topic, (uint8_t *)payload.c_str(), payload.size(), qos, dup, retain);
}