![img](/biki.png)

## 欢迎来到Biki主页<br>

### 目录

- 前言
- 开发平台
- 库函数说明

### 1. 前言
- 该主页主要介绍机器鱼视觉部分，方便大家进行二次开发<br>

### 2. 开发平台
- 该款机器鱼硬件使用Nanopi-S2，软件使用opencv2.10进行开发<br>
- 处理流程：Nanopi-S2处理视觉数据后，发送给STM32进行鱼尾的控制，从而实现自主避障，在调试模式下，Nanopi可将调试信息发送至PC调试端<br>
- 这里附上Nanopi-S2的wiki链接[Nanopi-S2](http://wiki.friendlyarm.com/wiki/index.php/NanoPi_S2/zh)方便大家查找问题

### 3. 库函数说明
- 在视觉部分，已经提供一些可以直接使用的接口，这里说明一下如何使用
