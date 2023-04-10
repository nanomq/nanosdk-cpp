#ifndef MQTT_MESSAGE_H
#define MQTT_MESSAGE_H

#include "property.h"
#include <nng/nng.h>
#include <string>
#include <vector>
#include <tuple>

class Message
{
protected:
    nng_msg *msg;

public:
    nng_msg *get_message() const;
};

class ConnMessage : public Message
{
private:
    std::string m_clientid;
    std::string m_username;
    std::string m_password;
    std::string m_will_topic;

public:
    ConnMessage();
    ~ConnMessage();

    // Set
    ConnMessage &proto_version(uint8_t);
    ConnMessage &keep_alive(uint16_t);
    ConnMessage &client_id(const std::string &);
    ConnMessage &user_name(const std::string &);
    ConnMessage &password(const std::string &);
    ConnMessage &clean_session(bool);
    ConnMessage &will_topic(const std::string &);
    ConnMessage &will_msg(uint8_t *, uint32_t);
    ConnMessage &will_retain(bool);
    ConnMessage &will_qos(uint8_t);
    ConnMessage &property(const Property &);

    // Get
    const std::string &user_name() const;
    const std::string &password() const;
    const std::string &client_id() const;
    const std::string &will_topic() const;
    bool clean_session() const;
    uint8_t proto_version() const;
    uint16_t keep_alive() const;
    uint8_t *will_msg(uint32_t *) const;
    bool will_retain() const;
    uint8_t will_qos() const;
};

class SubMessage : public Message
{
public:
    using topic_qos = std::tuple<const std::string, int>;
    using topics = std::vector<topic_qos>;
    SubMessage();
    ~SubMessage();

    // set
    SubMessage &topic_with_qos(const topics &);
    SubMessage &topic_with_qos(const std::string &, int);
    SubMessage &property(const Property &);
    // get
    const topics &topic_with_qos();

private:
    /* data */
    topics vts;
};

class PubMessage : public Message
{
private:
    std::string m_topic;

public:
    PubMessage();
    ~PubMessage();

    // set
    PubMessage &qos(uint8_t);
    PubMessage &retain(bool);
    PubMessage &dup(bool);
    PubMessage &topic(const std::string &);
    PubMessage &payload(uint8_t *, uint32_t);
    PubMessage &property(const Property &);

    // get
    uint8_t qos() const;
    bool retain() const;
    bool dup() const;
    const std::string &topic() const;
    const uint8_t *payload(uint32_t *) const;
};

#endif // MQTT_MESSAGE_H