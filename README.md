# 易寻(EveryLauncher)

![img](https://gitee.com/wanywhn/everyLauncher/raw/convert-to-cxx/demo/overview.gif)

# 简介
易寻项目初期致力于实现Linux下的Spotlight，并添加全文索引，可以针对各种类型的文件实现内容索引。

初始版本基于Recoll，便于项目快速开发。但随着项目的开始逐渐发现了一些不足。如索引文件有些大，外部整合不好。后期在针对Recoll的基础Xapian进行修改使用。

# 已基础实现
- [x] 地图集成
- [x] 设置启用禁用搜索源
- [x] se:** 搜索引擎快速访问
- [x] 索引应用程序

# 基础操作
- 默认Super(Win)+Alt+Space唤起程序
- Esc快速清空文本框
- 首次回车进行搜索，第二次回车触发所选项目动作（打开程序之类）
- 输入'se:bd 搜索内容','se:gg 搜索内容'进行快速搜索，具体在程序设置中可以添加或者修改

# TODO
- [ ] OCR切换至tesseract,额外训练tesseract
- [ ] 添加天气支持
- [ ] 自然语义理解
- [ ] 前缀限制检索范围
- [ ] 程序大小统计
create,modified,open
- [ ] 图片预览机制优化
- [ ] 百度、维基直接展示概要
- [ ] MP3现实内置图片
	meta data:created,modified,duration,sample Rate. icon with play btn
- [ ] MP4现实内部片段总览
- [ ] 检索未安装的程序
- [ ] 实现SectionView
- [ ] PPT Preview?
- [ ] Music Search?


- [ ] 地图当前位置
- [ ] 跳跃分组栏选择
- [ ] 设置分组搜索显示优先级
- [ ] 搜索选项

kind:application
kind:applications
kind:app

kind:folder

kind:pdf

kind:doc

kind:txt

kind:image

kind:audio

kind:video

date:today

date:yesterday

se(search engine):bd|bi|gg|so|zh
