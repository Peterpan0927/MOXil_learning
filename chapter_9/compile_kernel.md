---
title: 在High Serria上编译XNU内核
date: 2018-06-05 18:30:38
tags: 漏洞挖掘
categories: 内核
---


在进行MOXil学习的时候，第九章我们需要通过编译内核来对XNU有进一步的了解，那么如何在最近的系统上编译XNU呢？今天就一起踩坑吧

<!--more-->

# 资源工作

这里我们的内核版本和系统对应，首先看看是什么系统：

![系统版本](http://omunhj2f1.bkt.clouddn.com/%E5%B1%8F%E5%B9%95%E5%BF%AB%E7%85%A7%202018-06-05%20%E4%B8%8B%E5%8D%886.33.03.png)

那么我们从[苹果开源代码](https://opensource.apple.com/)网站上找到自己版本对应的XNU内核，对于我们来说，对应的是`xnu-4570.20.62`

如果下载了内核源码之后直接编译你就会遇到一大堆的`error`然后痛苦的`go die`，这是因为缺少很多头文件，库等等，首先我们需要安装`Xcode`的`Command line Tool`，这会帮我们解决很多麻烦，然后我们需要下载几个包，为了简便起见，我们直接通过脚本来做：

```shell
vim download.sh
```

脚本的内容为：

```shell
wget https://opensource.apple.com/tarballs/libplatform/libplatform-126.1.2.tar.gz
wget https://opensource.apple.com/tarballs/libdispatch/libdispatch-703.1.4.tar.gz
wget https://opensource.apple.com/tarballs/xnu/xnu-4570.20.62.tar.gz
wget https://opensource.apple.com/tarballs/dtrace/dtrace-262.tar.gz
wget https://opensource.apple.com/tarballs/AvailabilityVersions/AvailabilityVersions-32.tar.gz
```

这样我们的下载过程就完成，接下来就是解压然后逐个编译，并配置好环境



# 环境准备

## 编译

### AvailabilityVersions

```shell
$ mkdir -p dst
$ make install SRCROOT=$PWD DSTROOT=$PWD/dst
$ sudo ditto $PWD/dst/usr/local `xcrun -sdk macosx -show-sdk-path`/usr/local
```

### dtrace

只需要编译 **dtrace** 项目中的 **ctfconvert**、**ctfdump**、**ctfmerge**。

```shell
$ mkdir -p obj sym dst
$ xcodebuild install -target ctfconvert -target ctfdump -target ctfmerge ARCHS="x86_64" SRCROOT=$PWD OBJROOT=$PWD/obj SYMROOT=$PWD/sym DSTROOT=$PWD/dst
$ sudo ditto $PWD/dst/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain
```

### XNU头文件

在编译 **libdispatch** 之前，需要将XNU的头文件复制到SDK文件夹下，因为编译 **libdispatch** 时需要用到这些头文件。

```shell
$ mkdir -p BUILD.h/obj BUILD.h/sym BUILD.h/dst
$ make installhdrs SDKROOT=macosx ARCH_CONFIGS=X86_64 SRCROOT=$PWD OBJROOT=$PWD/BUILD.h/obj SYMROOT=$PWD/BUILD.h/sym DSTROOT=$PWD/BUILD.h/dst
$ sudo xcodebuild installhdrs -project libsyscall/Libsyscall.xcodeproj -sdk macosx ARCHS="x86_64" SRCROOT=$PWD/libsyscall OBJROOT=$PWD/BUILD.h/obj SYMROOT=$PWD/BUILD.h/sym DSTROOT=$PWD/BUILD.h/dst
$ sudo ditto BUILD.h/dst `xcrun -sdk macosx -show-sdk-path`
```

**注**：**xnu-4570.1.46** 版本的内核源码因为 **缺失thread_self_restrict.h头文件**，所以此处必定会编译失败。
需要在 **xcodebuild** 前先创建一个空的文件来使编译通过。

```shell
$ touch libsyscall/os/thread_self_restrict.h
```

### libplatform

编译 **libdispatch** 时同样需要用到 **libplatform** 的头文件。

```shell
$ sudo ditto $PWD/include `xcrun -sdk macosx -show-sdk-path`/usr/local/include
```

### libdispatch

```shell
$ mkdir -p obj sym dst
$ sudo xcodebuild install -project libdispatch.xcodeproj -target libfirehose_kernel -sdk macosx ARCHS="x86_64" SRCROOT=$PWD OBJROOT=$PWD/obj SYMROOT=$PWD/sym DSTROOT=$PWD/dst
$ sudo ditto $PWD/dst/usr/local `xcrun -sdk macosx -show-sdk-path`/usr/local
```

### XNU

```Shell
# KERNEL_CONFIGS: RELEASE / DEVELOPMENT / DEBUG
$ make SDKROOT=macosx ARCH_CONFIGS=X86_64 KERNEL_CONFIGS="RELEASE"
```

编译成功后，即可在BUILD/obj目录下找到内核文件。

```Shell
# KERNEL_CONFIGS: RELEASE
$ cd BUILD/obj/RELEASE_X86_64
$ file kernel
kernel: Mach-O 64-bit executable x86_64

# KERNEL_CONFIGS: DEVELOPMENT
$ cd BUILD/obj/DEVELOPMENT_X86_64
$ file kernel.development
kernel: Mach-O 64-bit executable x86_64

# KERNEL_CONFIGS: DEBUG
$ cd BUILD/obj/DEBUG_X86_64
$ file kernel.debug
kernel: Mach-O 64-bit executable x86_64
```

最后可以看到我们编译成功的图：

![编译好的内核](http://omunhj2f1.bkt.clouddn.com/%E5%B1%8F%E5%B9%95%E5%BF%AB%E7%85%A7%202018-06-05%20%E4%B8%8B%E5%8D%886.44.33.png)

最后要注意的是编译内核对于你的系统版本之类的有要求，比如上述的教程在更老的系统可能就不会成功，甚至在同为`High Serria`的其他小版本上也不会成功，我只是没有想到很坑的一点是我用的`Serria`的GCD和平台库函数加上`10.13.1`的内核最后东瓶西凑成功的。

一开始总是报一个没有`<internal/atomic.h>`的错，我本来想要创建一个解决，但是GCD中有很多依赖这个头文件的数据类型，所以我通过google找到这个出自于`libplatform-125`，于是乎我就在源代码逐个筛选，最后成功。如果你想体验这个过程，就全部使用`10.13.1`的`libplatform`和`libdispatch`，你就会有不一样的体会。

