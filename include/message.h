#ifndef MQTT_MESSAGE_H
#define MQTT_MESSAGE_H

#include <nng/nng.h>
#include <string>
#include <vector>

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
    ConnMessage(void);
    ~ConnMessage();

    // Set
    ConnMessage &keepalive(uint16_t keepalive);
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

    // Get
    uint16_t keepalive() const;
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
#endif // MQTT_MESSAGE_H