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

#ifndef ANIMPACKET_H
#define ANIMPACKET_H

#include <stdint.h>

#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsTextItem>
#include <QPropertyAnimation>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include <QGraphicsPathItem>


namespace netanim {

struct ArpInfo
{
    ArpInfo()
    {
        type = "null";
        sourceMac = "null";
        sourceIpv4 = "null";
        destMac = "ff:ff:ff:ff:ff:ff";
        destIpv4 = "null";
    }
    std::string toString()
    {
        return  " Arp "     + type       +
                " SMac: "   + sourceMac  +
                " DMac: "   + destMac    +
                " SrcIp : " + sourceIpv4 +
                " DstIp : " + destIpv4;
    }
    std::string type;
    std::string sourceMac;
    std::string sourceIpv4;
    std::string destMac;
    std::string destIpv4;
};

struct PppInfo
{
    std::string toString()
    {
        return " PPP";
    }

};

struct EthernetInfo
{
    EthernetInfo()
    {
        sourceMac = "null";
        destMac = "null";
    }
    std::string toString()
    {
        return  " Ethernet SMac: " + sourceMac +
                " DMac: "          + destMac;
    }
    std::string sourceMac;
    std::string destMac;
};


struct WifiMacInfo
{
    WifiMacInfo()
    {
        type = "null";
        toDs = "null";
        fromDs = "null";
        Da = "null";
        Sa = "null";
        Bssid = "null";
        Ra = "null";
        SSid = "null";
        assocResponseStatus = "null";

    }

    std::string toString()
    {
        if(type == "CTL_ACK")
            return " Wifi CTL_ACK RA:" + Ra;
        std::string temp = " Wifi " + type +
                " FromDS: " + fromDs +
                " toDS: " + toDs +
                " DA: " + Da +
                " SA: " + Sa +
                " BSSId: " + Bssid;
        if(type == "MGT_ASSOCIATION_REQUEST")
            temp += " SSid: " + SSid;

        if(type == "MGT_ASSOCIATION_RESPONSE")
            temp += " status: " + assocResponseStatus;
        return temp;
    }
    std::string type;
    std::string toDs;
    std::string fromDs;
    std::string Da;
    std::string Sa;
    std::string Bssid;
    std::string Ra;
    std::string SSid;
    std::string assocResponseStatus;
};


struct Ipv4Info
{
    Ipv4Info()
    {

    }
    std::string toString ()
    {
        return  " Ipv4 Proto:" + protocol +
                " SrcIp: " + SrcIp +
                " DstIp: " + DstIp;
    }
    std::string tos;
    std::string Dscp;
    std::string Ecn;
    std::string Ttl;
    std::string Id;
    std::string protocol;
    std::string length;
    std::string SrcIp;
    std::string DstIp;

};


struct IcmpInfo
{
    IcmpInfo()
    {

    }
    std::string toString()
    {
        std::string temp;
        temp += "ICMP type: " + type +
                "code: " + code;
        if (type == "3" && code == "3")
            temp += " DstUnreachable";
        return temp;

    }
    std::string type;
    std::string code;
};


struct UdpInfo
{
    UdpInfo()
    {

    }
    std::string toString()
    {
        return " UDP " + SPort + " > " + DPort;

    }
    std::string length;
    std::string SPort;
    std::string DPort;

};

struct TcpInfo
{
    TcpInfo()
    {

    }
    std::string toString()
    {
        return " TCP " + SPort + " > " + DPort +
                " " + flags +  " Seq=" + seq   +
                " Ack=" + ack + " Win=" + window;

    }
    std::string SPort;
    std::string DPort;
    std::string flags;
    std::string seq;
    std::string ack;
    std::string window;

};

struct AodvInfo
{
    AodvInfo()
    {

    }
    std::string toString()
    {
        if(type == "RERR")
        {
            return "RERR:" + rerrInfo + " " + destination;
        }
        return "AODV:" + type + " D=" + destination + " S=" + source + " Seq=" + seq;
    }
    std::string type;
    std::string destination;
    std::string source;
    std::string seq;
    std::string rerrInfo;
};

struct DsdvInfo
{
    DsdvInfo()
    {

    }
    std::string toString()
    {
        return "DSDV";
    }
};

struct OlsrInfo
{
    OlsrInfo()
    {

    }
    std::string toString()
    {
        return "OLSR";
    }
};


class AnimPacket
{
public:
    AnimPacket(uint32_t fromId,
                uint32_t toId,
                double fbTx,
                double lbTx,
                double fbRx,
                double lbRx,
                bool isWPacket);
    ~AnimPacket();
    enum AnimPacketFilterFlags
    {
        None = 0,
        All = 1,
        Ppp = 2,
        Ethernet = 4,
        Wifi = 8,
        Arp = 16,
        Ipv4 = 32,
        Tcp = 64,
        Udp = 128,
        Icmp = 256,
        Aodv = 512,
        Olsr = 1024,
        Dsdv = 2048

    };
    uint32_t getAnimId();
    uint32_t m_fromId;
    uint32_t m_toId;
    double m_fbTx;
    double * m_lbTx;
    double * m_fbRx;
    double * m_lbRx;
    uint32_t m_animId;
    QGraphicsLineItem * m_graphicsItem;
    QGraphicsTextItem * m_graphicsTextItem;
    bool m_isWpacket;
    bool m_selected;

    std::string m_shortMeta;

    static QFont gFont;
    static void setFontSize(int value);

    void setPointPosition(QPoint pos);
    void setLinePosition(QLineF line);
    static void systemReset();
    void parseMeta(std::string metaInfo);
    std::string getMeta(int flags);
    void storeShortMeta(int flags, bool force = false);
    std::string getShortMeta();
    void setTextTransform(std::string text, qreal angle);


// Meta info
    ArpInfo * m_arpInfo;
    PppInfo * m_pppInfo;
    EthernetInfo * m_ethernetInfo;
    WifiMacInfo * m_wifiMacInfo;
    Ipv4Info * m_ipv4Info;
    IcmpInfo * m_icmpInfo;
    UdpInfo * m_udpInfo;
    TcpInfo * m_tcpInfo;
    AodvInfo * m_aodvInfo;
    OlsrInfo * m_olsrInfo;
    DsdvInfo * m_dsdvInfo;



    void clearInfo();
    void parseArp(std::string metaInfo);
    void parsePpp(std::string metaInfo);
    void parseEthernet(std::string metaInfo);
    void parseWifi(std::string metaInfo);
    void parseIpv4(std::string metaInfo);
    void parseIcmp(std::string metaInfo);
    void parseUdp(std::string metaInfo);
    void parseTcp(std::string metaInfo);
    void parseAodv(std::string metaInfo);
    void parseDsdv(std::string metaInfo);
    void parseOlsr(std::string metaInfo);

};

} // namespace netanim
#endif // ANIMPACKET_H
