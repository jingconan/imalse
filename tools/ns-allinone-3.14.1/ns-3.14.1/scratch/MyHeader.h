#include <stdint.h>
#include "buffer.h"
#include "header.h"
#include "trailer.h"
class MyHeader : public Header
{
    public:
        // new methods
        void SetData (uint16_t data);
        uint16_t GetData (void);
        // new method needed
        static TypeId GetTypeId (void);
        // overridden from Header
        virtual uint32_t GetSerializedSize (void) const;
        virtual void Serialize (Buffer::Iterator start) const;
        virtual uint32_t Deserialize (Buffer::Iterator start);
        virtual void Print (std::ostream &os) const;
    private:
        uint16_t m_data;
};
