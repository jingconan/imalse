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

#ifndef ATTRIBUTERANDOMVARIABLEWIDGET_H
#define ATTRIBUTERANDOMVARIABLEWIDGET_H

#include "ns3/core-module.h"

#include <QWidget>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>

namespace netanim {

class AttributeRandomvariableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AttributeRandomvariableWidget(QWidget *parent = 0);
    ns3::RandomVariableValue getRandomVariableValue();

private:
    QComboBox * m_supportedRVComboBox;
    QHBoxLayout * m_hLayout;


    // Constant
    QLabel * m_constant_DoublecLabel;
    QLineEdit * m_constant_DoublecLineEdit;

    // Uniform
    QLabel * m_uniform_DoubleLowLabel;
    QLineEdit * m_uniform_DoubleLowLineEdit;
    QLabel * m_uniform_DoubleHiLabel;
    QLineEdit * m_uniform_DoubleHiLineEdit;

    QVector <QWidget *> m_allValueWidgets;

    void addSupportedRVs();
    void initConstantElements();
    void initUniformElements();
    void activateConstant();
    void activateUniform();
    void hideValueWidgets();



signals:

public slots:

private slots:
    void RVComboIndexChanged(QString change);


};

} // namespace netanim

#endif // ATTRIBUTERANDOMVARIABLEWIDGET_H
