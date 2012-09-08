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
 *
 * Author: John Abraham <john.abraham@gatech.edu>
 */


#include "animpacket.h"
#include "debug/xdebug.h"

#include <QFont>
#include <QGraphicsProxyWidget>

namespace netanim {

static uint32_t gAnimId = 1;

QFont AnimPacket::gFont;

AnimPacket::AnimPacket(uint32_t fromId, uint32_t toId, double fbTx, 
			double lbTx, double fbRx, 
			double lbRx, bool isWPacket)
    : m_fromId(fromId),
      m_toId(toId),
      m_fbTx(fbTx),
      m_animId(gAnimId++),
      m_isWpacket(isWPacket),
      m_selected(true)

{
    if(((lbTx - fbTx) < 0.00001) || (isWPacket))
    {
        m_fbRx = 0;
        m_lbRx = 0;
        m_lbTx = 0;
        m_isWpacket = true;
    }
    else
    {
        m_fbRx = new double(fbRx);
        m_lbRx = new double(lbRx);
        m_lbTx = new double(lbTx);

    }
    m_graphicsItem = new QGraphicsLineItem;

    m_graphicsTextItem = new QGraphicsTextItem;
    m_graphicsTextItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);

    //qDebug(m_animId);
    clearInfo();
}


AnimPacket::~AnimPacket()
{
    if(m_fbRx)
        delete m_fbRx;
    if(m_lbRx)
        delete m_lbRx;
    if(m_lbTx)
        delete m_lbTx;
    if(m_graphicsItem)
        delete m_graphicsItem;
    if(m_graphicsTextItem)
        delete m_graphicsTextItem;
    if(m_arpInfo)
        delete m_arpInfo;
    if(m_wifiMacInfo)
        delete m_wifiMacInfo;
    if(m_ethernetInfo)
        delete m_ethernetInfo;
    if(m_pppInfo)
        delete m_pppInfo;
    if(m_ipv4Info)
        delete m_ipv4Info;
    if(m_udpInfo)
        delete m_udpInfo;
    if(m_tcpInfo)
        delete m_tcpInfo;
    if(m_icmpInfo)
        delete m_icmpInfo;
    if(m_aodvInfo)
        delete m_aodvInfo;
    if(m_dsdvInfo)
        delete m_dsdvInfo;
    if(m_olsrInfo)
        delete m_olsrInfo;

}


void
AnimPacket::clearInfo()
{
    m_arpInfo = 0;
    m_pppInfo = 0;
    m_ethernetInfo = 0;
    m_wifiMacInfo = 0;
    m_ipv4Info = 0;
    m_icmpInfo = 0;
    m_udpInfo = 0;
    m_tcpInfo = 0;
    m_aodvInfo = 0;
    m_dsdvInfo = 0;
    m_olsrInfo = 0;
}

void
AnimPacket::parsePpp(std::string metaInfo)
{
    QRegExp rx("ns3::PppHeader.*");
    int pos = 0;
    if((pos = rx.indexIn(metaInfo.c_str())) == -1)
        return;
    m_pppInfo = new PppInfo;

}


void
AnimPacket::parseArp(std::string metaInfo)
{
    QRegExp rx("ns3::ArpHeader\\s+\\((request|reply) source mac: ..-..-(..:..:..:..:..:..) source ipv4: (\\S+) (?:dest mac: ..-..-)?(..:..:..:..:..:.. )?dest ipv4: (\\S+)\\)");
    int pos = 0;
    if ((pos = rx.indexIn(metaInfo.c_str())) == -1)
        return;
    ArpInfo *arpInfo = new ArpInfo;
    arpInfo->type = rx.cap(1).toAscii().data();
    arpInfo->sourceMac = rx.cap(2).toAscii().data();
    arpInfo->sourceIpv4 = rx.cap(3).toAscii().data();
    if( QString(rx.cap(4).toAscii().data()) != "")
    arpInfo->destMac = rx.cap(4).toAscii().data();
    arpInfo->destIpv4  = rx.cap(5).toAscii().data();
    m_arpInfo = arpInfo;

    /*qDebug (" Type:" + arpInfo->type +
            " SMac:" + arpInfo->sourceMac +
            " SIp:"  + arpInfo->sourceIpv4 +
            " DMac:" + arpInfo->destMac +
            " DIp:"  + arpInfo->destIpv4);*/


}

void
AnimPacket::parseEthernet(std::string metaInfo)
{
    QRegExp rx("ns3::EthernetHeader \\( length/type\\S+ source=(..:..:..:..:..:..), destination=(..:..:..:..:..:..)\\)");
    int pos = 0;
    if ((pos = rx.indexIn(metaInfo.c_str())) == -1)
        return;
    EthernetInfo * ethernetInfo = new EthernetInfo;
    ethernetInfo->sourceMac = rx.cap(1).toAscii().data();
    ethernetInfo->destMac = rx.cap(2).toAscii().data();
    m_ethernetInfo = ethernetInfo;
}


void
AnimPacket::parseIcmp(std::string metaInfo)
{
    QRegExp rx("ns3::Icmpv4Header \\(type=(.*), code=([^\\)]*)");
    int pos = 0;
    if ((pos = rx.indexIn(metaInfo.c_str())) == -1)
        return;
    m_icmpInfo = new IcmpInfo;
    m_icmpInfo->type = rx.cap(1).toAscii().data();
    m_icmpInfo->code = rx.cap(2).toAscii().data();

}

void
AnimPacket::parseUdp(std::string metaInfo)
{

    QRegExp rx("ns3::UdpHeader \\(length: (\\S+) (\\S+) > (\\S+)\\)");
    int pos = 0;
    if ((pos = rx.indexIn(metaInfo.c_str())) == -1)
        return;
    m_udpInfo = new UdpInfo;
    m_udpInfo->length = rx.cap(1).toAscii().data();
    m_udpInfo->SPort = rx.cap(2).toAscii().data();
    m_udpInfo->DPort = rx.cap(3).toAscii().data();
}

void
AnimPacket::parseIpv4(std::string metaInfo)
{
    QRegExp rx("ns3::Ipv4Header \\(tos (\\S+) DSCP (\\S+) ECN (\\S+) ttl (\\d+) id (\\d+) protocol (\\d+) .* length: (\\d+) (\\S+) > (\\S+)\\)");
    int pos = 0;
    if ((pos = rx.indexIn(metaInfo.c_str())) == -1)
        return;
    m_ipv4Info = new Ipv4Info;
    m_ipv4Info->tos = rx.cap(1).toAscii().data();
    m_ipv4Info->Dscp = rx.cap(2).toAscii().data();
    m_ipv4Info->Ecn = rx.cap(3).toAscii().data();
    m_ipv4Info->Ttl = rx.cap(4).toAscii().data();
    m_ipv4Info->Id = rx.cap(5).toAscii().data();
    m_ipv4Info->protocol = rx.cap(6).toAscii().data();
    m_ipv4Info->length = rx.cap(7).toAscii().data();
    m_ipv4Info->SrcIp = rx.cap(8).toAscii().data();
    m_ipv4Info->DstIp = rx.cap(9).toAscii().data();
}

void
AnimPacket::parseTcp(std::string metaInfo)
{
    QRegExp rx("ns3::TcpHeader \\((\\S+) > (\\S+) \\[([^\\]]*)\\] Seq=(\\S+) Ack=(\\S+) Win=(\\S+)\\)");
    int pos = 0;
    if ((pos = rx.indexIn(metaInfo.c_str())) == -1)
        return;
    m_tcpInfo = new TcpInfo;
    m_tcpInfo->SPort = rx.cap(1).toAscii().data();
    m_tcpInfo->DPort = rx.cap(2).toAscii().data();
    m_tcpInfo->flags = rx.cap(3).toAscii().data();
    m_tcpInfo->seq = rx.cap(4).toAscii().data();
    m_tcpInfo->ack = rx.cap(5).toAscii().data();
    m_tcpInfo->window = rx.cap(6).toAscii().data();
}

void
AnimPacket::parseWifi(std::string metaInfo)
{
    QRegExp rxCTL_ACK("ns3::WifiMacHeader \\(CTL_ACK .*RA=(..:..:..:..:..:..)");
    int pos = 0;
    if ((pos = rxCTL_ACK.indexIn(metaInfo.c_str())) != -1)
    {
        m_wifiMacInfo = new WifiMacInfo;
        m_wifiMacInfo->type = "CTL_ACK";
        m_wifiMacInfo->Ra = rxCTL_ACK.cap(1).toAscii().data();
       return;

    }

    QRegExp rx("ns3::WifiMacHeader \\((\\S+) ToDS=(0|1), FromDS=(0|1), .*DA=(..:..:..:..:..:..), SA=(..:..:..:..:..:..), BSSID=(..:..:..:..:..:..)");
    pos = 0;
    if ((pos = rx.indexIn(metaInfo.c_str())) == -1)
        return;
    m_wifiMacInfo = new WifiMacInfo;
    m_wifiMacInfo->type = rx.cap(1).toAscii().data();
    m_wifiMacInfo->toDs = rx.cap(2).toAscii().data();
    m_wifiMacInfo->fromDs = rx.cap(3).toAscii().data();
    m_wifiMacInfo->Da = rx.cap(4).toAscii().data();
    m_wifiMacInfo->Sa = rx.cap(5).toAscii().data();
    m_wifiMacInfo->Bssid = rx.cap(6).toAscii().data();

    if(m_wifiMacInfo->type == "MGT_ASSOCIATION_REQUEST")
    {
        QRegExp rx("ns3::MgtAssocRequestHeader \\(ssid=(\\S+),");
        int pos = 0;
        if ((pos = rx.indexIn(metaInfo.c_str())) == -1)
            return;
        m_wifiMacInfo->SSid = rx.cap(1).toAscii().data();
    }
    if(m_wifiMacInfo->type == "MGT_ASSOCIATION_RESPONSE")
    {
        QRegExp rx("ns3::MgtAssocResponseHeader \\(status code=(\\S+), rates");
        int pos = 0;
        if ((pos = rx.indexIn(metaInfo.c_str())) == -1)
            return;
        m_wifiMacInfo->assocResponseStatus = rx.cap(1).toAscii().data();
    }


}

void
AnimPacket::parseAodv(std::string metaInfo)
{
    QRegExp rx("ns3::aodv::TypeHeader \\((\\S+)\\) ");
    int pos = 0;
    if ((pos = rx.indexIn(metaInfo.c_str())) == -1)
        return;
    m_aodvInfo = new AodvInfo;
    m_aodvInfo->type = rx.cap(1).toAscii().data();
    if(m_aodvInfo->type == "RREQ")
    {
        QRegExp rx("ns3::aodv::RreqHeader \\(RREQ ID \\d+ destination: ipv4 (\\S+) sequence number (\\d+) source: ipv4 (\\S+) sequence number \\d+");
        int pos = 0;
        if ((pos = rx.indexIn(metaInfo.c_str())) == -1)
            return;
        m_aodvInfo->destination = rx.cap(1).toAscii().data();
        m_aodvInfo->seq = rx.cap(2).toAscii().data();
        m_aodvInfo->source = rx.cap(1).toAscii().data();

    }
    if(m_aodvInfo->type == "RREP")
    {
        QRegExp rx("ns3::aodv::RrepHeader \\(destination: ipv4 (\\S+) sequence number (\\d+) source ipv4 (\\S+) ");
        int pos = 0;
        if ((pos = rx.indexIn(metaInfo.c_str())) == -1)
            return;
        m_aodvInfo->destination = rx.cap(1).toAscii().data();
        m_aodvInfo->seq = rx.cap(2).toAscii().data();
        m_aodvInfo->source = rx.cap(1).toAscii().data();
    }
    if(m_aodvInfo->type == "RERR")
    {
        QRegExp rx("ns3::aodv::RerrHeader \\(([^\\)]+) \\(ipv4 address, seq. number):(\\S+) ");
        int pos = 0;
        if ((pos = rx.indexIn(metaInfo.c_str())) == -1)
            return;
        m_aodvInfo->rerrInfo = rx.cap(1).toAscii().data();
        m_aodvInfo->destination = rx.cap(2).toAscii().data();
    }

}

void
AnimPacket::parseDsdv(std::string metaInfo)
{
    QRegExp rx("ns3::dsdv::DsdvHeader");
    int pos = 0;
    if ((pos = rx.indexIn(metaInfo.c_str())) == -1)
        return;
    m_dsdvInfo = new DsdvInfo;

}

void
AnimPacket::parseOlsr(std::string metaInfo)
{
    QRegExp rx("ns3::olsr::MessageHeader");
    int pos = 0;
    if ((pos = rx.indexIn(metaInfo.c_str())) == -1)
        return;
    m_olsrInfo = new OlsrInfo;
}

std::string
AnimPacket::getShortMeta()
{
    return m_shortMeta;
}
void
AnimPacket::storeShortMeta(int flags, bool force)
{
    m_shortMeta = "";

    if(m_aodvInfo && (force || (flags & AnimPacket::Aodv)))
    {
        if(m_aodvInfo->type == "RERR")
        {
            m_shortMeta = "AODV:" + m_aodvInfo->type + " " + m_aodvInfo->destination;
            return;
        }
        m_shortMeta = "AODV:" + m_aodvInfo->type + "D:" + m_aodvInfo->destination + " S:" + m_aodvInfo->source ;
        return;
    }

    if(m_olsrInfo && (force || (flags & AnimPacket::Olsr)))
    {
        m_shortMeta = "OLSR";
        return;
    }

    if(m_dsdvInfo && (force || (flags & AnimPacket::Dsdv)))
    {
        m_shortMeta = "DSDV";
        return;
    }

    if(m_tcpInfo && (force || (flags & AnimPacket::Tcp)))
    {
        m_shortMeta =  "TCP:[" + m_tcpInfo->flags + "]" + " S=" + m_tcpInfo->seq +
                " A=" + m_tcpInfo->ack;
        return;
    }
    if(m_udpInfo && (force || (flags & AnimPacket::Udp)))
    {
        m_shortMeta = "UDP:" + m_udpInfo->SPort + " > " + m_udpInfo->DPort;
        return;

    }
    if(m_arpInfo && (force || (flags & AnimPacket::Arp)))
    {
        m_shortMeta = "Arp:" + m_arpInfo->type + " DstIP=" + m_arpInfo->destIpv4;
        return;

    }
    if(m_icmpInfo && (force || (flags & AnimPacket::Icmp)))
    {
        if ((m_icmpInfo->type == "3") & (m_icmpInfo->code == "3"))
        {
            m_shortMeta =  "ICMP: Dst Unreachable";
            return;
        }
        m_shortMeta = "ICMP: type=" + m_icmpInfo->type + " code="+ m_icmpInfo->code;
        return;

    }
    if(m_ipv4Info && (force || (flags & AnimPacket::Ipv4)))
    {
        m_shortMeta =  "IPv4:" + m_ipv4Info->SrcIp + " > " + m_ipv4Info->DstIp;
        return;

    }
    if(m_wifiMacInfo && (force || (flags & AnimPacket::Wifi)))
    {
        if(m_wifiMacInfo->type == "MGT_BEACON")
            m_shortMeta =  "Wifi:BEACON ssid" + m_wifiMacInfo->SSid;
        if(m_wifiMacInfo->type == "MGT_ASSOCIATION_REQUEST")
            m_shortMeta =  "Wifi:ASSOC_REQ ssid" + m_wifiMacInfo->SSid;
        if(m_wifiMacInfo->type == "CTL_ACK")
            m_shortMeta =  "Wifi:CTL_ACK RA:" + m_wifiMacInfo->Ra;
        else
            m_shortMeta =  "Wifi:" + m_wifiMacInfo->type;
        return;

    }
    if(m_pppInfo && (force || (flags & AnimPacket::Ppp)))
    {
        m_shortMeta =  "PPP";
        return;

    }
    if(m_ethernetInfo && (force || (flags & AnimPacket::Ethernet)))
    {
        m_shortMeta =  "Ethernet:" + m_ethernetInfo->sourceMac + " > " + m_ethernetInfo->destMac;
        return;

    }

}

std::string
AnimPacket::getMeta(int flags)
{    std::string metaString = "";



     if(m_pppInfo && ((flags & AnimPacket::Ppp) || (flags & AnimPacket::All)))
        metaString += m_pppInfo->toString();
    if(m_ethernetInfo && ((flags & AnimPacket::Ethernet) || (flags & AnimPacket::All)))
        metaString += m_ethernetInfo->toString();
    if(m_wifiMacInfo && ((flags & AnimPacket::Wifi) || (flags & AnimPacket::All)))
        metaString += m_wifiMacInfo->toString();
    if(m_arpInfo && ((flags & AnimPacket::Arp) || (flags & AnimPacket::All)))
        metaString += m_arpInfo->toString();
    if(m_ipv4Info && ((flags & AnimPacket::Ipv4) || (flags & AnimPacket::All)))
        metaString += m_ipv4Info->toString();
    if(m_icmpInfo && ((flags & AnimPacket::Icmp) || (flags & AnimPacket::All)))
        metaString += m_icmpInfo->toString();
    if(m_udpInfo && ((flags & AnimPacket::Udp) || (flags & AnimPacket::All)))
        metaString += m_udpInfo->toString();
    if(m_tcpInfo && ((flags & AnimPacket::Tcp) || (flags & AnimPacket::All)))
        metaString += m_tcpInfo->toString();
    if(m_aodvInfo && ((flags & AnimPacket::Aodv) || (flags & AnimPacket::All)))
        metaString += m_aodvInfo->toString();
    if(m_olsrInfo && ((flags & AnimPacket::Olsr) || (flags & AnimPacket::All)))
        metaString += m_olsrInfo->toString();
    if(m_dsdvInfo && ((flags & AnimPacket::Dsdv) || (flags & AnimPacket::All)))
        metaString += m_dsdvInfo->toString();

    bool force = (flags & AnimPacket::All)? true:false;
    storeShortMeta (flags , force);
    if((metaString == "") && (flags & AnimPacket::All))
        return "Unclassified packet: All Packet Filter applied";

    return metaString;
}

void
AnimPacket::setFontSize(int value)
{
    gFont.setPixelSize(value);
    gFont.setStyleHint(QFont::SansSerif);
    gFont.setWeight(QFont::Light);
    gFont.setStyleStrategy(QFont::PreferAntialias);
}

void
AnimPacket::parseMeta(std::string metaInfo)
{

    parsePpp(metaInfo);
    parseEthernet(metaInfo);
    parseWifi(metaInfo);
    parseArp(metaInfo);
    parseIpv4(metaInfo);
    parseIcmp(metaInfo);
    parseUdp(metaInfo);
    parseTcp(metaInfo);
    parseAodv(metaInfo);
    parseDsdv(metaInfo);
    parseOlsr(metaInfo);
    storeShortMeta(1, true);

}

uint32_t
AnimPacket::getAnimId()
{
    return m_animId;
}

void
AnimPacket::systemReset()
{
    gAnimId = 1;
}



void
AnimPacket::setTextTransform(std::string text, qreal angle)
{
    QTransform currentTextTransform;
    currentTextTransform.rotate(angle);
    m_graphicsTextItem->setPlainText(text.c_str());
    m_graphicsTextItem->setTransform(currentTextTransform);
    m_graphicsTextItem->setFont(gFont);

}


} // namespace netanim
