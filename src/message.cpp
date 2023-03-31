#include "message.h"
#include <nng/supplemental/util/platform.h>
#include <nng/mqtt/mqtt_quic.h>
#include <nng/mqtt/mqtt_client.h>

nng_msg *Message::get_message() const
{
    return msg;
}

ConnMessage::ConnMessage()
{
    nng_mqtt_msg_alloc(&msg, 0);
    nng_mqtt_msg_set_packet_type(Message::msg, NNG_MQTT_CONNECT);
}

ConnMessage::~ConnMessage()
{
    nng_msg_free(msg);
}

ConnMessage &ConnMessage::keepalive(uint16_t keepalive)
{
    nng_mqtt_msg_set_connect_keep_alive(msg, keepalive);
    return *this;
}

ConnMessage &ConnMessage::proto_version(uint8_t version)
{
    nng_mqtt_msg_set_connect_proto_version(msg, version);
    return *this;
}
ConnMessage &ConnMessage::client_id(const std::string &clientid)
{
    nng_mqtt_msg_set_connect_client_id(msg, clientid.c_str());
    m_clientid = std::move(clientid);
    return *this;
}
ConnMessage &ConnMessage::user_name(const std::string &username)
{
    nng_mqtt_msg_set_connect_user_name(msg, username.c_str());
    m_username = std::move(username);
    return *this;
}
ConnMessage &ConnMessage::password(const std::string &password)
{
    nng_mqtt_msg_set_connect_password(msg, password.c_str());
    m_password = std::move(password);
    return *this;
}
ConnMessage &ConnMessage::clean_session(bool clean_session)
{
    nng_mqtt_msg_set_connect_clean_session(msg, clean_session);
    return *this;
}
ConnMessage &ConnMessage::will_topic(const std::string &will_topic)
{
    nng_mqtt_msg_set_connect_will_topic(msg, will_topic.c_str());
    m_will_topic = std::move(will_topic);
    return *this;
}
ConnMessage &ConnMessage::will_msg(uint8_t *will_msg, uint32_t len)
{
    nng_mqtt_msg_set_connect_will_msg(msg, will_msg, len);
    return *this;
}
ConnMessage &ConnMessage::will_retain(bool retain)
{
    nng_mqtt_msg_set_connect_will_retain(msg, retain);
    return *this;
}
ConnMessage &ConnMessage::will_qos(uint8_t qos)
{
    nng_mqtt_msg_set_connect_will_qos(msg, qos);
    return *this;
}

// Get
uint16_t ConnMessage::keepalive() const
{
    return nng_mqtt_msg_get_connect_keep_alive(msg);
}

const std::string &ConnMessage::user_name() const
{
    return m_username;
}

const std::string &ConnMessage::password() const
{
    return m_password;
}

const std::string &ConnMessage::client_id() const
{
    return m_clientid;
}

const std::string &ConnMessage::will_topic() const
{
    return m_will_topic;
}

bool ConnMessage::clean_session() const
{
    return nng_mqtt_msg_get_connect_clean_session(msg);
}

uint8_t ConnMessage::proto_version() const
{
    return nng_mqtt_msg_get_connect_proto_version(msg);
}

uint16_t ConnMessage::keep_alive() const
{
    return nng_mqtt_msg_get_connect_keep_alive(msg);
}

bool ConnMessage::will_retain() const
{
    return nng_mqtt_msg_get_connect_will_retain(msg);
}

uint8_t ConnMessage::will_qos() const
{
    return nng_mqtt_msg_get_connect_will_qos(msg);
}

uint8_t *ConnMessage::will_msg(uint32_t *len) const
{
    return nng_mqtt_msg_get_connect_will_msg(msg, len);
}