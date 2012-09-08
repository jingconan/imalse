/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "imalse.h"

#include <stdint.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <string.h>

#define CMD_MAX_SIZE 100

namespace ns3 {

/* ... */
void
ImalseHeader::SetData (std::string data)
{
  m_data = data;
}

std::string
ImalseHeader::GetData (void)
{
  return m_data;
}

uint32_t
ImalseHeader::GetSerializedSize (void) const
{
  return m_data.size() + 4; // be carefule, we use 4 here because we use uint32_t to represent size
}

void
ImalseHeader::Serialize (Buffer::Iterator start) const
{
  start.WriteU32( (uint32_t) m_data.size());
  start.Write ( reinterpret_cast<uint8_t const *> (m_data.c_str()), m_data.size() );
  // for (std::string::const_iterator it = m_data.begin(); it != m_data.end(); it++)
  // {
  //     start.WriteU8( (uint8_t) *it );
  // }

}

uint32_t
ImalseHeader::Deserialize (Buffer::Iterator start)
{
  uint32_t m_data_size = start.ReadU32 (); // size of the string. 

  // uint8_t buffer[CMD_MAX_SIZE];
  // memset (buffer, 0, CMD_MAX_SIZE);
  // uint8_t tmp;
  // for (int i = 0; i < (int) m_data_size; i++) 
  // {
  //     tmp =  start.ReadU8();
  //     std::cout << (char) tmp << "-";
  //     memcpy(buffer+i, &tmp, 1);
  // }
  // std::cout << std::endl;
  // m_data =  std::string( (char *) buffer );

  // uint8_t buffer[CMD_MAX_SIZE];
  // memset (buffer, 0, CMD_MAX_SIZE);
  // start.Read (&buffer[0], m_data_size);
  // m_data =  std::string( (char *) buffer );

  std::vector<char> buffer(m_data_size);
  start.Read( reinterpret_cast< uint8_t * > (&buffer[0]), m_data_size);
  std::string tmp(buffer.begin(), buffer.end());
  m_data = tmp;

  // return m_data_size + 1;
  return (uint32_t) (m_data_size + 4);
}
void
ImalseHeader::Print (std::ostream &os) const
{
  os << m_data;
}


TypeId
ImalseHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ImalseHeader")
    .SetParent<Header> ()
    ;
  return tid;
}

TypeId
ImalseHeader::GetInstanceTypeId (void) const
{
    return GetTypeId();
}

}

