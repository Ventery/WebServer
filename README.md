## 高性能文件浏览器

[![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://opensource.org/licenses/MIT) 
[![Build Status](https://api.travis-ci.com/Ventery/WebServer.svg?branch=master)](https://travis-ci.com/github/Ventery/WebServer)

&nbsp; &nbsp; &nbsp; 本项目为lin ya的[Webserver](https://github.com/linyacool/WebServer)的修改版本，这是一个优秀的高性能服务器。基本上来说，源自陈硕[muduo](https://github.com/chenshuo/muduo)早期版本的高性能网络库，提供了应用接口，不仅仅可以支持http服务，因为应用部分与服务器部分耦合性极低，所以可以自由适应很多高并发的需求。它从功能上可以分为两部分，服务器与应用。本项目主要是对应用部分进行了功能扩充。

&nbsp; &nbsp; &nbsp; 本人在研究和学习其代码后，做出一定修改。在原本的http服务器的基础上，增加了文件浏览器的功能，还增加了图片浏览、文本浏览，音频和视频浏览的功能（使用浏览器原生播放器）。根本目的还是对网络部分进行学习，应用部分代码有诸多不规范之处，未来版本需要重构。

&nbsp; &nbsp; &nbsp;[点击查看运行效果。](https://github.com/Ventery/WebServer#%E4%BA%8C%E8%BF%90%E8%A1%8C)

### 零、关于修改的部分。

-	网络部分修改比较少，关于原理和详细强烈建议看lin ya的[Webserver](https://github.com/linyacool/WebServer)的md文档介绍，我这里修改主要是HTTP应用部分，虽然网络部分修改较少，但是关键部分都学到了。
-	应用部分增加了对多种文件浏览的支持，为了支持视频文件解析了HTTP的分段功能，使用内存映射把文件映射到内存中（内存映射涉及缺页中断相关知识），所以同一个文件在内存中只会有一份，即使有多人同时访问。
-	因为网络部分支持高并发，所以支持多个用户同时浏览文件，后续会加入并发程度测试。
-	HTTP应用部分是完全的的测试版本，虽然能运行，实际上代码非常琐碎，变量也不规范，需要完全重构，出于时间成本的考虑先搁置。
-	最好只在局域网或者本地做实验使用，因为数据没有加密，文件路径甚至文件内容有泄漏的风险，公网虽然能用但是最好别用。


### 一、安装：

-	解压，然后修改WebServer/HttpData.cpp。
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

### 二、运行：

浏览器访问http://192.168.2.111:39000/ （改成上面你自己的设置的ip和port）
如果目录中有可以浏览的文件会自动打开，另外也可以点击目录或者文件来实现切换目录和打开文件

1、视频，解析了HTTP的分段请求，支持拖动进度条
![](https://github.com/Ventery/WebServer/blob/master/picture/%E6%B5%8F%E8%A7%88%E8%A7%86%E9%A2%91.png)


2、图片，加入了一些前端代码，支持放大与缩小
![](https://github.com/Ventery/WebServer/blob/master/picture/%E6%B5%8F%E8%A7%88%E5%9B%BE%E7%89%87.png)


3、简单的文本浏览
![](https://github.com/Ventery/WebServer/blob/master/picture/%E6%B5%8F%E8%A7%88%E6%96%87%E6%9C%AC.png)


### 三、高并发性能测试

几点说明：
-	使用了修改过后webbench，可以支持长连接，可以对文件传输进行测试，buff为1500字节，再大一点的文件就收不了了，内存也不够。
-	WebServer和webbench均在本机运行，为什么在本机运行？因为本地的网络环境不支持，有线为五类线只有100M，无线只有家庭宽带配的无线路由，实测更差。两个程序同在一台机器会对测试有一定影响。
- 如果想要极限测试一定要关闭LOG输出，因为LOG输出里面涉及大量系统调用。本人在开启LOG后再测试，QPS居然少了5倍有余，可能之前使用LOG地方太多了。不是说LOG不能用，要只在必要的地方用。
- 
配置：
![](https://github.com/Ventery/WebServer/blob/master/picture/CPU%E5%9E%8B%E5%8F%B7%E3%80%81%E6%A0%B8%E5%BF%83%E6%95%B0%E4%BB%A5%E5%8F%8A%E5%86%85%E5%AD%98%E5%AE%B9%E9%87%8F.png)
可以看到逻辑核心为12核心，因为LOG还要1个线程，所以凑个整我们只开10个线程。

启动命令，为10线程：
![](https://github.com/Ventery/WebServer/blob/master/picture/%E5%90%AF%E5%8A%A8%E5%91%BD%E4%BB%A4.png)

首先是短连接和长连接的测试，测试目标为内存数据，这是为了减少磁盘io带来的开销专注于对request的反馈。
短连接：
![](https://github.com/Ventery/WebServer/blob/master/picture/%E7%9F%AD%E8%BF%9E%E6%8E%A5.png)
短连接时候CPU占用：
![](https://github.com/Ventery/WebServer/blob/master/picture/%E7%9F%AD%E8%BF%9E%E6%8E%A5%E6%97%B6CPU%E5%8D%A0%E7%94%A8.png)
可以看出主线程CPU占用率很高，因为频繁的连接建立和连接分配都在主线程。子线程主要是专注于数据读取和解析以及写入，比连接的建立和分配快，就比较闲一点了。


长连接：
![](https://github.com/Ventery/WebServer/blob/master/picture/%E9%95%BF%E8%BF%9E%E6%8E%A5.png)
可以看出跟短连接比起来效率提升了非常多，四倍有余。

长连接时候CPU占用：
![](https://github.com/Ventery/WebServer/blob/master/picture/%E9%95%BF%E8%BF%9E%E6%8E%A5%E6%97%B6CPU%E5%8D%A0%E7%94%A8.png)
主线程只在开始建立和分配连接时候占用一点资源，之后都是子线程对连接进行操作，主线程几乎没有CPU占用，在最下面没截到。子线程CPU利用率比短连接高多了，但离占满仍然有一定距离，这是因为webbench也在本机上运行，因为是长连接，webbench也有相当的时间用在io上所以占用了不少CPU。

长连接访问文件：
![](https://github.com/Ventery/WebServer/blob/master/picture/%E9%95%BF%E8%BF%9E%E6%8E%A5%E8%AE%BF%E9%97%AE%E6%96%87%E4%BB%B6.png)

其实跟上面的长连接相比，只是测试目标大了一点而已（文件是映射到内存里的），而且只能接受1500K，再大了内存就不足了。但是因为一个HTTP包变大了，所以数据传输量变大，request处理量差不多。这个参考意义不大。


### 四、总结

&nbsp; &nbsp; &nbsp; 作为我学习后台以及服务器编程的第一个开源项目，获益颇丰，真的学习了很多，对陈硕和lin ya这两位大佬表示由衷的感谢。
&nbsp; &nbsp; &nbsp; 最后引用一句话作为收尾：
> 优秀的工程师从不拒绝使用别人的代码，但自己始终保留造轮子的能力。


&nbsp; &nbsp; &nbsp; 与各位共勉。
