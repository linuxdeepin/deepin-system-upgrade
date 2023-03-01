// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QVBoxLayout>
#include <QWidget>

#include "../../widgets/spinnerwidget.h"
#include "isoinfochecker.h"

class CheckImageWidget: public QWidget
{
    Q_OBJECT
public:
    CheckImageWidget(QWidget *parent = nullptr);

Q_SIGNALS:
    void Cancel();
    // 检查本地导入的镜像是否和当前系统为一样的系统类型。当前仅检查是否为社区版V23镜像。
    void CheckImported(const QString path);
    // 本地导入检查的几个阶段完成信号
    void EditionNameAvailable(const QString editionName);
    void VersionAvailable(const QString version);
    void IntegrityStatusAvailable(int status);
    // 检查下载好的镜像文件。如不通过就删掉以便重新下载。
    void CheckDownloaded();
    void CheckDone(bool state);

private Q_SLOTS:
    void onCanceled();
    void onCheckImported(const QString path);
    void onEditionNameAvailable(const QString editionName);
    void onVersionAvailable(const QString version);
    void onIntegrityStatusAvailable(int status);
    void onCheckDownloaded();

private:
    bool            m_canceled;
    QString         m_isoPath;
    QVBoxLayout     *m_mainLayout;
    SpinnerWidget   *m_spinnerWidget;
    IsoInfoChecker  *m_editionNameChecker;
    IsoInfoChecker  *m_versionChecker;
    IsoInfoChecker  *m_integrityChecker;

    void initUI();
    void initConnections();

};
