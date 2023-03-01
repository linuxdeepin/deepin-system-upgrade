// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <DDialog>
#include <DSwitchButton>

#include <QUrl>
#include <QSpacerItem>
#include <QPushButton>
#include <QDesktopServices>

#include "../core/dbusworker.h"
#include "storageresultwidget.h"
#include "backgroundframe.h"
#include "baselabel.h"

#define ICON_SIZE 15
#define RESULT_TEXT_SIZE 9
#define RIGHT_SPACE 35
#define RIGHT_SPACE_WITH_CLEANUP 2

StorageResultWidget::StorageResultWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QHBoxLayout(this))
    , m_leftLayout(new QVBoxLayout(this))
    , m_resultLayout(new QVBoxLayout(this))
    , m_systemResultLayout(new QHBoxLayout(this))
    , m_dataResultLayout(new QHBoxLayout(this))
    , m_tipLayout(new QHBoxLayout(this))
    , m_checkResultIconLabel(new IconLabel(this))
    , m_requirementTitleLabel(new DLabel(this))
    , m_requirementTipLabel(new SimpleLabel(this))
    , m_systemResultTextLabel(new DLabel(this))
    , m_dataResultTextLabel(new DLabel(this))
    , m_systemStatusLabel(new DLabel(this))
    , m_dataStatusLabel(new DLabel(this))
    , m_cleanupSystemLabel(new DLabel(QString("<a href=\"dummy_url\" style=\"text-decoration: none;\">%1</a>").arg(tr("Free up")), this))
    , m_cleanupDataLabel(new DLabel(QString("<a href=\"dummy_url\" style=\"text-decoration: none;\">%1</a>").arg(tr("Free up")), this))
    , m_cleanupSystemSpacerItem(new QSpacerItem(RIGHT_SPACE, 0, QSizePolicy::Maximum))
    , m_cleanupDataSpacerItem(new QSpacerItem(RIGHT_SPACE, 0, QSizePolicy::Maximum))
{
    initUI();
    initConnections();
}

void StorageResultWidget::initUI()
{
    QFont titleFont("SourceHanSansSC", 10, QFont::Medium);
    QFont tipFont("SourceHanSansSC", 9, QFont::Thin);
    QColor titleColor("#414D68");

    m_requirementTitleLabel->setForegroundRole(DPalette::TextTitle);
    m_requirementTitleLabel->setFont(titleFont);

    m_requirementTipLabel->setForegroundRole(DPalette::TextTips);
    m_requirementTipLabel->setFont(tipFont);

    m_systemResultTextLabel->setFont(QFont("SourceHanSansSC", RESULT_TEXT_SIZE));
    m_dataResultTextLabel->setFont(QFont("SourceHanSansSC", RESULT_TEXT_SIZE));

    m_systemResultLayout->addSpacerItem(new QSpacerItem(114514, 0, QSizePolicy::Maximum));
    m_systemResultLayout->addWidget(m_systemResultTextLabel);
    m_systemResultLayout->addSpacing(8);
    m_systemResultLayout->addWidget(m_systemStatusLabel, 0, Qt::AlignRight);
    m_systemResultLayout->addSpacing(3);
    m_systemResultLayout->addWidget(m_cleanupSystemLabel, 0, Qt::AlignRight);
    m_systemResultLayout->addSpacerItem(m_cleanupSystemSpacerItem);

    m_dataResultLayout->addSpacerItem(new QSpacerItem(114514, 0, QSizePolicy::Maximum));
    m_dataResultLayout->addWidget(m_dataResultTextLabel);
    m_dataResultLayout->addSpacing(8);
    m_dataResultLayout->addWidget(m_dataStatusLabel, 0, Qt::AlignRight);
    m_dataResultLayout->addSpacing(3);
    m_dataResultLayout->addWidget(m_cleanupDataLabel, 0, Qt::AlignRight);
    m_dataResultLayout->addSpacerItem(m_cleanupDataSpacerItem);

    m_resultLayout->setContentsMargins(0, 0, 0, 0);
    m_resultLayout->addLayout(m_dataResultLayout);
    m_resultLayout->addLayout(m_systemResultLayout);
    m_resultLayout->addSpacing(114514);

    m_checkResultIconLabel->addSpacing(8);
    m_checkResultIconLabel->addWidget(m_requirementTitleLabel);
    m_checkResultIconLabel->addSpacing(666);
    m_checkResultIconLabel->layout()->setContentsMargins(0, 0, 0, 0);

    m_tipLayout->addSpacing(26);
    m_tipLayout->addWidget(m_requirementTipLabel);
    m_tipLayout->setContentsMargins(0, 0, 0, 0);

    m_leftLayout->addWidget(m_checkResultIconLabel);
    m_leftLayout->addLayout(m_tipLayout);
    m_leftLayout->addSpacing(114514);
    m_leftLayout->setContentsMargins(0, 0, 0, 0);

    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->addLayout(m_leftLayout);
    m_mainLayout->addLayout(m_resultLayout);
    setLayout(m_mainLayout);
}

void StorageResultWidget::initConnections()
{
    connect(m_cleanupSystemLabel, &DLabel::linkActivated, this, &StorageResultWidget::openCleanupDialog);
    connect(m_cleanupDataLabel, &DLabel::linkActivated, this, &StorageResultWidget::openCleanupDialog);
}

void StorageResultWidget::setIcon(QString iconPath)
{
    m_checkResultIconLabel->setIcon(iconPath);
}

void StorageResultWidget::setIcon(QString iconPath, int w, int h)
{
    m_checkResultIconLabel->setIcon(iconPath, w, h);
}

void StorageResultWidget::setStatus(CheckResultType status, DLabel *statusLabel, DLabel *resultTextLabel)
{
    switch(status)
    {
    case CheckResultType::PASSED:
    {
        statusLabel->setToolTip(QString(""));
        // 调整结果Label的颜色
        resultTextLabel->setForegroundRole(DPalette::TextTitle);
    }
    break;
    case CheckResultType::FAILED:
    {
        // 调整结果Label的颜色
        resultTextLabel->setForegroundRole(DPalette::TextWarning);
    }
    break;
    default:
    {
        statusLabel->setToolTip(m_toolTipText);
        resultTextLabel->setForegroundRole(DPalette::TextWarning);
    }
    }
    statusLabel->setPixmap(getIcon(status).pixmap(30, 30).scaled(ICON_SIZE, ICON_SIZE));
}

void StorageResultWidget::setDataStatus(CheckResultType status) {
    m_dataStatus = status;
    if (status == CheckResultType::FAILED)
    {
        m_cleanupDataLabel->setVisible(true);
        m_cleanupDataSpacerItem->changeSize(RIGHT_SPACE_WITH_CLEANUP, 0);
        m_mainLayout->invalidate();
    }
    else
    {
        m_cleanupDataLabel->setVisible(false);
        m_cleanupDataSpacerItem->changeSize(RIGHT_SPACE, 0);
        m_mainLayout->invalidate();
    }
    setStatus(status, m_dataStatusLabel, m_dataResultTextLabel);
}

void StorageResultWidget::setSystemStatus(CheckResultType status) {
    m_systemStatus = status;
    if (status == CheckResultType::FAILED && m_dataStatus == CheckResultType::PASSED)
    {
        m_cleanupSystemLabel->setVisible(true);
        m_cleanupSystemSpacerItem->changeSize(RIGHT_SPACE_WITH_CLEANUP, 0);
        m_mainLayout->invalidate();
    }
    else
    {
        m_cleanupSystemLabel->setVisible(false);
        m_cleanupSystemSpacerItem->changeSize(RIGHT_SPACE, 0);
        m_mainLayout->invalidate();
    }
    setStatus(status, m_systemStatusLabel, m_systemResultTextLabel);
}

void StorageResultWidget::setSystemResult(QString text) {
    m_systemResultTextLabel->setText(text);
}

void StorageResultWidget::setDataResult(QString text) {
    m_dataResultTextLabel->setText(text);
}

void StorageResultWidget::setTitle(QString text)
{
    m_requirementTitleLabel->setText(text);
}

void StorageResultWidget::setToolTip(QString text)
{
    m_toolTipText = text;
}

void StorageResultWidget::setRequirement(QString text)
{
    m_requirementTipLabel->setText(text);
}


void StorageResultWidget::scale(int w, int h)
{
    m_checkResultIconLabel->scale(w, h);
}

void StorageResultWidget::addSpacerItem(QSpacerItem *item)
{
    m_checkResultIconLabel->addSpacerItem(item);
}

void StorageResultWidget::addWidget(QWidget *widget)
{
    m_checkResultIconLabel->addWidget(widget);
}

void StorageResultWidget::openCleanupDialog()
{
    DBusWorker *dbusWorker = DBusWorker::getInstance();
    DDialog dlg(this);
    const QMap<QString, QString> resultMap = dbusWorker->getCheckResultMap();
    double dataBase = resultMap["database"].toDouble();
    double dataFree = resultMap["datafree"].toDouble();
    double systemBase = resultMap["systembase"].toDouble();
    double systemFree = resultMap["systemfree"].toDouble();

    qDebug() << "dataBase:" << dataBase;
    qDebug() << "dataFree:" << dataFree;
    qDebug() << "systemBase:" << systemBase;
    qDebug() << "systemFree:" << systemFree;

    QFont textFont("SourceHanSansSC", 10, QFont::Normal);

    DLabel *titleLabel = new DLabel(tr("Your system cannot be upgraded to V23"));
    titleLabel->setForegroundRole(DPalette::BrightText);
    titleLabel->setFont(textFont);
    dlg.addButton(tr("Cancel"));
    dlg.addButton(tr("Confirm"), false, DDialog::ButtonType::ButtonRecommend);

    BackgroundFrame *frameDoSystemCleanup = new BackgroundFrame;
    QHBoxLayout *doSystemCleanupLayout = new QHBoxLayout;
    doSystemCleanupLayout->setContentsMargins(10, 10, 10, 10);
    BaseLabel *doSystemCleanupTextLabel = new BaseLabel(this);
    doSystemCleanupTextLabel->setText(tr("Clean system disk"));
    doSystemCleanupTextLabel->setTip(tr("%1 GB free space required").arg(systemBase - systemFree));
    doSystemCleanupTextLabel->m_label->setForegroundRole(DPalette::TextTitle);
    QPushButton *doSystemCleanupButton = new QPushButton(tr("Free Up"));
    doSystemCleanupButton->setFixedSize(74, 36);
    doSystemCleanupLayout->addWidget(doSystemCleanupTextLabel);
    doSystemCleanupLayout->addSpacing(0);
    doSystemCleanupLayout->addWidget(doSystemCleanupButton);
    frameDoSystemCleanup->setLayout(doSystemCleanupLayout);

    BackgroundFrame *frameDoDataCleanup = new BackgroundFrame;
    QHBoxLayout *doDataCleanupLayout = new QHBoxLayout;
    doDataCleanupLayout->setContentsMargins(10, 10, 10, 10);
    BaseLabel *doDataCleanupTextLabel = new BaseLabel(this);
    doDataCleanupTextLabel->setText(tr("Clean data disk"));
    doDataCleanupTextLabel->setTip(tr("%1 GB free space required").arg(dataBase - dataFree));
    doDataCleanupTextLabel->m_label->setForegroundRole(DPalette::TextTitle);
    QPushButton *doDataCleanupButton = new QPushButton(tr("Free Up"));
    doDataCleanupButton->setFixedSize(74, 36);
    doDataCleanupLayout->addWidget(doDataCleanupTextLabel);
    doDataCleanupLayout->addSpacing(0);
    doDataCleanupLayout->addWidget(doDataCleanupButton);
    frameDoDataCleanup->setLayout(doDataCleanupLayout);

    // 设置清理按钮功能
    connect(doSystemCleanupButton, &QPushButton::clicked, [] {
        QDesktopServices::openUrl(QUrl::fromLocalFile("/"));
    });
    connect(doDataCleanupButton, &QPushButton::clicked, [] {
        QDesktopServices::openUrl(QUrl::fromLocalFile("/home"));
    });

    // 仅备份系统按钮
    BackgroundFrame *frameBackupSystemOnly = new BackgroundFrame;
    QHBoxLayout *backupSystemOnlyLayout = new QHBoxLayout;

    backupSystemOnlyLayout->setContentsMargins(10, 10, 10, 10);
    DLabel *backupSystemOnlyTextLabel = new DLabel(tr("System backup only, no app backup"));
    backupSystemOnlyTextLabel->setForegroundRole(DPalette::TextTitle);
    backupSystemOnlyTextLabel->setFont(textFont);
    DSwitchButton *backupSystemOnlyButton = new DSwitchButton;
    backupSystemOnlyLayout->addWidget(backupSystemOnlyTextLabel);
    backupSystemOnlyLayout->addSpacing(0);
    backupSystemOnlyLayout->addWidget(backupSystemOnlyButton);
    frameBackupSystemOnly->setLayout(backupSystemOnlyLayout);
    backupSystemOnlyButton->setChecked(!dbusWorker->m_isBackupApps);

    DLabel *tipLabel = new DLabel(tr("Less disk space is required if enabled, but some apps may not work properly after restoring the system from version 23."));
    tipLabel->setFont(QFont("SourceHanSansSC", 8, QFont::Light));
    tipLabel->setForegroundRole(DPalette::TextTips);

    // 添加清理空间对话框布局
    dlg.addContent(titleLabel, Qt::AlignCenter);
    dlg.addSpacing(24);
    if (m_systemStatus != CheckResultType::PASSED) {
        dlg.addContent(frameDoSystemCleanup);
        dlg.addSpacing(4);
    }
    if (m_dataStatus != CheckResultType::PASSED) {
        dlg.addContent(frameDoDataCleanup);
        dlg.addSpacing(4);
    }
    dlg.addSpacing(6);
    dlg.addContent(frameBackupSystemOnly);
    dlg.addSpacing(10);
    dlg.addContent(tipLabel);

    dlg.setIcon(QIcon::fromTheme("dialog-warning"));
    if (dlg.exec() == DDialog::Accepted)
    {
        dbusWorker->m_isBackupApps = !backupSystemOnlyButton->isChecked();
        dbusWorker->CancelBackupApp(!dbusWorker->m_isBackupApps);
        emit dbusWorker->StartUpgradeCheck();
    }
}
