// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <DDialog>
#include <DFileDialog>
#include <DFloatingMessage>
#include <DMessageManager>

#include <QFileInfo>
#include <QMimeData>
#include <QPixmap>
#include <QButtonGroup>
#include <QStandardPaths>
#include <QStyle>

#include <iostream>

#include "../mainwindow.h"
#include "imagepreparationwidget.h"
#include "../application.h"
#include "../../core/dbusworker.h"
#include "../../widgets/alertdialog.h"

ImagePreparationWidget::ImagePreparationWidget(QWidget *parent)
    : BaseContainerWidget(parent, 1)
    , m_noNetworkMessage(new DFloatingMessage())
    , m_networkConnected(false)
    , m_stackedLayout(new QStackedLayout(this))
    , m_imageMethodWidget(new RetrieveImageMethodWidget(this))
    , m_checkImageWidget(new CheckImageWidget(this))
    , m_checkImageResultWidget(new CheckImageResultWidget(this))
    , m_checkNetworkThread(nullptr)
    , m_imageDownloadWidget(new ImageDownloadWidget(this))
    , m_errorMessageWidget(new ErrorMessageWidget(this))
    , m_softwareCheckProgressWidget(new SoftwareCheckProgressWidget(this))
    , m_softwareTableWidget(new SoftwareEvaluationWidget(this))
{
    setAcceptDrops(true);
    initUI();
    initConnections();

}

void ImagePreparationWidget::initUI()
{
    m_noNetworkMessage->setIcon(QIcon(":icons/p2v_warning_normal.svg"));
    m_noNetworkMessage->setMessage(tr("Failed to connect to the server. Please check your network connection or select a local V23 image file."));

    m_stackedLayout->addWidget(m_imageMethodWidget);
    m_stackedLayout->addWidget(m_checkImageWidget);
    m_stackedLayout->addWidget(m_checkImageResultWidget);
    m_stackedLayout->addWidget(m_imageDownloadWidget);
    m_stackedLayout->addWidget(m_errorMessageWidget);
    m_stackedLayout->addWidget(m_softwareCheckProgressWidget);
    m_stackedLayout->addWidget(m_softwareTableWidget);
    m_errorMessageWidget->setTitle(tr("Download failed"));

    // 显示底部按钮
    showAllButtons();

    m_spacerItem->changeSize(0, 0, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    m_contentLayout->addLayout(m_stackedLayout, 10);
}

void ImagePreparationWidget::initConnections()
{
    // 设置底部按钮操作
    connect(m_cancelButton, &DPushButton::clicked, this, [this] {
        if (m_stackedLayout->currentWidget() == m_imageDownloadWidget)
        {
            AlertDialog dlg(this, tr("Do you want to exit the downloading?"));
            if (dlg.exec() == DDialog::Accepted)
            {
                emit m_imageDownloadWidget->abort();
            }
        }
        else if (m_stackedLayout->currentWidget() == m_checkImageWidget)
        {
            AlertDialog dlg(this, tr("Do you want to exit the image file checking?"));
            if (dlg.exec() == DDialog::Accepted)
            {
                emit m_checkImageWidget->Cancel();
                jumpImageMethodWidget();
            }
        }
        else if (m_stackedLayout->currentWidget() == m_softwareCheckProgressWidget)
        {
            AlertDialog dlg(this, tr("Do you want to exit software evaluation?"));
            if (dlg.exec() == DDialog::Accepted)
            {
                DBusWorker::getInstance()->StopUpgrade();
                m_softwareTableWidget->clearAppInfos();
                emit m_checkImageWidget->CheckDone(true);
            }
        }
        else
        {
            MainWindow::getInstance()->close();
        }
    });
    connect(m_suggestButton, &DPushButton::clicked, this, [this] {
        if (m_stackedLayout->currentWidget() == m_imageMethodWidget)
        {
            if (m_imageMethodWidget->isLocalFileSelected())
            {
                // 提前设置按钮样式，避免覆盖后面报错设置报错界面按钮样式
                // 显示底部按钮
                setCancelButtonStyle();
                // 检查镜像完整度
                 m_stackedLayout->setCurrentWidget(m_checkImageWidget);
                 emit m_checkImageWidget->CheckImported(m_imageMethodWidget->getFileUrl());
            }
            else
            {
                if (m_networkConnected)
                {
                    // 提前设置按钮样式，避免覆盖后面报错设置报错界面按钮样式
                    setCancelButtonStyle();
                    SourceInfo info = DBusWorker::getInstance()->getSource();
                    m_stackedLayout->setCurrentWidget(m_imageDownloadWidget);
                    emit m_imageDownloadWidget->download(info.addr);
                }
                else
                {
                    showNoNetworkMessage();
                    checkNetwork();
                }
            }
        }
        // Todo(Yutao Meng): 按钮样式修改
        else if (m_stackedLayout->currentWidget() == m_checkImageResultWidget)
        {
            if (m_checkImageResultWidget->getResult())
            {
                emit DBusWorker::getInstance()->Assess(m_checkImageResultWidget->getResultIsoPath());
                m_stackedLayout->setCurrentWidget(m_softwareCheckProgressWidget);
                m_suggestButton->setVisible(false);
                m_cancelButton->setFixedSize(250, 36);
                m_cancelButton->setText(tr("Cancel"));
            }
            else
            {
                // 重新获取
                jumpImageMethodWidget();
            }
        }
        else if (m_stackedLayout->currentWidget() == m_softwareTableWidget)
        {
            // 笔记本电池状态要弹窗警告
            if (DBusWorker::getInstance()->IsOnBattery())
            {
                showPowerWarning();
            }
            else
            {
                // 去升级
                emit StartUpgrade(m_checkImageResultWidget->getResultIsoPath());
            }
        }
        else if (m_stackedLayout->currentWidget() == m_errorMessageWidget)
        {
            // 重新获取
            jumpImageMethodWidget();
        }
        else
        {
            qCritical() << "Fuck, what is widget " << m_stackedLayout->currentWidget()->metaObject()->className();
        }
    });
    connect(m_softwareCheckProgressWidget, &SoftwareCheckProgressWidget::CheckDone, [this] {
        m_softwareTableWidget->fillTable();
        m_stackedLayout->setCurrentWidget(m_softwareTableWidget);
        showAllButtons();
        m_suggestButton->setText(tr("Start Upgrade"));
    });
    // 连接子组件的信号
    connect(m_imageMethodWidget, &RetrieveImageMethodWidget::FileAdded, this, [this] () {
        activateWindow();
        m_suggestButton->setEnabled(true);
    });
    connect(m_imageMethodWidget, &RetrieveImageMethodWidget::FileClear, this, [this] () {
        if (m_imageMethodWidget->isLocalFileSelected())
            m_suggestButton->setEnabled(false);
    });
    connect(m_imageMethodWidget, &RetrieveImageMethodWidget::ToggleLocalButton, this, [this] (bool checked) {
        // 仅当本地镜像位置有配置或选择从网络获取时才启用“下一步”按钮
        if (checked)
        {
            if (m_imageMethodWidget->getFileUrl().length() > 0)
            {
                m_suggestButton->setEnabled(true);
            }
            else
            {
                m_suggestButton->setEnabled(false);
            }
        }
        else
        {
            m_suggestButton->setEnabled(true);
        }

    });
    connect(m_checkImageWidget, &CheckImageWidget::CheckDone, this, [this](bool passed) {
        emit m_checkImageResultWidget->SetResult(passed);
        if (m_imageMethodWidget->isLocalFileSelected())
        {
            emit m_checkImageResultWidget->ReadImageInfo(m_imageMethodWidget->getFileUrl());
        }
        else
        {
            emit m_checkImageResultWidget->ReadImageInfo(m_imageDownloadWidget->getFilePath());
        }
        m_stackedLayout->setCurrentWidget(m_checkImageResultWidget);
        m_spacerItem->changeSize(0, 0, QSizePolicy::Preferred, QSizePolicy::Preferred);

        showAllButtons();
        if (passed)
        {
            m_suggestButton->setText(tr("Next"));
        }
        else
        {
            m_suggestButton->setText(tr("Reselect"));
        }
    });
    connect(m_imageDownloadWidget, &ImageDownloadWidget::done, [this] {
        SourceInfo info = DBusWorker::getInstance()->getSource();
        QString checksumUrlStr = QUrl(info.addr).adjusted(QUrl::RemoveFilename).toString() + QString("SHA256SUMS");
        // 检查镜像完整度
        m_stackedLayout->setCurrentWidget(m_checkImageWidget);
        emit m_checkImageWidget->CheckDownloaded();
    });
    connect(m_imageDownloadWidget, &ImageDownloadWidget::error, [this](QNetworkReply::NetworkError err, QString errLog) {
        // 镜像选择界面忽略报错弹窗
        if (m_stackedLayout->currentWidget() == m_imageMethodWidget)
            return;

        if (err == QNetworkReply::OperationCanceledError)
        {
            // 忽略下载取消报错，直接跳转获取镜像方式界面
            jumpImageMethodWidget();
            m_suggestButton->setEnabled(true);
            return;
        }
        // 跳转下载错误界面
        qDebug() << "setting up error widget";
        showAllButtons();
        m_cancelButton->setFixedSize(120, 36);
        m_spacerItem->changeSize(0, 0);

        m_suggestButton->setText(tr("Reselect"));
        m_suggestButton->setVisible(true);

        QStringList errorList;
        errorList << tr("Errors:") << errLog;
        m_errorMessageWidget->setErrorLog(errorList.join(QString("\n")));
        m_stackedLayout->setCurrentWidget(m_errorMessageWidget);
        errorList << tr("Error code:") << QVariant::fromValue(err).toString();
        qCritical() << errorList.join(QString("\n"));
    });
    connect(DBusWorker::getInstance(), &DBusWorker::EvaluateSoftwareError, [this] (const QString errorLog) {
       m_cancelButton->setText(tr("Exit"));
       m_errorMessageWidget->setTitle(tr("Software evaluation failed"));
       m_errorMessageWidget->setErrorLog(errorLog);
       m_stackedLayout->setCurrentWidget(m_errorMessageWidget);
    });
}

void ImagePreparationWidget::checkNetwork()
{
    if (m_checkNetworkThread == nullptr || m_checkNetworkThread->isFinished())
    {
        m_checkNetworkThread = new CheckSizeThread(DBusWorker::getInstance()->getSource().addr);
        connect(m_checkNetworkThread, &CheckSizeThread::done, this, [this] {
            m_networkConnected = true;
        });
        connect(m_checkNetworkThread, &CheckSizeThread::error, this, [this] (QString errStr){
            qDebug() << "check network error message:" << errStr;
            m_networkConnected = false;
        });
        m_checkNetworkThread->start();
    }
}

void ImagePreparationWidget::showNoNetworkMessage()
{
    qCritical() << QString("cannot connect to %1").arg(DBusWorker::getInstance()->getSource().addr);
    m_noNetworkMessage->close();
    m_noNetworkMessage->show();
    DMessageManager::instance()->sendMessage(m_imageMethodWidget, m_noNetworkMessage);
}

void ImagePreparationWidget::setCancelButtonStyle()
{
    // 显示底部按钮
    showAllButtons();
    qDebug() << "setting up check or download widgets";
    m_cancelButton->setFixedSize(250, 36);
    m_cancelButton->setText(tr("Cancel"));
    m_suggestButton->setVisible(false);
}

void ImagePreparationWidget::showAllButtons()
{
    m_cancelButton->setVisible(true);
    m_cancelButton->setText(tr("Exit"));
    m_cancelButton->setFixedSize(120, 36);
    m_suggestButton->setVisible(true);
    m_suggestButton->setText(tr("Next"));
    m_suggestButton->setFixedSize(120, 36);
    m_buttonSpacerItem->changeSize(5, 0);
}

void ImagePreparationWidget::jumpImageMethodWidget()
{
    checkNetwork();
    showAllButtons();
    m_cancelButton->setFixedSize(120, 36);
    m_suggestButton->setEnabled(true);
    m_stackedLayout->setCurrentWidget(m_imageMethodWidget);
}

void ImagePreparationWidget::showPowerWarning() {
    DDialog dlg(this);
    MainWindow *window = MainWindow::getInstance();

    dlg.move(window->x() + (window->width() - dlg.width()) / 2, window->y() + (window->height() - dlg.height()) / 2);
    dlg.setTitle(tr("Your computer is not plugged in, please plug in a power cord first and then upgrade."));
    dlg.addButton(tr("OK"));
    dlg.setIcon(QIcon::fromTheme("dialog-warning"));
    dlg.exec();
}
