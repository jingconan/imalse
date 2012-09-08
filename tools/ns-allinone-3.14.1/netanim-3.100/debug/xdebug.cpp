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

#include "xdebug.h"

#include <QMessageBox>

namespace netanim {

void
qDebug(QString string)
{
    ::qDebug("%s",string.toAscii().data());
}

void
qDebug(qreal x)
{
    qDebug(QString::number(x));

}


void
qDebug(QTransform transform)
{
    QString string;
    string =  "m11:" + QString::number(transform.m11()) + "\n";
    string += "m12:" + QString::number(transform.m12()) + "\n";
    string += "m13:" + QString::number(transform.m13()) + "\n";
    string += "m21:" + QString::number(transform.m21()) + "\n";
    string += "m22:" + QString::number(transform.m22()) + "\n";
    string += "m23::" + QString::number(transform.m23()) + "\n";
    qDebug(string);
}

void
qDebug(QPointF pt,QString prefix)
{
    QString string;
    string = prefix + ": x:" + QString::number(pt.x()) + " y:" + QString::number(pt.y());
    qDebug(string);

}

void
qDebug(qreal x,QString prefix)
{

    QString string;
    string = prefix + ":" + QString::number(x);
    qDebug(string);
}


void
fatalError(QString msg)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.exec();
    exit(-1);

}

} // namespace netanim
