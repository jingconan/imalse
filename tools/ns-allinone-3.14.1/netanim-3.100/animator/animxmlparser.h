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


#ifndef ANIMXMLPARSER_H
#define ANIMXMLPARSER_H

#include <stdint.h>

#include <QDialog>
#include <QFile>
#include <QtCore/QXmlStreamReader>
#include <QLabel>
#include <QTableWidget>

namespace netanim {

enum ParsedElementType
{
    XML_INVALID,
    XML_TOPOLOGY,
    XML_NODE,
    XML_LINK,
    XML_PACKET_RX,
    XML_WPACKET_RX
};

struct ParsedElement
{
    ParsedElementType type;

    // Topology
    qreal topo_width;
    qreal topo_height;

    // Node

    uint32_t nodeId;
    qreal node_x;
    qreal node_y;

    // Link

    uint32_t link_fromId;
    uint32_t link_toId;

    // Packet Rx
    double packetrx_fbTx;
    double packetrx_lbTx;
    uint32_t packetrx_fromId;
    double packetrx_toId;
    double packetrx_fbRx;
    double packetrx_lbRx;


    //meta-info
    QString meta_info;
    QString nodeDescription;

};


class Animxmlparser
{
public:
    Animxmlparser(QString traceFileName);
    ~Animxmlparser();
    ParsedElement parseNext();
    bool isParsingComplete();
    double getMaxSimulationTime();
    bool isFileValid();
    uint64_t getRxCount();


private:
    QString m_traceFileName;
    bool m_parsingComplete;
    QXmlStreamReader * m_reader;
    QFile * m_traceFile;
    double m_maxSimulationTime;
    bool m_fileIsValid;

    ParsedElement parseTopology();
    ParsedElement parseNode();
    ParsedElement parseLink();
    ParsedElement parsePacket();
    ParsedElement parseWPacket();

};

} // namespace netanim
#endif // ANIMXMLPARSER_H
