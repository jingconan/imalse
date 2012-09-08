/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include <stdint.h>
#include "ns3/packet.h"
#include <iostream>
#include "ns3/imalse.h"

NS_LOG_COMPONENT_DEFINE ("imalse-run");

using namespace ns3;
using std::cout;
using std::endl;

    int 
main (int argc, char *argv[])
{
    NS_LOG_UNCOND ("Scratch Simulator");
    uint32_t size = 1024;
    const uint8_t *buffer = reinterpret_cast<const uint8_t*>("good");
    // Ptr<Packet> p  = Create<Packet> (size);
    Ptr<Packet> p  = Create<Packet> (buffer, size);
    ImalseHeader header;
    header.SetData("imalse header");
    p->AddHeader(header);

    ImalseHeader header2;
    p->RemoveHeader (header2);
    cout << "This is a demo of imalse packet manipulation!" << endl
        << "Packet: " << p << endl
        << "Header content: " << header2.GetData() << endl;

}
