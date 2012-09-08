/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef __IMALSE_H__
#define __IMALSE_H__

#include <stdint.h>
// #include "src/network/buffer.h"
#include "ns3/header.h"
#include "ns3/buffer.h"

namespace ns3 {

/* ... */

class ImalseHeader : public Header
{
    public:
        void SetData (std::string data);
        std::string GetData (void);
        static TypeId GetTypeId (void);
        virtual TypeId GetInstanceTypeId (void) const;
        virtual uint32_t GetSerializedSize (void) const;
        virtual void Serialize (Buffer::Iterator start) const;
        virtual uint32_t Deserialize (Buffer::Iterator start);
        virtual void Print (std::ostream &os) const;
    // private:
        std::string m_data;
};


}

#endif /* __IMALSE_H__ */

