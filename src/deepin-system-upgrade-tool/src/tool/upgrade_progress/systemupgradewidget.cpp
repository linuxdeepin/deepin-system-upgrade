// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <QDesktopWidget>

#include "../mainwindow.h"
#include "systemupgradewidget.h"

SystemUpgradeWidget::SystemUpgradeWidget(QWidget *parent)
    : BaseContainerWidget(parent, 2)
    , m_dbusWorker(DBusWorker::getInstance(this))
    , m_stackedLayout(new QStackedLayout(this))
    , m_upgradeWidget(new UpgradeWidget(this))
    , m_backupWidget(new BackupWidget(this))
    , m_restorationWidget(new RestorationWidget(this))
    , m_errorMessageWidget(new ErrorMessageWidget(this))
{
    initUI();
    initConnections();
}

void SystemUpgradeWidget::initUI()
{
    m_spacerItem->changeSize(0, 0);
    m_stackedLayout->addWidget(m_upgradeWidget);
    m_stackedLayout->addWidget(m_backupWidget);
    m_stackedLayout->addWidget(m_restorationWidget);
    m_stackedLayout->addWidget(m_errorMessageWidget);
    m_contentLayout->addLayout(m_stackedLayout);
}

void SystemUpgradeWidget::initConnections()
{
    connect(this, &SystemUpgradeWidget::start, m_upgradeWidget, &UpgradeWidget::start);
    connect(m_cancelButton, &QPushButton::clicked, [] {
        MainWindow::getInstance()->close();
    });
    connect(m_suggestButton, &QPushButton::clicked, [this] {
        if (m_errorMessageWidget->title() == tr("Backup failed")) {
            // 重新备份
            jumpToBackupWidget();
        } else {
            // 升级失败反馈
            QDesktopServices::openUrl(QUrl("https://bbs.deepin.org/"));
        }
    });
    connect(m_upgradeWidget, &UpgradeWidget::done, this, &SystemUpgradeWidget::jumpToBackupWidget);
    connect(m_backupWidget, &BackupWidget::done, [this] {
        // 备份好后，开始升级
        emit m_dbusWorker->StartUpgrade();
    });
    // DBus报错跳转
    connect(m_dbusWorker, &DBusWorker::error, [this] (QString errorTitle, QString errorLog) {
        if (DBusWorker::getInstance()->getUpgradeStage() == UpgradeStage::INIT)
        {
            // 不在系统升级流程，只是被其他复用，就不走还原流程
            setupErrorWidget(errorTitle, errorLog);
            jumpErrorWidget();
        }
        else if (errorTitle == tr("Restoration failed"))
        {
            // 还原失败，直接展示报错信息
            setupErrorWidget(errorTitle, errorLog);
            jumpErrorWidget();
        }
        else
        {
            m_stackedLayout->setCurrentWidget(m_restorationWidget);
            emit m_restorationWidget->start();
            // 准备报错界面，以便系统还原完成时直接跳转
            setupErrorWidget(errorTitle, errorLog);
        }
    });
    connect(m_restorationWidget, &RestorationWidget::done, this, &SystemUpgradeWidget::jumpErrorWidget);
}

void SystemUpgradeWidget::jumpToBackupWidget()
{
    m_cancelButton->setVisible(false);
    m_suggestButton->setVisible(false);
    m_stackedLayout->setCurrentWidget(m_backupWidget);
    emit m_backupWidget->start();
}

void SystemUpgradeWidget::setupErrorWidget(QString errorTitle, QString errorLog)
{
    if (errorTitle == tr("Backup failed"))
    {
        m_cancelButton->setFixedSize(120, 36);
        m_cancelButton->setText(tr("Exit"));

        m_suggestButton->setFixedSize(120, 36);
        m_suggestButton->setText(tr("Try Again"));
    }
    else
    {
        m_cancelButton->setFixedSize(120, 36);
        m_cancelButton->setText(tr("Exit"));

        m_suggestButton->setFixedSize(120, 36);
        m_suggestButton->setText(tr("Submit Feedback"));
    }
    m_errorMessageWidget->setTitle(errorTitle);
    m_errorMessageWidget->setErrorLog(errorLog);
}

void SystemUpgradeWidget::jumpErrorWidget()
{
    // // 从全屏中恢复窗口设置
    // MainWindow *w = MainWindow::getInstance();
    // w->setFixedSize(MAIN_WINDOW_W, MAIN_WINDOW_H);
    // w->setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    // w->showNormal();

    m_cancelButton->setVisible(true);
    m_suggestButton->setVisible(true);
    m_stackedLayout->setCurrentWidget(m_errorMessageWidget);
}
