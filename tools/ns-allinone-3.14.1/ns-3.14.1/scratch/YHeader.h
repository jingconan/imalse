#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include "buffer.h"
#include "header.h"

class YHeader : public Header
{
public:
    static TypeId GetTypeId (void);
    virtual TypeId GetInstanceTypeId (void) const;
    virtual uint32_t GetSerializedSize (void) const;
    virtual void Serialize (Buffer::Iterator start) const;
    virtual uint32_t Deserialize (Buffer::Iterator start);
    virtual void Print(std::ostream &os) const;
    void SetData (uint32_t data);
    uint32_t GetData (void) const;

private:
    uint32_t m_data;

}

#endif
