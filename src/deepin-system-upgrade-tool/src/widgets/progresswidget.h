// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <DProgressBar>

#include <QString>
#include <QSpacerItem>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>

#include "../widgets/simplelabel.h"
#include "../widgets/iconlabel.h"

QString secondsToReadableTime(time_t sec);


class ProgressWidget: public QWidget
{
    Q_OBJECT
public:
    explicit ProgressWidget(QWidget *parent = nullptr);

protected:
    // -1为尚未初始化状态，因为可能断点续传，所以每次下载都需要得知初始进度值
    qint64      m_initProgress = -1;
    qint64      m_lastProgress = 0;
    time_t      m_startTime = 0;

    DProgressBar    *m_progressbar;
    QVBoxLayout     *m_mainLayout;
    QHBoxLayout     *m_progressbarLayout;
    SimpleLabel     *m_iconLabel;
    SimpleLabel     *m_titleLabel;
    SimpleLabel     *m_tipLabel;
    SimpleLabel     *m_progressLabel;
    SimpleLabel     *m_etaLabel;
    QSpacerItem     *m_iconSpacerItem;
    QSpacerItem     *m_tipSpacerItem;

    void initUI();
    void initConnections();

protected Q_SLOTS:
    // 默认的更新预计完成时间的SLOT。默认不连接上updateProgress信号，需要用的请自行连接。
    void updateDefaultEtaText(qint64 p);

    // 开始进度的槽。初始化开始时间和初始进度，以便计算预计时间。需要连接自定义的开始信号。
    void onStart();

Q_SIGNALS:
    void updateProgress(qint64 progress);

private Q_SLOTS:
    void onUpdateProgress(qint64 progress);
};
