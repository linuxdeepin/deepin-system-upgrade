## 背景

这是一款系统升级工具，与传统系统升级方式不同，我们使用的是根目录替换的方式进行升级，使系统升级更具有安全性、稳定性以及原子性。

## 依赖 

### 构建依赖 

* cmake

* golang-any

* golang-yaml.v2-dev

* golang-dbus-dev

* golang-github-linuxdeepin-go-lib-dev
* libdtkwidget-dev

* libdtkgui-dev

* qtbase5-dev

* qttools5-dev-tools



### 运行时依赖

* [deepin-upgrade-manager](https://github.com/linuxdeepin/deepin-upgrade-manager)

* plymouth-label

* fuseiso

* dpkg-repack

* squashfs-tools

  

## 安装

1.确保你已安装上述依赖

2.构建:

```
$ cd deepin-system-upgrade
$ mkdir build
$ cd build
$ cmake ..
$ make
```

3.安装:

```
$ sudo make install
```



## 用法

启动升级工具之后，将会出现图形界面，可根据提示逐步完成升级。

## 维护者

* [github](https://github.com/l631197874)

## 帮助

* [Matrix](https://matrix.to/#/#deepin-community:matrix.org)
* [WiKi](https://wiki.deepin.org)
* [官方论坛](https://bbs.deepin.org)
* [开发者中心](https://github.com/linuxdeepin/developer-center/issues) 

## 贡献指南

我们鼓励您报告问题并做出更改

* [Contribution guide for developers](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers-en). (English)
* [开发者代码贡献指南](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers) (中文)


## 开源许可证书

deepin-system-upgrade 在 [GPL-3.0-or-later](LICENSE) 下发布。

