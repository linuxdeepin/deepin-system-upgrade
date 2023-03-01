// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>
#include <QPair>
#include <QLocale>
#include <QVersionNumber>

//#include <string>
//#include <vector>

#define MAIN_WINDOW_W 750
#define MAIN_WINDOW_H 550
//#define MAIN_WINDOW_W 700
//#define MAIN_WINDOW_H 500
#define SOFTWARE_TABLE_ROW_H 40

enum class Orientation {
    Horizontal,
    Vertical
};

/**
 * @brief 协议类型
 */
enum class LicenseType {
    USER,       // 最终用户许可协议
    PRIVACY,    // 隐私保护指引
};

/**
 * @brief 系统升级检查结果
 */
enum class CheckResultType {
    PASSED,
    WARNING,
    FAILED,
};

/**
 * @brief 系统升级阶段
 *
 */
enum class UpgradeStage {
    INIT,              // 还没开始升级，用于展示重启升级后的升级失败错误
    EVALUATE_SOFTWARE, // 软件评估
    PREPARATION,       // 升级准备
    BACKUP,            // 系统备份
    RESTORATION,       // 系统还原
    MIGRATION,         // 应用迁移
};

// QDBus 异步调用超时 单位: ms
const int kQDBusAsyncCallTimeout = 5 * 3600000;
// 下载读取缓冲区大小 单位: Byte
const int kReadBufferSize = 512 * 1024;
// 下载重试间隔 单位：ms
const int kRetryInterval = 5000;
// 下载重连超时 单位：s
const int kReconnectTimeout = 300;

// 下载存放位置
const QString kDownloadPath(".cache/deepin-system-upgrade-tool");
// 升级状态文件位置
const QString kUpgradeStatesPath("/etc/deepin-upgrade-manager/state.records");

const QVersionNumber kUosMinimalVersion = QVersionNumber::fromString("1060");
const QVersionNumber kDeepinMinimalVersion = QVersionNumber::fromString("20.8");
const QVersionNumber kDeepinTargetVersion = QVersionNumber::fromString("23");
const QString kCurrentLocale = QLocale::system().name();
