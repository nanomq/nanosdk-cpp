#ifndef MQTT_PROPERTY_H
#define MQTT_PROPERTY_H

#include <nng/mqtt/mqtt_client.h>
#include <string>
#include <vector>

class Property
{
private:
    property *m_p;

public:
    Property();
    ~Property();

    Property &append_u8(uint8_t prop_id, uint8_t value);
    Property &append_u16(uint8_t prop_id, uint16_t value);
    Property &append_u32(uint8_t prop_id, uint32_t value);
    Property &append_varint(uint8_t prop_id, uint32_t value);
    Property &append_binary(uint8_t prop_id, uint8_t *value, uint32_t len);
    Property &append_str(uint8_t prop_id, const std::string &value);
    Property &append_strpair(uint8_t prop_id, const std::string &key, const std::string &value);
    property *get_property() const;
};

#endif