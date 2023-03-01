// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDir>
#include <QByteArray>
#include <QNetworkRequest>
#include <QNetworkConfiguration>

#include <iostream>
#include <cstdio>

#include "../../core/constants.h"
#include "filedownloader.h"

FileDownloader::FileDownloader(QObject *parent): QObject(parent)
{
    connect(&m_retryTimer, &QTimer::timeout, this, &FileDownloader::retryDownload);
}

void FileDownloader::startDownload(QString urlStr)
{
    // 检查URL合法性
    if (!QUrl(urlStr).isValid())
    {
        m_errCode = QNetworkReply::NoError;
        m_errStr = QString(tr("URL \"%1\" is illegal").arg(urlStr));
        emit error();
        return;
    }

    m_url = QUrl(urlStr);
    if (m_checkSizeThread != nullptr)
    {
        disconnect(m_checkSizeThread);
    }
    m_checkSizeThread = new CheckSizeThread(m_url.toString());
    connect(m_checkSizeThread, &CheckSizeThread::done, this, &FileDownloader::onSizeAvailable);
    connect(m_checkSizeThread, &CheckSizeThread::error, this, [=](const QString errMsg) {
        m_errCode = QNetworkReply::NoError;
        m_errStr = errMsg;
        emit error();
    });
    m_checkSizeThread->start();
}

void FileDownloader::initConnections(QNetworkReply *reply)
{
    connect(reply, &QNetworkReply::downloadProgress, [this] (qint64 bytesReceived, qint64 bytesTotal) {
        m_totalSize = m_curSize + bytesTotal;
        Q_EMIT progress((bytesReceived + m_curSize) * 100 / m_totalSize);
    });
    connect(reply, &QNetworkReply::readyRead, this, [=] {;
            // 仅在下载正常时操作
            if (reply->error() == QNetworkReply::NoError)
{
    const QByteArray a = reply->readAll();

        if (m_file != nullptr && m_file->isOpen())
            m_file->write(a);
    }
                                                        });
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (m_file && m_file->isOpen())
        {
            m_file->close();
            m_file->deleteLater();
            m_file = nullptr;
        }
    });
    connect(reply, &QNetworkReply::finished, [this, reply] {
        QNetworkReply::NetworkError err = reply->error();
        if (err == QNetworkReply::NoError)
        {
            Q_EMIT done();
        }
        else if (err == QNetworkReply::NetworkSessionFailedError)
        {
            // 处理网络断连错误
            qCritical() << "QNetworkReply::NetworkSessionFailedError received";
            qCritical() << "error message" << reply->errorString();
            m_errCode = reply->error();
            m_errStr = reply->errorString();
            this->abort();

            m_disconnectedTime = std::time(nullptr);

            // 启动重试定时器。
            m_retryTimer.start(kRetryInterval);
            return;
        }
        else
        {
            m_errCode = reply->error();
            m_errStr = reply->errorString();
            reply->abort();
            reply->deleteLater();
            qCritical() << "Download error message:";
            qCritical() << "error code:" << m_errCode;
            qCritical() << "error message:" << m_errStr;
            Q_EMIT error();
        }

    });
}

void FileDownloader::abort()
{
    if (m_reply)
    {
        qDebug() << "isRunning:" << m_reply->isRunning();
        qDebug() << "isFinished:" << m_reply->isFinished();

        if (m_reply->isRunning())
        {
            m_reply->abort();
        }

        disconnect(m_reply);
        m_reply->deleteLater();
        m_reply = nullptr;
    }
    else
    {
        // 处理当check size都还没完成时的取消动作。需要触发error并设置成QNetworkReply::OperationCanceledError
        m_errCode = QNetworkReply::OperationCanceledError;
        emit error();
    }
}

void FileDownloader::onSizeAvailable(long long size)
{
    // 下载开始时停止重试计时器
    m_retryTimer.stop();

    // 更新镜像大小并启动下载
    m_totalSize = size;

    download();
}

void FileDownloader::download()
{
    // 如果已经正在下载（已经开始写文件了），就跳过
    if (m_file && m_file->isOpen())
        return;

    QDir downloadDir = QDir::home();
    downloadDir.mkpath(kDownloadPath);
    downloadDir.cd(kDownloadPath);


    QString filename = getFileName();

    m_file = new QFile(downloadDir.absoluteFilePath(filename));

    if (QFile::exists(downloadDir.absoluteFilePath(filename)))
    {
        QNetworkRequest req(m_url);
        m_curSize = m_file->size();
        req.setRawHeader(QByteArray("Range"), "bytes=" + QByteArray::number(m_curSize) + "-");
        // 这里仅做大小检查，靠完整性检查确保镜像完好。
        if (m_curSize >= m_totalSize)
        {
            emit done();
            return;
        }

        m_reply = m_netMgr.get(req);
        if (m_reply->error() != QNetworkReply::NoError)
        {
            emit error();
            return;
        }

        if (m_file->open(QFile::WriteOnly | QFile::Append))
        {
            initConnections(m_reply);
        }
        else
        {
            qCritical() << QString("ISO File: %1 cannot be opened as \"Append & Write\"").arg(m_url.toString());
        }
    }
    else
    {
        m_curSize = 0;
        m_reply = m_netMgr.get(QNetworkRequest(m_url));
        if (m_reply->error() != QNetworkReply::NoError)
        {
            emit error();
            return;
        }

        if (m_file->open(QFile::WriteOnly))
        {
            initConnections(m_reply);
        }
    }
    m_reply->setReadBufferSize(kReadBufferSize);
}

void FileDownloader::retryDownload()
{
    qCritical() << "重试下载，距离上次断连时间：" << std::time(nullptr) - m_disconnectedTime;
    // 若超过重连等待时间，就直接报错
    if (std::time(nullptr) - m_disconnectedTime > kReconnectTimeout)
    {
        m_retryTimer.stop();
        emit error();
        return;
    }

    // 检查网络连通性，并在连上后启动下载
    if (m_checkSizeThread != nullptr)
    {
        disconnect(m_checkSizeThread);
        m_checkSizeThread->deleteLater();
    }
    m_checkSizeThread = new CheckSizeThread(m_url.toString());
    connect(m_checkSizeThread, &CheckSizeThread::done, this, &FileDownloader::onSizeAvailable);
    m_checkSizeThread->start();
}
