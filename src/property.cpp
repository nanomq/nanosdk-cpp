#include "property.h"

Property::Property()
{
    m_p = mqtt_property_alloc();
}

Property::~Property()
{
    // mqtt_property_free(m_p);
}

Property &Property::append_u8(uint8_t prop_id, uint8_t value)
{
    property *p = mqtt_property_set_value_u8(prop_id, value);
    mqtt_property_append(m_p, p);
    return *this;
}

Property &Property::append_u16(uint8_t prop_id, uint16_t value)
{
    property *p = mqtt_property_set_value_u16(prop_id, value);
    mqtt_property_append(m_p, p);
    return *this;
}

Property &Property::append_u32(uint8_t prop_id, uint32_t value)
{
    property *p = mqtt_property_set_value_u32(prop_id, value);
    mqtt_property_append(m_p, p);
    return *this;
}

Property &Property::append_varint(uint8_t prop_id, uint32_t value)
{
    property *p = mqtt_property_set_value_varint(prop_id, value);
    mqtt_property_append(m_p, p);
    return *this;
}

Property &Property::append_binary(uint8_t prop_id, uint8_t *value, uint32_t len)
{
    property *p = mqtt_property_set_value_binary(prop_id, value, len, true);
    mqtt_property_append(m_p, p);
    return *this;
}

Property &Property::append_str(uint8_t prop_id, const std::string &value)
{
    property *p = mqtt_property_set_value_str(prop_id, value.c_str(), value.length(), true);
    mqtt_property_append(m_p, p);
    return *this;
}

Property &Property::append_strpair(uint8_t prop_id, const std::string &key, const std::string &value)
{
    property *p = mqtt_property_set_value_strpair(prop_id, key.c_str(), key.length(), value.c_str(), value.length(), true);
    mqtt_property_append(m_p, p);
    return *this;
}

property *Property::get_property() const
{
    return m_p;
}