// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <DLabel>
#include <DPalette>

#include <QColor>
#include <QFont>
#include <QIcon>
#include <QSizePolicy>

#include <string>
#include <unistd.h>

#include "upgradecheckprogresswidget.h"
#include "../../widgets/baselabel.h"
#include "../../widgets/simplelabel.h"


UpgradeCheckProgressWidget::UpgradeCheckProgressWidget(DWidget *parent)
    : DWidget(parent)
    , m_titleLabel(new DLabel(tr("Pre-Check"), this))
    , m_dbusWorker(DBusWorker::getInstance(this))
    , m_mainVBoxLayout(new QVBoxLayout(this))
    , m_checklistVBoxLayout(new QVBoxLayout(this))
    , m_panelHBoxLayout(new QHBoxLayout(this))
    , m_checkProgress(new ProgressCircleWidget(this))
    , m_conditionsCheckLabel(new IconLabel(this))
    , m_softwareChangesLabel(new IconLabel(this))
{
    initUI();
    initConnections();
}

void UpgradeCheckProgressWidget::initUI()
{
    QFont titleFont("SourceHanSansSC", 17, QFont::DemiBold);

    m_titleLabel->setForegroundRole(DPalette::TextTitle);
    m_titleLabel->setFont(titleFont);

    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(1);
    sizePolicy1.setVerticalStretch(1);

    DLabel *conditionsTextLabel = new DLabel;
    conditionsTextLabel->setFont(QFont("SourceHanSansSC", 10, QFont::Normal));
    conditionsTextLabel->setForegroundRole(DPalette::TextTitle);
    conditionsTextLabel->setText(tr("Check your computer to see if it meets the upgrade conditions"));

    // 进度显示组件
    m_checkProgress->setFixedSize(162, 162);
    // m_checkProgress->setSizePolicy(sizePolicy2);
    m_checkProgress->setValue(1);
    // 开始动画,否则除非切换窗口，进度不会实时更新。
    m_checkProgress->start();


    // 主要框架布局
    m_mainVBoxLayout->setContentsMargins(0, 80, 0, 0);
    m_mainVBoxLayout->setSpacing(0);
    m_mainVBoxLayout->addWidget(m_checkProgress, 1, Qt::AlignCenter);
    m_mainVBoxLayout->addSpacing(20);
    m_mainVBoxLayout->addWidget(m_titleLabel, 0, Qt::AlignCenter);
    m_mainVBoxLayout->addSpacing(6);
    m_mainVBoxLayout->addWidget(conditionsTextLabel, 0, Qt::AlignCenter);
    m_mainVBoxLayout->addSpacing(90);

    setLayout(m_mainVBoxLayout);
}

void UpgradeCheckProgressWidget::initConnections()
{
    connect(this, SIGNAL(CheckDone()), this, SLOT(onCheckDone()));
    connect(m_dbusWorker, &DBusWorker::CheckProgressUpdate, this, &UpgradeCheckProgressWidget::onCheckProgressUpdate);
}

void UpgradeCheckProgressWidget::onCheckProgressUpdate(qint64 progress)
{
    m_checkProgress->setValue(progress);
    if (progress == 100)
    {
        Q_EMIT CheckDone();
    }
}

void UpgradeCheckProgressWidget::onCheckDone()
{
    Q_EMIT AllCheckDone();
}
