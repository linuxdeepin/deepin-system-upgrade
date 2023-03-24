// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDebug>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QVersionNumber>

#include <thread>
#include <iostream>
#include <cstdio>

#include "checkimagewidget.h"
#include "isoinfochecker.h"
#include "../../core/utils.h"
#include "../../core/constants.h"
#include "../../core/dbusworker.h"


CheckImageWidget::CheckImageWidget(QWidget *parent)
    : QWidget(parent)
    , m_canceled(false)
    , m_mainLayout(new QVBoxLayout(this))
    , m_spinnerWidget(new SpinnerWidget(this))
    , m_editionNameChecker(nullptr)
    , m_versionChecker(nullptr)
    , m_integrityChecker(nullptr)
{
    initUI();
    initConnections();
}

void CheckImageWidget::initUI()
{
    m_spinnerWidget->setMinimumSize(100, 100);
    m_spinnerWidget->setMessage(tr("Verifying the integrity…"));
    m_mainLayout->addWidget(m_spinnerWidget, 0, Qt::AlignCenter);
}

void CheckImageWidget::initConnections()
{
    connect(this, &CheckImageWidget::Cancel, &CheckImageWidget::onCanceled);
    connect(this, &CheckImageWidget::CheckImported, this, &CheckImageWidget::onCheckImported);
    connect(this, &CheckImageWidget::EditionNameAvailable, &CheckImageWidget::onEditionNameAvailable);
    connect(this, &CheckImageWidget::VersionAvailable, &CheckImageWidget::onVersionAvailable);
    connect(this, &CheckImageWidget::IntegrityStatusAvailable, &CheckImageWidget::onIntegrityStatusAvailable);
    connect(this, &CheckImageWidget::CheckDownloaded, this, &CheckImageWidget::onCheckDownloaded);
    connect(this, &CheckImageWidget::CheckDone, this, [this](bool state) {
        qInfo() << "check sum result:" << state;
        m_spinnerWidget->stop();
        // Handle download failure
        if (!state)
        {
            SourceInfo info = DBusWorker::getInstance()->getSource();
            const QString isoName = QUrl(info.addr).fileName();
            QDir downloadDir = QDir::home();
            downloadDir.mkpath(kDownloadPath);
            downloadDir.cd(kDownloadPath);
            const QString downloadedIsoPath = downloadDir.filePath(isoName);

            if (downloadedIsoPath == m_isoPath)
            {
                QFile(m_isoPath).remove();
            }
        }
    });
}

void CheckImageWidget::onCanceled()
{
    m_canceled = true;
    m_spinnerWidget->stop();

    if (m_editionNameChecker != nullptr)
    {
        m_editionNameChecker->stop();
    }
    if (m_versionChecker != nullptr)
    {
        m_versionChecker->stop();
    }
    if (m_integrityChecker != nullptr)
    {
        m_integrityChecker->stop();
    }
}

void CheckImageWidget::onCheckImported(const QString path)
{
    m_canceled = false;
    m_isoPath = path;
    m_editionNameChecker = new IsoInfoChecker(this);

    m_spinnerWidget->start();
    connect(m_editionNameChecker, &IsoInfoChecker::Stdout, this, &CheckImageWidget::EditionNameAvailable);
    m_editionNameChecker->retrieveVersionValue(path, "EditionName");
}

void CheckImageWidget::onEditionNameAvailable(const QString editionName)
{
    if (m_canceled)
    {
        return;
    }
    if (m_editionNameChecker != nullptr)
    {
        m_editionNameChecker->deleteLater();
    }
    m_editionNameChecker = nullptr;

    FILE *curEditionNamePipe = popen("echo -n $(grep -Po '(?<=EditionName=).*$' /etc/os-version)", "r");
    char curEditionName[64] = "";

    fgets(curEditionName, 64, curEditionNamePipe);
    qDebug() << "ISO EditionName:" << editionName;
    qDebug() << "Current System EditionName:" << curEditionName;
    if (editionName == curEditionName)
    {
        m_versionChecker = new IsoInfoChecker(this);
        connect(m_versionChecker, &IsoInfoChecker::Stdout, this, &CheckImageWidget::VersionAvailable);
        m_versionChecker->retrieveVersionValue(m_isoPath, "MajorVersion");
    }
    else
    {
        emit CheckDone(false);
    }
}

void CheckImageWidget::onVersionAvailable(const QString versionText)
{
    if (m_canceled)
    {
        return;
    }

    if (m_versionChecker != nullptr)
    {
        m_versionChecker->deleteLater();
    }
    m_versionChecker = nullptr;

    qDebug() << "Version:" << versionText;
    qDebug() << "VersionNumber" << QVersionNumber::fromString(versionText);
    if (QVersionNumber::fromString(versionText) >= kDeepinTargetVersion)
    {
        m_integrityChecker = new IsoInfoChecker(this);
        connect(m_integrityChecker, &IsoInfoChecker::ExitStatus, this, &CheckImageWidget::IntegrityStatusAvailable);
        m_integrityChecker->integrityCheck(m_isoPath);
    }
    else
    {
        emit CheckDone(false);
    }
}

void CheckImageWidget::onIntegrityStatusAvailable(int status)
{
    if (m_canceled)
    {
        return;
    }

    if (m_integrityChecker != nullptr)
    {
        m_integrityChecker->deleteLater();
    }
    m_integrityChecker = nullptr;
    emit CheckDone(0 == status);
}

void CheckImageWidget::onCheckDownloaded()
{
    m_canceled = false;
    m_spinnerWidget->start();
    SourceInfo info = DBusWorker::getInstance()->getSource();
    const QString isoName = QUrl(info.addr).fileName();
    QDir downloadDir = QDir::home();
    downloadDir.mkpath(kDownloadPath);
    downloadDir.cd(kDownloadPath);
    const QString isoPath = downloadDir.filePath(isoName);
    emit CheckImported(isoPath);
}
