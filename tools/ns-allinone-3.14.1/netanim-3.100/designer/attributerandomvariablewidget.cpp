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

#include "attributerandomvariablewidget.h"
#include "debug/xdebug.h"

#include <QComboBox>

namespace netanim {

AttributeRandomvariableWidget::AttributeRandomvariableWidget(QWidget *parent) :
    QWidget(parent)
{
    m_supportedRVComboBox = new QComboBox;
    connect(m_supportedRVComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(RVComboIndexChanged(QString)));
    m_hLayout = new QHBoxLayout;
    m_hLayout->addWidget(m_supportedRVComboBox);
    setLayout(m_hLayout);

    initConstantElements();
    initUniformElements();
    addSupportedRVs();

    m_supportedRVComboBox->setCurrentIndex(m_supportedRVComboBox->findText("Constant"));
}


void
AttributeRandomvariableWidget::RVComboIndexChanged(QString change)
{
    hideValueWidgets();
    if(change == "Constant")
        activateConstant();
    if(change == "Uniform")
        activateUniform();
    adjustSize();

}

void
AttributeRandomvariableWidget::addSupportedRVs()
{
    QStringList supportedRVs;
    supportedRVs << "None"
                 << "Constant"
                 << "Uniform";
    m_supportedRVComboBox->addItems(supportedRVs);
}


void
AttributeRandomvariableWidget::initConstantElements()
{
    m_constant_DoublecLabel =  new QLabel("Value");
    m_hLayout->addWidget(m_constant_DoublecLabel);
    m_constant_DoublecLineEdit = new QLineEdit;
    m_hLayout->addWidget(m_constant_DoublecLineEdit);
    m_constant_DoublecLineEdit->setText("1");

    m_constant_DoublecLabel->setVisible(false);
    m_constant_DoublecLineEdit->setVisible(false);

    m_allValueWidgets.push_back(m_constant_DoublecLabel);
    m_allValueWidgets.push_back(m_constant_DoublecLineEdit);

}

void
AttributeRandomvariableWidget::initUniformElements()
{
    m_uniform_DoubleLowLabel = new QLabel("Low");
    m_hLayout->addWidget(m_uniform_DoubleLowLabel);
    m_uniform_DoubleLowLineEdit = new QLineEdit;
    m_hLayout->addWidget(m_uniform_DoubleLowLineEdit);

    m_uniform_DoubleHiLabel = new QLabel("High");
    m_hLayout->addWidget(m_uniform_DoubleHiLabel);
    m_uniform_DoubleHiLineEdit = new QLineEdit;
    m_hLayout->addWidget(m_uniform_DoubleHiLineEdit);

    m_uniform_DoubleLowLabel->setVisible(false);
    m_uniform_DoubleLowLineEdit->setVisible(false);
    m_uniform_DoubleHiLabel->setVisible(false);
    m_uniform_DoubleHiLineEdit->setVisible(false);


    m_allValueWidgets.push_back(m_uniform_DoubleLowLabel);
    m_allValueWidgets.push_back(m_uniform_DoubleLowLineEdit);
    m_allValueWidgets.push_back(m_uniform_DoubleHiLabel);
    m_allValueWidgets.push_back(m_uniform_DoubleHiLineEdit);

}

void
AttributeRandomvariableWidget::activateConstant()
{
    m_constant_DoublecLabel->setVisible(true);
    m_constant_DoublecLineEdit->setVisible(true);
}

void
AttributeRandomvariableWidget::activateUniform()
{
    m_uniform_DoubleLowLabel->setVisible(true);
    m_uniform_DoubleLowLineEdit->setVisible(true);
    m_uniform_DoubleHiLabel->setVisible(true);
    m_uniform_DoubleHiLineEdit->setVisible(true);

}


void
AttributeRandomvariableWidget::hideValueWidgets()
{
    for(int i = 0; i < m_allValueWidgets.size(); ++i)
    {
        m_allValueWidgets[i]->setVisible(false);
    }
}




ns3::RandomVariableValue
AttributeRandomvariableWidget::getRandomVariableValue()
{
    QString currentRvType = m_supportedRVComboBox->currentText();
    if(currentRvType == "None")
    {
        return ns3::ConstantVariable(0);
    }
    else if(currentRvType == "Constant")
    {
        return ns3::ConstantVariable(m_constant_DoublecLineEdit->text().toDouble());
    }
    else if(currentRvType == "Uniform")
    {
        return ns3::UniformVariable(m_uniform_DoubleLowLineEdit->text().toDouble(),
                                    m_uniform_DoubleHiLineEdit->text().toDouble());
    }
    return ns3::RandomVariableValue(ns3::RandomVariable());

}


} // namespace netanim
