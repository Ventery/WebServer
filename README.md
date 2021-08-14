## 高性能文件浏览器

[![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://opensource.org/licenses/MIT) 
[![Build Status](https://api.travis-ci.com/Ventery/WebServer.svg?branch=master)](https://travis-ci.com/github/Ventery/WebServer)

本项目为lin ya的[Webserver](https://github.com/linyacool/WebServer)的修改版本，这是一个优秀的高性能服务器。基本上来说，源自陈硕[muduo](https://github.com/chenshuo/muduo)早期版本的高性能网络库，提供了应用接口，不仅仅可以支持http服务，应用部分与服务器部分耦合性极低，所以可以自由适应很多高并发的需求。它从功能上可以分为两部分，服务器与应用。本项目主要是对应用部分进行了功能扩充。
本人在研究和学习其代码后，做出一定修改。在原本的http服务器的基础上，增加了文件浏览器的功能，还增加了图片浏览、文本浏览，音频和视频浏览的功能（使用浏览器原生播放器）。根本目的还是对网络部分进行学习，应用部分代码有诸多不规范之处，未来版本需要重构，但造轮子要适可而止。

零、关于修改的部分。

-	网络部分修改比较少，关于原理和详细强烈建议看lin ya的[Webserver](https://github.com/linyacool/WebServer)的md文档介绍，我这里修改主要是HTTP应用部分，网络部分修改较少。
-	应用部分增加了对多种文件浏览的支持，为了支持视频文件解析了HTTP的分段功能，使用内存映射把文件映射到内存中（内存映射涉及缺页中断相关知识），所以同一个文件在内存中只会有一份，即使有多人同时访问。
-	因为网络部分支持高并发，所以支持多个用户同时浏览文件，后续会加入并发程度测试。
-	HTTP应用部分是完全的的测试版本，虽然能运行，实际上代码非常琐碎，变量也不规范，需要完全重构，出于时间成本的考虑先搁置。
-	最好只在局域网或者本地做实验使用，因为数据没有加密，文件路径甚至文件内容有泄漏的风险，公网虽然能用但是最好别用。


一、安装：

-	随便找个地方解压，然后修改WebServer/HttpData.cpp。
将HTML_DOMAIN修改为你自己的ip与port，ip为本地地址或者局域网地址均可，看你想从哪里访问。
将path_prefix与application_path_prefix1与application_path_prefix2修改为自己想默认访问的几个目录；

-	运行命令./build.sh。

-	运行命令
WebServer -t threadnum -p port -l log_path -r runmode；

threadnum为线程数，不填的话默认为4；
port为你在WebServer/HttpData.cpp中的port，不填默认为80；
log_path为你想生成log的地址，不填的话默认为当前目录；
runmode为必填可以为1或者2或者3；
1对应上面填的application_path_prefix1；
2对应application_path_prefix2；
3对应path_prefix。

二、运行：

浏览器访问http://192.168.2.111:39000/ （改成上面你自己的设置的ip和port）
如果目录中有可以浏览的文件会自动打开，另外也可以点击目录或者文件来实现切换目录和打开文件

1、视频，支持拖动进度条
![](https://github.com/Ventery/WebServer/blob/master/picture/%E6%B5%8F%E8%A7%88%E8%A7%86%E9%A2%91.png)


2、图片，支持放大与缩小
![](https://github.com/Ventery/WebServer/blob/master/picture/%E6%B5%8F%E8%A7%88%E5%9B%BE%E7%89%87.png)


3、文本浏览
![](https://github.com/Ventery/WebServer/blob/master/picture/%E6%B5%8F%E8%A7%88%E6%96%87%E6%9C%AC.png)


三、高并发性能测试

待补充



四、总结

作为我学习后台&服务器编程的第一个开源项目，获益颇丰，真的学习了很多，对陈硕和lin ya这两位大佬表示由衷的感谢。
最后引用一句话作为收尾：
> 优秀的工程师从不拒绝使用别人的代码，但自己始终保留造轮子的能力。
