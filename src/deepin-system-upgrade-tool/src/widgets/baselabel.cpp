// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "baselabel.h"

BaseLabel::BaseLabel(QWidget *parent)
    : QWidget(parent)
    , m_label(new SimpleLabel(this))
    , m_tip(new SimpleLabel(this))
    , m_layout(new QVBoxLayout(this))
{
    initUI();
}

void BaseLabel::initUI()
{
    QFont titleFont("SourceHanSansSC", 10, QFont::Medium);
    QFont tipFont("SourceHanSansSC", 9, QFont::Normal);

    m_label->setFont(titleFont);
    m_label->setForegroundRole(DPalette::TextTitle);

    m_tip->setFont(tipFont);
    m_tip->setForegroundRole(DPalette::TextTips);

    m_layout->addWidget(m_label, 0, Qt::AlignBottom);
    m_layout->addWidget(m_tip, 0, Qt::AlignTop);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_layout);
}

void BaseLabel::setText(QString text)
{
    m_label->setText(text);
}

void BaseLabel::setTip(QString text)
{
    m_tip->setText(text);
}

void BaseLabel::setFont(QFont font)
{
    m_label->setFont(font);
}

void BaseLabel::setTextColor(QColor color)
{
    QPalette palette = m_label->palette();
    palette.setColor(m_label->foregroundRole(), color);
    m_label->setPalette(palette);
}

void BaseLabel::setTipColor(QColor color)
{
    QPalette palette = m_tip->palette();
    palette.setColor(m_tip->foregroundRole(), color);
    m_tip->setPalette(palette);
}
