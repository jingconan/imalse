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


#include "animxmlparser.h"
#include "debug/xdebug.h"

#include <QFile>
#include <QRegExp>

namespace netanim {

Animxmlparser::Animxmlparser(QString traceFileName):
    m_traceFileName(traceFileName),
    m_parsingComplete(false),
    m_reader(0),
    m_maxSimulationTime(0),
    m_fileIsValid(true)
{
    if(m_traceFileName == "")
        return;

    m_traceFile = new QFile(m_traceFileName);
    if (!m_traceFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug(QString("Critical:Trace file is invalid"));
        m_fileIsValid = false;
        return;
    }
    //qDebug(m_traceFileName);
    m_reader = new QXmlStreamReader(m_traceFile);
}

Animxmlparser::~Animxmlparser()
{
    if(m_traceFile)
        delete m_traceFile;
    if(m_reader)
        delete m_reader;
}

uint64_t
Animxmlparser::getRxCount()
{
    uint64_t count = 0;
    QFile * f = new QFile(m_traceFileName);
    if(f->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString allContent = QString(f->readAll());
        int j = 0;

        while ((j = allContent.indexOf(" toId=", j)) != -1) {
            ++j;
            ++count;
        }
        f->close();
        delete f;
        return count;
    }
    return count;
}

bool
Animxmlparser::isFileValid()
{
    return m_fileIsValid;
}

bool
Animxmlparser::isParsingComplete()
{
    return m_parsingComplete;
}




ParsedElement
Animxmlparser::parseNext()
{
    ParsedElement parsedElement;
    parsedElement.type = XML_INVALID;

    if(m_reader->atEnd() || m_reader->hasError())
    {
        m_parsingComplete = true;
        m_traceFile->close();
        return parsedElement;
    }



    QXmlStreamReader::TokenType token =  m_reader->readNext();
    if(token == QXmlStreamReader::StartDocument)
        return parsedElement;

    if(token == QXmlStreamReader::StartElement)
    {
        if(m_reader->name() == "topology")
        {
            parsedElement = parseTopology();
        }
        if(m_reader->name() == "node")
        {
            parsedElement = parseNode();

        }
        if(m_reader->name() == "packet")
        {
            parsedElement = parsePacket();

        }
        if(m_reader->name() == "wpacket")
        {
            parsedElement = parseWPacket();
        }
        if(m_reader->name() == "link")
        {
            parsedElement = parseLink();
        }
        //qDebug(m_reader->name().toString());
    }

    if(m_reader->atEnd())
    {
        m_parsingComplete = true;
        m_traceFile->close();
    }
    return parsedElement;
}




ParsedElement
Animxmlparser::parseTopology()
{
    ParsedElement parsedElement;
    parsedElement.type = XML_TOPOLOGY;
    parsedElement.topo_width = m_reader->attributes().value("maxX").toString().toDouble();
    parsedElement.topo_height = m_reader->attributes().value("maxY").toString().toDouble();
    return parsedElement;

}

ParsedElement
Animxmlparser::parseLink()
{
    ParsedElement parsedElement;
    parsedElement.type = XML_LINK;
    parsedElement.link_fromId = m_reader->attributes().value("fromId").toString().toUInt();
    parsedElement.link_toId = m_reader->attributes().value("toId").toString().toDouble();
    return parsedElement;


}

ParsedElement
Animxmlparser::parseNode()
{
    ParsedElement parsedElement;
    parsedElement.type = XML_NODE;
    parsedElement.nodeId = m_reader->attributes().value("id").toString().toUInt();
    parsedElement.node_x = m_reader->attributes().value("locX").toString().toDouble();
    parsedElement.node_y = m_reader->attributes().value("locY").toString().toDouble();
    parsedElement.nodeDescription = m_reader->attributes().value("descr").toString();
    return parsedElement;

}


ParsedElement
Animxmlparser::parsePacket()
{
    ParsedElement parsedElement;
    parsedElement.type = XML_PACKET_RX;
    parsedElement.packetrx_fromId = m_reader->attributes().value("fromId").toString().toUInt();
    parsedElement.packetrx_fbTx = m_reader->attributes().value("fbTx").toString().toDouble();
    parsedElement.packetrx_lbTx = m_reader->attributes().value("lbTx").toString().toDouble();
    parsedElement.meta_info = "null";
    m_maxSimulationTime = std::max(m_maxSimulationTime,parsedElement.packetrx_lbTx);
    while(m_reader->name() != "rx")
        m_reader->readNext();

    if(m_reader->atEnd() || m_reader->hasError())
    {
        m_parsingComplete = true;
        m_traceFile->close();
        return parsedElement;
    }

    parsedElement.packetrx_toId = m_reader->attributes().value("toId").toString().toUInt();
    parsedElement.packetrx_fbRx = m_reader->attributes().value("fbRx").toString().toDouble();
    parsedElement.packetrx_lbRx = m_reader->attributes().value("lbRx").toString().toDouble();
    m_maxSimulationTime = std::max(m_maxSimulationTime, parsedElement.packetrx_lbRx);

    while(m_reader->name() == "rx")
        m_reader->readNext();
    if(m_reader->name() == "packet")
        return parsedElement;
    m_reader->readNext();
    if(m_reader->name() != "meta")
        return parsedElement;
    parsedElement.meta_info = m_reader->attributes().value("info").toString();
    //qDebug(parsedElement.meta_info);
    return parsedElement;

}

ParsedElement
Animxmlparser::parseWPacket()
{

    ParsedElement parsedElement;
    parsedElement.type = XML_WPACKET_RX;
    parsedElement.packetrx_fromId = m_reader->attributes().value("fromId").toString().toUInt();
    parsedElement.packetrx_fbTx = m_reader->attributes().value("fbTx").toString().toDouble();
    parsedElement.packetrx_lbTx = m_reader->attributes().value("lbTx").toString().toDouble();
    parsedElement.meta_info = "null";
    m_maxSimulationTime = std::max(m_maxSimulationTime,parsedElement.packetrx_lbTx);
    while(m_reader->name() != "rx")
        m_reader->readNext();
    if(m_reader->atEnd() || m_reader->hasError())
    {
        m_parsingComplete = true;
        m_traceFile->close();
        return parsedElement;
    }

    //qDebug(m_reader->name().toString()+"parseWpacket");
    parsedElement.packetrx_toId = m_reader->attributes().value("toId").toString().toUInt();
    parsedElement.packetrx_fbRx = m_reader->attributes().value("fbRx").toString().toDouble();
    parsedElement.packetrx_lbRx = m_reader->attributes().value("lbRx").toString().toDouble();
    m_maxSimulationTime = std::max(m_maxSimulationTime, parsedElement.packetrx_lbRx);
    while(m_reader->name() == "rx")
        m_reader->readNext();
    if(m_reader->name() == "wpacket")
        return parsedElement;
    m_reader->readNext();
    if(m_reader->name() != "meta")
        return parsedElement;
    parsedElement.meta_info = m_reader->attributes().value("info").toString();
    //qDebug(parsedElement.meta_info);
    return parsedElement;

}

double
Animxmlparser::getMaxSimulationTime()
{
    return m_maxSimulationTime;
}


} // namespace netanim
