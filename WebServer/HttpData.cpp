
#include "HttpData.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include "Channel.h"
#include "EventLoop.h"
#include "Util.h"
#include "time.h"
#include <dirent.h>
#include <ctime>
#include <cstdlib>       
#include <sys/types.h>
#include <algorithm>
#include "base/MutexLock.h"
#include <sys/stat.h>



pthread_once_t MimeType::once_control = PTHREAD_ONCE_INIT;
std::unordered_map<std::string, std::string> MimeType::mime;

const __uint32_t DEFAULT_EVENT = EPOLLIN | EPOLLET | EPOLLONESHOT;
const int DEFAULT_EXPIRED_TIME = 2000;              // ms
const int DEFAULT_KEEP_ALIVE_TIME = 15 * 1000;  // ms
const long RESPOND_LENGTH = 20*1024*1024;   //bytes
const int CACHE_CONTROL = 60*30;   //seconds

const std::string HTML_DOMAIN="http://192.168.2.111:39000/";
const std::string HTML_HEAD="<!DOCTYPE html><html><head><meta charset=\"utf-8\">";
const std::string HTML_TAIL="</body></html>";

const std::string VIDEO_TITLE1="<title>选择一个视频播放</title></head><body><h3>  选择一个视频播放  <a href=\""+HTML_DOMAIN+"index.html?";
const std::string VIDEO_TITLE2="\" class=\"button\">  随机  </a><a href=\""+HTML_DOMAIN+"index.html?";
const std::string VIDEO_TITLE3="\" class=\"button\">  上层目录  </a>";

const std::string HTML_VIDEO_HEAD="<div align=\"center\" ><video id=\"shipin\" width=80%  controls autoplay><source src=\"";
const std::string HTML_VIDEO_TAIL="\" type=\"video/mp4\"></object></video></div>\n<script>document.getElementById(\"shipin\").volume = 0.3;</script><br>";

const std::string PIC_TITLE1="<title>浏览图片</title></head><h3>  浏览图片  <a href=\""+HTML_DOMAIN+"index.html?";
const std::string PIC_TITLE2="\" class=\"button\">  上一张  </a><a href=\""+HTML_DOMAIN+"index.html?";
const std::string PIC_TITLE3="\" class=\"button\">  下一张  </a><a href=\""+HTML_DOMAIN+"index.html?";
const std::string PIC_TITLE4="\" class=\"button\">  上层目录  </a><button onclick=\"transbig()\">  放大尺寸  </button>&nbsp;&nbsp;<button onclick=\"transsm()\">  缩小尺寸  </button></h3>";


const std::string HTML_PIC_HEAD="<style>img{width:75%;height:auto;}</style><div align=\"center\" ><a href=\""+HTML_DOMAIN+"index.html?";
const std::string HTML_PIC_MID="\" ><img src=\"/source.jpg?";
const std::string HTML_PIC_TAIL="\"  class=\"pic\" ></a></div><br><script type=\"text/javascript\">function transbig(){var image=document.getElementsByClassName(\"pic\")[0];image.style.width=image.width*1.1+'px';}function transsm(){var image=document.getElementsByClassName(\"pic\")[0];image.style.width=image.width*0.91+'px';}</script>";



const std::string path_prefix = "/var/www/html/";
const std::string application_path_prefix1="/media/root/D2/迅雷下载/MMD/";
const std::string application_path_prefix2="/media/root/E/BaiduYunDownload/I站MMD/";
extern int runMode;

std::map<std::string , std::shared_ptr<void *>> videoFilename2mapp;
MutexLock videoFilename2mapp_lock;
char favicon[555] = {
    '\x89', 'P',    'N',    'G',    '\xD',  '\xA',  '\x1A', '\xA',  '\x0',
    '\x0',  '\x0',  '\xD',  'I',    'H',    'D',    'R',    '\x0',  '\x0',
    '\x0',  '\x10', '\x0',  '\x0',  '\x0',  '\x10', '\x8',  '\x6',  '\x0',
    '\x0',  '\x0',  '\x1F', '\xF3', '\xFF', 'a',    '\x0',  '\x0',  '\x0',
    '\x19', 't',    'E',    'X',    't',    'S',    'o',    'f',    't',
    'w',    'a',    'r',    'e',    '\x0',  'A',    'd',    'o',    'b',
    'e',    '\x20', 'I',    'm',    'a',    'g',    'e',    'R',    'e',
    'a',    'd',    'y',    'q',    '\xC9', 'e',    '\x3C', '\x0',  '\x0',
    '\x1',  '\xCD', 'I',    'D',    'A',    'T',    'x',    '\xDA', '\x94',
    '\x93', '9',    'H',    '\x3',  'A',    '\x14', '\x86', '\xFF', '\x5D',
    'b',    '\xA7', '\x4',  'R',    '\xC4', 'm',    '\x22', '\x1E', '\xA0',
    'F',    '\x24', '\x8',  '\x16', '\x16', 'v',    '\xA',  '6',    '\xBA',
    'J',    '\x9A', '\x80', '\x8',  'A',    '\xB4', 'q',    '\x85', 'X',
    '\x89', 'G',    '\xB0', 'I',    '\xA9', 'Q',    '\x24', '\xCD', '\xA6',
    '\x8',  '\xA4', 'H',    'c',    '\x91', 'B',    '\xB',  '\xAF', 'V',
    '\xC1', 'F',    '\xB4', '\x15', '\xCF', '\x22', 'X',    '\x98', '\xB',
    'T',    'H',    '\x8A', 'd',    '\x93', '\x8D', '\xFB', 'F',    'g',
    '\xC9', '\x1A', '\x14', '\x7D', '\xF0', 'f',    'v',    'f',    '\xDF',
    '\x7C', '\xEF', '\xE7', 'g',    'F',    '\xA8', '\xD5', 'j',    'H',
    '\x24', '\x12', '\x2A', '\x0',  '\x5',  '\xBF', 'G',    '\xD4', '\xEF',
    '\xF7', '\x2F', '6',    '\xEC', '\x12', '\x20', '\x1E', '\x8F', '\xD7',
    '\xAA', '\xD5', '\xEA', '\xAF', 'I',    '5',    'F',    '\xAA', 'T',
    '\x5F', '\x9F', '\x22', 'A',    '\x2A', '\x95', '\xA',  '\x83', '\xE5',
    'r',    '9',    'd',    '\xB3', 'Y',    '\x96', '\x99', 'L',    '\x6',
    '\xE9', 't',    '\x9A', '\x25', '\x85', '\x2C', '\xCB', 'T',    '\xA7',
    '\xC4', 'b',    '1',    '\xB5', '\x5E', '\x0',  '\x3',  'h',    '\x9A',
    '\xC6', '\x16', '\x82', '\x20', 'X',    'R',    '\x14', 'E',    '6',
    'S',    '\x94', '\xCB', 'e',    'x',    '\xBD', '\x5E', '\xAA', 'U',
    'T',    '\x23', 'L',    '\xC0', '\xE0', '\xE2', '\xC1', '\x8F', '\x0',
    '\x9E', '\xBC', '\x9',  'A',    '\x7C', '\x3E', '\x1F', '\x83', 'D',
    '\x22', '\x11', '\xD5', 'T',    '\x40', '\x3F', '8',    '\x80', 'w',
    '\xE5', '3',    '\x7',  '\xB8', '\x5C', '\x2E', 'H',    '\x92', '\x4',
    '\x87', '\xC3', '\x81', '\x40', '\x20', '\x40', 'g',    '\x98', '\xE9',
    '6',    '\x1A', '\xA6', 'g',    '\x15', '\x4',  '\xE3', '\xD7', '\xC8',
    '\xBD', '\x15', '\xE1', 'i',    '\xB7', 'C',    '\xAB', '\xEA', 'x',
    '\x2F', 'j',    'X',    '\x92', '\xBB', '\x18', '\x20', '\x9F', '\xCF',
    '3',    '\xC3', '\xB8', '\xE9', 'N',    '\xA7', '\xD3', 'l',    'J',
    '\x0',  'i',    '6',    '\x7C', '\x8E', '\xE1', '\xFE', 'V',    '\x84',
    '\xE7', '\x3C', '\x9F', 'r',    '\x2B', '\x3A', 'B',    '\x7B', '7',
    'f',    'w',    '\xAE', '\x8E', '\xE',  '\xF3', '\xBD', 'R',    '\xA9',
    'd',    '\x2',  'B',    '\xAF', '\x85', '2',    'f',    'F',    '\xBA',
    '\xC',  '\xD9', '\x9F', '\x1D', '\x9A', 'l',    '\x22', '\xE6', '\xC7',
    '\x3A', '\x2C', '\x80', '\xEF', '\xC1', '\x15', '\x90', '\x7',  '\x93',
    '\xA2', '\x28', '\xA0', 'S',    'j',    '\xB1', '\xB8', '\xDF', '\x29',
    '5',    'C',    '\xE',  '\x3F', 'X',    '\xFC', '\x98', '\xDA', 'y',
    'j',    'P',    '\x40', '\x0',  '\x87', '\xAE', '\x1B', '\x17', 'B',
    '\xB4', '\x3A', '\x3F', '\xBE', 'y',    '\xC7', '\xA',  '\x26', '\xB6',
    '\xEE', '\xD9', '\x9A', '\x60', '\x14', '\x93', '\xDB', '\x8F', '\xD',
    '\xA',  '\x2E', '\xE9', '\x23', '\x95', '\x29', 'X',    '\x0',  '\x27',
    '\xEB', 'n',    'V',    'p',    '\xBC', '\xD6', '\xCB', '\xD6', 'G',
    '\xAB', '\x3D', 'l',    '\x7D', '\xB8', '\xD2', '\xDD', '\xA0', '\x60',
    '\x83', '\xBA', '\xEF', '\x5F', '\xA4', '\xEA', '\xCC', '\x2',  'N',
    '\xAE', '\x5E', 'p',    '\x1A', '\xEC', '\xB3', '\x40', '9',    '\xAC',
    '\xFE', '\xF2', '\x91', '\x89', 'g',    '\x91', '\x85', '\x21', '\xA8',
    '\x87', '\xB7', 'X',    '\x7E', '\x7E', '\x85', '\xBB', '\xCD', 'N',
    'N',    'b',    't',    '\x40', '\xFA', '\x93', '\x89', '\xEC', '\x1E',
    '\xEC', '\x86', '\x2',  'H',    '\x26', '\x93', '\xD0', 'u',    '\x1D',
    '\x7F', '\x9',  '2',    '\x95', '\xBF', '\x1F', '\xDB', '\xD7', 'c',
    '\x8A', '\x1A', '\xF7', '\x5C', '\xC1', '\xFF', '\x22', 'J',    '\xC3',
    '\x87', '\x0',  '\x3',  '\x0',  'K',    '\xBB', '\xF8', '\xD6', '\x2A',
    'v',    '\x98', 'I',    '\x0',  '\x0',  '\x0',  '\x0',  'I',    'E',
    'N',    'D',    '\xAE', 'B',    '\x60', '\x82',
};

void MimeType::init() {
  mime[".html"] = "text/html";
  mime[".avi"] = "video/x-msvideo";
  mime[".bmp"] = "image/bmp";
  mime[".c"] = "text/plain";
  mime[".doc"] = "application/msword";
  mime[".gif"] = "image/gif";
  mime[".gz"] = "application/x-gzip";
  mime[".htm"] = "text/html";
  mime[".ico"] = "image/x-icon";
  mime[".jpg"] = "image/jpeg";
  mime[".JPG"] = "image/jpeg";
  mime[".gif"] = "image/jpeg";
  mime[".png"] = "image/png";
  mime[".PNG"] = "image/png";
  mime[".txt"] = "text/plain";
  mime[".mp3"] = "video/mp4";
  mime[".mp4"] = "video/mp4";
  mime[".mkv"] = "video/mp4";
  mime[".flv"] = "video/mp4";
  mime[".avi"] = "video/mp4";
  mime[".wmv"] = "video/mp4";
  mime[".rmvb"] = "video/mp4";

  mime["default"] = "text/html";
}

std::string MimeType::getMime(const std::string &suffix) {
  pthread_once(&once_control, MimeType::init);
  if (mime.find(suffix) == mime.end())
    return mime["default"];
  else
    return mime[suffix];
}
std::string getMime(std::string& filename)
{
    int dot_pos = filename.rfind('.',filename.size()-1);
    if (dot_pos == std::string::npos)
      return MimeType::getMime("default");
    else
      return MimeType::getMime(filename.substr(dot_pos));
}
typedef std::vector<std::string>::iterator vec_itor;
bool cmp(const string &p1, const string &p2)
{
  size_t pos1=p1.find('.'),pos2=p2.find('.');
    if (pos1!=std::string::npos&&pos2!=std::string::npos
      &&p1.substr(0,pos1).c_str()[0]>='0'&&p1.substr(0,pos1).c_str()[0]<='9'
      &&p2.substr(0,pos2).c_str()[0]>='0'&&p2.substr(0,pos2).c_str()[0]<='9')
      return atoi(p1.substr(0,pos1).c_str())<atoi(p2.substr(0,pos2).c_str());
    return p1<p2;
}

void FindFiles(HttpData *obj,std::string root ,std::vector<std::string> &files){ 
    //printf("FindFiles!\n");
    DIR *dir;
    struct dirent *ent;
    files.clear();
    obj->Vedioname2Num.clear();
    if ((dir = opendir (root.c_str())) != NULL) {

        while ((ent = readdir (dir)) != NULL) {
            if(!strcmp(ent->d_name,".")||!strcmp(ent->d_name,".."))
                continue;
            files.push_back(ent->d_name);
        }
        closedir (dir);
    }
    std::sort(files.begin(),files.end(),cmp);
    for (int i=0;i<files.size();i++)
      obj->Vedioname2Num[files[i]]=i;
}

HttpData::HttpData(EventLoop *loop, int connfd)
    : loop_(loop),
      channel_(new Channel(loop, connfd)),
      fd_(connfd),
      error_(false),
      connectionState_(H_CONNECTED),
      method_(METHOD_GET),
      HTTPVersion_(HTTP_11),
      nowReadPos_(0),
      state_(STATE_PARSE_URI),
      hState_(H_START),
      keepAlive_(false),
      mmapRet(NULL),
      para_("") {
  // loop_->queueInLoop(bind(&HttpData::setHandlers, this));
  channel_->setReadHandler(bind(&HttpData::handleRead, this));
  channel_->setWriteHandler(bind(&HttpData::handleWrite, this));
  channel_->setConnHandler(bind(&HttpData::handleConn, this));
  src_fd=0;
  if (runMode)
  {
    application_default_prefix=runMode==1?application_path_prefix1:application_path_prefix2;
    seed = time(0);//随机数种子，随机列表中的某个MMD文件播放
    srand(seed);
    //FindFiles(this,application_path_prefix,file_lists);
  }
}

void HttpData::reset() {
  // inBuffer_.clear();
  fileName_.clear();
  path_.clear();
  nowReadPos_ = 0;
  state_ = STATE_PARSE_URI;
  if_Ranges_ = 0;
  hState_ = H_START;
  headers_.clear();
  // keepAlive_ = false;
  if (timer_.lock()) {
    shared_ptr<TimerNode> my_timer(timer_.lock());
    my_timer->clearReq();
    timer_.reset();
  }
}

void HttpData::seperateTimer() {
  // cout << "seperateTimer" << endl;
  if (timer_.lock()) {
    shared_ptr<TimerNode> my_timer(timer_.lock());
    my_timer->clearReq();
    timer_.reset();
  }
}

void HttpData::handleRead() {
  __uint32_t &events_ = channel_->getEvents();
  do {
    bool zero = false;
    int read_num = readn(fd_, inBuffer_, zero);
    LOG << "Request( " << read_num <<" bytes ):\n" << inBuffer_;
    if (connectionState_ == H_DISCONNECTING) {
      inBuffer_.clear();
      break;
    }
    // cout << inBuffer_ << endl;
    if (read_num < 0) {
      perror("Connection dumped");
      error_ = true;
      handleError(fd_, 400, "Bad Request");
      break;
    }
    // else if (read_num == 0)
    // {
    //     error_ = true;
    //     break;
    // }
    else if (zero) {
      // 有请求出现但是读不到数据，可能是Request
      // Aborted，或者来自网络的数据没有达到等原因
      // 最可能是对端已经关闭了，统一按照对端已经关闭处理
      // error_ = true;
      connectionState_ = H_DISCONNECTING;
      if (read_num == 0) {
        // error_ = true;
        break;
      }
      // cout << "readnum == 0" << endl;
    }

    if (state_ == STATE_PARSE_URI) {
      URIState flag = this->parseURI();
      if (flag == PARSE_URI_AGAIN)
        break;
      else if (flag == PARSE_URI_ERROR) {
        perror("PARSE_URI_ERROR");
        LOG << "FD = " << fd_ << "," << inBuffer_ << "******";
        inBuffer_.clear();
        error_ = true;
        handleError(fd_, 400, "Bad Request");
        break;
      } else
        state_ = STATE_PARSE_HEADERS;
    }
    if (state_ == STATE_PARSE_HEADERS) {
      HeaderState flag = this->parseHeaders();
      if (flag == PARSE_HEADER_AGAIN)
        break;
      else if (flag == PARSE_HEADER_ERROR) {
        perror("PARSE_HEADER_ERROR");
        error_ = true;
        handleError(fd_, 400, "Bad Request");
        break;
      }
      if (method_ == METHOD_POST) {
        // POST方法准备
        state_ = STATE_RECV_BODY;
      } else {
        state_ = STATE_ANALYSIS;
      }
    }
    if (state_ == STATE_RECV_BODY) {
      int content_length = -1;
      if (headers_.find("Content-length") != headers_.end()) {
        content_length = stoi(headers_["Content-length"]);
      } else {
        // cout << "(state_ == STATE_RECV_BODY)" << endl;
        error_ = true;
        handleError(fd_, 400, "Bad Request: Lack of argument (Content-length)");
        break;
      }
      if (static_cast<int>(inBuffer_.size()) < content_length) break;
      state_ = STATE_ANALYSIS;
    }
    if (state_ == STATE_ANALYSIS) 
    {
      AnalysisState flag = this->analysisRequest();
      if (flag == ANALYSIS_SUCCESS) 
      {
        state_ = STATE_FINISH;
        break;
      } 
      else 
      {
        // cout << "state_ == STATE_ANALYSIS" << endl;
        error_ = true;
        break;
      }
    }
  } while (false);
  // cout << "state_=" << state_ << endl;
  if (!error_) 
  {
    if (outBuffer_.size() > 0) 
    {
      handleWrite();
      // events_ |= EPOLLOUT;
    }
    // error_ may change
    if (!error_ && state_ == STATE_FINISH) 
      {
      this->reset();
      if (inBuffer_.size() > 0) 
        {
          if (connectionState_ != H_DISCONNECTING) handleRead();//在这里递归调用的话error可能会变，指的是当前buff可能不止一个http请求，
        }

      // if ((keepAlive_ || inBuffer_.size() > 0) && connectionState_ ==
      // H_CONNECTED)
      // {
      //     this->reset();
      //     events_ |= EPOLLIN;
      // }
    } /*else if (!error_ && connectionState_ != H_DISCONNECTED)
      events_ |= EPOLLIN;      *///没用
  }
}

void HttpData::handleWrite() {
  if (!error_ && connectionState_ != H_DISCONNECTED) {
    /*if (outBuffer_.substr(0,9)=="HTTP/1.1 ")      
    {
      std::string temp;
      for (int i=0;i<outBuffer_.size();i++)
      if (i+1<outBuffer_.size()&&outBuffer_.c_str()[i]=='\n'&&outBuffer_.c_str()[i+1]=='\r')
      {
              LOG << "Answer without data:( " << i+3 <<" bytes ):\n" << outBuffer_.substr(0,i+3)<<"\ndata: "<<outBuffer_.size()-i-3<<"bytes\n"<<outBuffer_.substr(i+3,min(500,outBuffer_.size()-i-3));
              break;
      }
    }*/


    __uint32_t &events_ = channel_->getEvents();
    if (writen(fd_, outBuffer_) < 0) {
      perror("writen");
      events_ = 0;
      error_ = true;
    }
    if (outBuffer_.size() > 0) events_ |= EPOLLOUT;
    //if (outBuffer_.size() > 0) printf("%d\n",outBuffer_.size());
    //if (outBuffer_.size() == 0) printf("Write ok!\n");
  }
}

void HttpData::handleConn() {
  seperateTimer();
  __uint32_t &events_ = channel_->getEvents();
  if (!error_ && connectionState_ == H_CONNECTED) { //正常无错误
    if (events_ != 0) { //但是没写完
      int timeout = DEFAULT_EXPIRED_TIME;
      if (keepAlive_) timeout = DEFAULT_KEEP_ALIVE_TIME;
      /*if ((events_ & EPOLLIN) && (events_ & EPOLLOUT)) {//这里面代码不会到达
        events_ = __uint32_t(0);
        events_ |= EPOLLOUT;
      }*/
      // events_ |= (EPOLLET | EPOLLONESHOT);
      events_ |= EPOLLET;
      loop_->updatePoller(channel_, timeout);

    } else if (keepAlive_) {
      events_ |= (EPOLLIN | EPOLLET);
      // events_ |= (EPOLLIN | EPOLLET | EPOLLONESHOT);
      int timeout = DEFAULT_KEEP_ALIVE_TIME;
      loop_->updatePoller(channel_, timeout);
    } else {    //继续读，表示当前http没读完
      // cout << "close normally" << endl;
      // loop_->shutdown(channel_);
      // loop_->runInLoop(bind(&HttpData::handleClose, shared_from_this()));
      events_ |= (EPOLLIN | EPOLLET);
      // events_ |= (EPOLLIN | EPOLLET | EPOLLONESHOT);
      int timeout = (DEFAULT_KEEP_ALIVE_TIME >> 1);
      loop_->updatePoller(channel_, timeout);
    }
  } else if (!error_ && connectionState_ == H_DISCONNECTING &&
             (events_ & EPOLLOUT)) //可能在递归的时候突然断开，且还没写完，不能直接结束
  {
    events_ = (EPOLLOUT | EPOLLET);
  } 
  else 
  {//解析出错了直接结束
    // cout << "close with errors" << endl;
    loop_->runInLoop(bind(&HttpData::handleClose, shared_from_this()));
  }
}

URIState HttpData::parseURI() {
  //printf("parseURI\n");
  string &str = inBuffer_;
  string cop = str;
  // 读到完整的请求行再开始解析请求
  size_t pos = str.find('\r', nowReadPos_);
  if (pos == std::string::npos) {
    return PARSE_URI_AGAIN;
  }
  // 去掉请求行所占的空间，节省空间
  string request_line = str.substr(0, pos);
  //printf("%s\n",request_line.c_str());

  if (str.size() > pos + 1)
    str = str.substr(pos + 1);
  else
    str.clear();
  // Method
  int posGet = request_line.find("GET");
  int posPost = request_line.find("POST");
  int posHead = request_line.find("HEAD");

  if (posGet >= 0) {
    pos = posGet;
    method_ = METHOD_GET;
  } else if (posPost >= 0) {
    pos = posPost;
    method_ = METHOD_POST;
  } else if (posHead >= 0) {
    pos = posHead;
    method_ = METHOD_HEAD;
  } else {
    return PARSE_URI_ERROR;
  }

  // filename
  pos = request_line.find("/", pos);//请求的资源分为三段filename?application_path_prefix/para_
  if (pos == std::string::npos) {   //只有filename，显示并播放默认目录
    fileName_ = "index.html";
    setDefault();                   //有filename和application_path_prefix，切换到目标目录，如果目录无效则显示并播放默认目录
    HTTPVersion_ = HTTP_11;         //三个均有，则切换目标目录并播放目标文件
    return PARSE_URI_SUCCESS;       //只有访问媒体资源时候，filename=media，其他时候均按访问页面处理
  } else {
    size_t _pos = request_line.find(' ', pos);
    if (_pos == std::string::npos)
      return PARSE_URI_ERROR;
    else {
      if (_pos - pos > 1) {
          fileName_ = request_line.substr(pos + 1, _pos - pos - 1);
          bool is_decode=true;
          Url_Decode_UTF8(fileName_);//bug:有的文件不合法，文件名本身就是encode后的结果，故而在url中没有encode，然后在这里反而不需要decode，在这里却默认decode了造成文件找不到。windows下文件名中带%的文件linux不能识别
    redo: //printf("%sfileName_:%s\n",is_decode?"Decoded:":"Not Decoded",fileName_.c_str());
          size_t __pos = fileName_.find('?');
          if (__pos == std::string::npos) 
          {
            __pos=fileName_.size();//string::find没找到就返回string::npos，值为-1，我们设定为没找到就返回size()

          }
            if (__pos+1>=fileName_.size()) //没有para_
            {
                setDefault();
                para_=fileName_;
            }
            else //有para_
            {
                para_=fileName_.substr(__pos+1,fileName_.size()-__pos-1);//此时para_包含application_path_prefix和para_稍后分开

            if (para_[0]=='/')
            {
              struct stat tp_sbuf;
              stat(para_.c_str(), &tp_sbuf);
              if (S_ISDIR(tp_sbuf.st_mode))//para_为纯路径
                {
                  if (para_.c_str()[para_.size()-1]!='/')
                    para_.append("/");
                  if (application_path_prefix!=para_);
                  {
                    //printf("Change dir to %s\n",para_.c_str());
                    application_path_prefix=para_;
                    FindFiles(this,application_path_prefix.c_str(),file_lists);
                  }
                  para_="";
                }
                else if (S_ISREG(tp_sbuf.st_mode))//para_为文件
                {
                  int pos=para_.rfind('/',para_.size()-1);
                  if (application_path_prefix!=para_.substr(0,pos+1))
                  {
                    //printf("Change dir and open file %s\n",para_.c_str());
                    application_path_prefix=para_.substr(0,pos+1);
                    FindFiles(this,application_path_prefix.c_str(),file_lists);
                  }
                    para_=para_.substr(pos+1,para_.size()-pos-1);
                }
                else if (is_decode)
                  {
                    is_decode=false;
                    fileName_ = request_line.substr(pos + 1, _pos - pos - 1);
                    goto redo;
                  }
                  else 
                  {
                    setDefault();
                    printf("Open dir: %s failed\n",para_.c_str());
                  }


            }
            else//para_不是路径+文件的形式直接默认
            {
              setDefault();
            }
        }
        fileName_ = fileName_.substr(0, __pos);
        if (fileName_.empty())
              fileName_ = "index.html";
      }
      else 
      {
        fileName_ = "index.html";
        setDefault();
      }
    }
    pos = _pos;
  }
  //printf("application_path_prefix: %s\npara_: %s\n",application_path_prefix.c_str(),para_.c_str());

  // cout << "fileName_: " << fileName_ << endl;
  // HTTP 版本号
  pos = request_line.find("/", pos);
  if (pos == std::string::npos)
    return PARSE_URI_ERROR;
  else {
    if (request_line.size() - pos <= 3)
      return PARSE_URI_ERROR;
    else {
      string ver = request_line.substr(pos + 1, 3);
      if (ver == "1.0")
        HTTPVersion_ = HTTP_10;
      else if (ver == "1.1")
        HTTPVersion_ = HTTP_11;
      else
        return PARSE_URI_ERROR;
    }
  }
  return PARSE_URI_SUCCESS;
}
void HttpData::setDefault()
{
  para_="";
  application_path_prefix=application_default_prefix;
  FindFiles(this,application_path_prefix.c_str(),file_lists);
}

HeaderState HttpData::parseHeaders() {
  //printf("parseHeaders!\n %s",inBuffer_.c_str());
  string &str = inBuffer_;
  int key_start = -1, key_end = -1, value_start = -1, value_end = -1;
  int now_read_line_begin = 0;
  bool notFinish = true;
  size_t i = 0;
  HeaderState default_return=PARSE_HEADER_ERROR;
  for (; i < str.size() && notFinish; ++i) {
    switch (hState_) {
      case H_START: {
        if (str[i] == '\n' || str[i] == '\r') break;
        hState_ = H_KEY;
        key_start = i;
        now_read_line_begin = i;
        break;
      }
      case H_KEY: {
        if (str[i] == ':') {
          key_end = i;
          if (key_end - key_start <= 0) return default_return;
          hState_ = H_COLON;
        } else if (str[i] == '\n' || str[i] == '\r')
           return default_return;
        break;
      }
      case H_COLON: {
        if (str[i] == ' ') {
          hState_ = H_SPACES_AFTER_COLON;
        } else
           return default_return;
        break;
      }
      case H_SPACES_AFTER_COLON: {
        hState_ = H_VALUE;
        value_start = i;
        break;
      }
      case H_VALUE: {
        if (str[i] == '\r') {
          hState_ = H_CR;
          value_end = i;
          if (value_end - value_start <= 0)  return default_return;
        } else if (i - value_start > 1024)
           return default_return;
        break;
      }
      case H_CR: {
        if (str[i] == '\n') {
          hState_ = H_LF;
          string key(str.begin() + key_start, str.begin() + key_end);
          string value(str.begin() + value_start, str.begin() + value_end);
          headers_[key] = value;
          //printf("%s %s\n",key.c_str(),value.c_str());
          now_read_line_begin = i+1;////修复了每次读一行header导致的bug
        } else
           return default_return;
        break;
      }
      case H_LF: {
        if (str[i] == '\r') {
          hState_ = H_END_CR;
        } else {
          key_start = i;
          hState_ = H_KEY;
        }
        break;
      }
      case H_END_CR: {
        if (str[i] == '\n') {
          hState_ = H_END_LF;
        } else
           return default_return;
        break;
      }
      case H_END_LF: {
        notFinish = false;
        key_start = i;
        now_read_line_begin = i+1;
        break;
      }
    }
  }
  if (hState_ == H_END_LF) {
    str = str.substr(i);
    return PARSE_HEADER_SUCCESS;
  }
  if (now_read_line_begin<str.size()) //修复了每次读一行header导致的bug
  str = str.substr(now_read_line_begin);
else str.clear();
  return PARSE_HEADER_AGAIN;
}
std::string handle_size(double trans)
{
  std::string suffix,result;
  if (trans<1024)
  {
    suffix="B";
  }
  else if (trans<1024*1024)
  {
    suffix="KB";
    trans=(double)((long)trans*100/1024)/100;
  }
  else if (trans<1024*1024*1024)
  {
    suffix="MB";
    trans=(double)((long)trans*100/(1024*1024))/100;
  }
  else 
  {
    suffix="GB";
    trans=(double)((long)trans*100/(1024*1024*1024))/100;
  }
  result=to_string(trans);
  return result.substr(0,result.size()-4)+suffix;
}
AnalysisState HttpData::analysisRequest() 
{
  //printf("analysisRequest!\n");
  if (method_ == METHOD_POST) {
    // ------------------------------------------------------
    // My CV stitching handler which requires OpenCV library
    // ------------------------------------------------------
    // string header;
    // header += string("HTTP/1.1 200 OK\r\n");
    // if(headers_.find("Connection") != headers_.end() &&
    // headers_["Connection"] == "Keep-Alive")
    // {
    //     keepAlive_ = true;
    //     header += string("Connection: Keep-Alive\r\n") + "Keep-Alive:
    //     timeout=" + to_string(DEFAULT_KEEP_ALIVE_TIME) + "\r\n";
    // }
    // int length = stoi(headers_["Content-length"]);
    // vector<char> data(inBuffer_.begin(), inBuffer_.begin() + length);
    // Mat src = imdecode(data, CV_LOAD_IMAGE_ANYDEPTH|CV_LOAD_IMAGE_ANYCOLOR);
    // //imwrite("receive.bmp", src);
    // Mat res = stitch(src);
    // vector<uchar> data_encode;
    // imencode(".png", res, data_encode);
    // header += string("Content-length: ") + to_string(data_encode.size()) +
    // "\r\n\r\n";
    // outBuffer_ += header + string(data_encode.begin(), data_encode.end());
    // inBuffer_ = inBuffer_.substr(length);
    // return ANALYSIS_SUCCESS;
  } else if (method_ == METHOD_GET || method_ == METHOD_HEAD) 
  {  
    string header;
    std::string filetype = getMime(fileName_);
    if (headers_.find("Range") != headers_.end())//处理文件
    {
      if_Ranges_ = true;
      header += "HTTP/1.1 206 OK\r\n";
    }
    else
    header += "HTTP/1.1 200 OK\r\n";
    if (headers_.find("Connection") != headers_.end() &&
        (headers_["Connection"] == "Keep-Alive" ||
         headers_["Connection"] == "keep-alive")) {
      keepAlive_ = true;
      header += string("Connection: Keep-Alive\r\n") + "Keep-Alive: timeout=" +
                to_string(DEFAULT_KEEP_ALIVE_TIME) + "\r\n";
    }

    // echo test
    if (fileName_ == "hello") {
      outBuffer_ =
          "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nContent-Length: 12\r\n\r\nHello World\r\n";
      return ANALYSIS_SUCCESS;
    }
    if (fileName_ == "favicon.ico") {
      header += "Content-Type: image/png\r\n";
      header += "Content-Length: " + to_string(sizeof favicon) + "\r\n";
      header += "Server: HP's Web Server\r\n";

      header += "\r\n";
      outBuffer_ += header;
      outBuffer_ += string(favicon, favicon + sizeof favicon);
      ;
      return ANALYSIS_SUCCESS;
    }
    if (runMode &&  fileName_ == "index.html") { //处理html否则处理文件
      std::string TITLE,SUB_TITLE,CONTENT;//整个html分为head title content tail四部分
      int num;
      auto it=Vedioname2Num.find(para_);
      if (it==Vedioname2Num.end())//没有找到文件那就随机
      {
        if (file_lists.size())
          num=rand()%file_lists.size();
        else num=0;
      }
      else num=it->second;
      if (file_lists.size()&&para_.empty()&&getMime(file_lists[num])!="video/mp4")//不是视频的话哪就从第一个读起，是视频的话那就随机
        num=0;

      struct stat tp_sbuf;//为读取文件内容准备

      std::string UP_LEVEL_DIR;//上层目录
      int L_pos=application_path_prefix.rfind('/',application_path_prefix.size()-2);
      if (L_pos==std::string::npos)
        UP_LEVEL_DIR="/";
      else UP_LEVEL_DIR=application_path_prefix.substr(0,L_pos+1);

      if (file_lists.size()) //目前仅支持部分视频和图片
      {
          TITLE="<p>"+application_path_prefix+file_lists[num]+"    第"+to_string(num+1)+"/"+to_string(file_lists.size())+"项</p>";
          if (getMime(file_lists[num])==std::string("video/mp4"))
          {
            TITLE+=VIDEO_TITLE1+application_path_prefix+file_lists[rand()%file_lists.size()]+VIDEO_TITLE2+UP_LEVEL_DIR+VIDEO_TITLE3;//随机按钮
            TITLE+=std::string("正在播放:")+"<a href=\""+HTML_DOMAIN+"index.html?"+application_path_prefix+file_lists[num]+"\">"+file_lists[num]+"</a></h3>";
            TITLE+=HTML_VIDEO_HEAD+std::string("/source.mp4?")+application_path_prefix+file_lists[num]+HTML_VIDEO_TAIL;
          }
          else if (getMime(file_lists[num])==std::string("image/jpeg")||getMime(file_lists[num])==std::string("image/png"))
          {
            SUB_TITLE+=PIC_TITLE1+application_path_prefix+file_lists[num-1<0?file_lists.size()-1:num-1];//上一张
            SUB_TITLE+=PIC_TITLE2+application_path_prefix+file_lists[num+1<file_lists.size()?num+1:0];//下一张
            SUB_TITLE+=PIC_TITLE3+UP_LEVEL_DIR+PIC_TITLE4;//上层目录
            TITLE+=SUB_TITLE;
            TITLE+=HTML_PIC_HEAD+application_path_prefix+file_lists[num+1<file_lists.size()?num+1:0]+HTML_PIC_MID+application_path_prefix+file_lists[num]+HTML_PIC_TAIL;//点击图片下一张
            TITLE+=SUB_TITLE+"<body>";
          }
      }
      CONTENT+="<a>共"+to_string(file_lists.size()+1)+"项</a><br>";
      CONTENT+="<a href=\""+HTML_DOMAIN+"index.html?"+UP_LEVEL_DIR+"\">"+".."+"\\"+"</a><br>";//返回上层目录
      for (int i=0;i<file_lists.size();i++)
      {
        stat((application_path_prefix+file_lists[i]).c_str(), &tp_sbuf);
        if (S_ISREG(tp_sbuf.st_mode))//如果是普通文件
          CONTENT+="<a href=\""+HTML_DOMAIN+"index.html?"+application_path_prefix+file_lists[i]+"\">"+file_lists[i]+"("+handle_size(tp_sbuf.st_size)+")</a><br>";
        else CONTENT+="<a href=\""+HTML_DOMAIN+"index.html?"+application_path_prefix+file_lists[i]+"/\">"+file_lists[i]+"/"+"</a><br>";
      }
      outBuffer_ =
          "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-Length: "+to_string(HTML_HEAD.size()+TITLE.size()+CONTENT.size()+HTML_TAIL.size())+"\r\n\r\n"+HTML_HEAD+TITLE+CONTENT+HTML_TAIL+"\r\n";
      return ANALYSIS_SUCCESS;
    }
    /*if (runMode && fileName_ == "index.html") {//随机选取一个mmd播放
      int num=rand()%file_lists.size();
      //printf("%d %d\n",num,file_lists.size());
      outBuffer_ =
          "HTTP/1.1 307 Temporary Redirect\r\nLocation: "+file_lists[num]+"\r\n\r\n";
      return ANALYSIS_SUCCESS;
    }*/

//    Url_Decode_UTF8(para_);//url里的中文或者其他字符会被转义，这里将其转回来
    //这里开始是处理文件
    if (runMode) filename_full= application_path_prefix + para_;
        else filename_full = path_prefix + para_;
    if (stat(filename_full.c_str(), &sbuf) < 0) {
      header.clear();
      handleError(fd_, 404, "Not Found!");
      return ANALYSIS_ERROR;
    }
    //printf("filename_full: %s\n",filename_full.c_str());
    header += "Content-Type: " + filetype + "\r\n";
    long from=0,to=0,length=0;
    std::string temp_str;
    if (!if_Ranges_)//没有提出分段内容请求那就直接发了
      {
        header += "Content-Length: " + to_string(sbuf.st_size) + "\r\n";
      }
    else 
      {
        header += "Accept-Ranges: bytes\r\n";
        temp_str=headers_["Range"].substr(6,headers_["Range"].size()-6);//value="bytes=XXXXXX-XXXXX"
        int pos=temp_str.find('-');
        from=atol(temp_str.substr(0,pos).c_str());
        to=atol(temp_str.substr(pos+1,temp_str.size()-pos-1).c_str());
        if (to==0) to=sbuf.st_size-1;
        if (to-from+1>RESPOND_LENGTH) to=from+RESPOND_LENGTH-1;
        length=to-from+1;
        //printf("from,to,length   %d     %d     %d    \n",from,to,length);
        header += "Content-Length: " + to_string(length) + "\r\n";
        header += "Content-Range: bytes "+ to_string(from) +"-" +to_string(to)+"/"+to_string(sbuf.st_size)+"\r\n";
        header += "Cache-Control: max-age="+to_string(CACHE_CONTROL)+"\r\n";
      }
    header += "Server: HP's Web Server\r\n";
    // 头部结束
    header += "\r\n";
    outBuffer_ += header;

    if (method_ == METHOD_HEAD) return ANALYSIS_SUCCESS;
    if (getMime(para_)=="video/mp4"&&!if_Ranges_) return ANALYSIS_SUCCESS;//如果请求是mp4视频格式，浏览器首先发一个没有带Range的请求以获取视频长度，得到服务器回复后（此回复不带数据虽然请求是get），然后再发送带Range的，之后按range发数据；
                                                                          //如果请求的是其他格式比如html或者图片，则浏览器只发一次请求，服务器应当直接返回请求资源
                                                                          //视频有两种头，第一种是第一次发送的不带range的，在前面那一行就返回了，用于请求视频长度
                                                                          //第二种头是带range的，还有直接请求资源的，均可以运行下面代码
    if (!if_Ranges_)//请求的是资源文件
    {
        src_fd = open(filename_full.c_str(), O_RDONLY, 0);

        if (src_fd < 0) {
          outBuffer_.clear();
          handleError(fd_, 404, "Not Found!");
          return ANALYSIS_ERROR;
        }
          mmapRet = make_shared<void*>(mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0));//指向指针的智能指针
          //printf("file mmap :OX%p\n",*mmapRet);
          if (*mmapRet == (void *)-1) {
            mmapRet=nullptr;
            perror("munmap failed");
            outBuffer_.clear();
            handleError(fd_, 404, "Not Found!");
            return ANALYSIS_ERROR;
          }
      close(src_fd);
      char *src_addr = static_cast<char *>(*mmapRet);
      outBuffer_ += string(src_addr, sbuf.st_size);//资源文件的请求直接全部发送
      int ret=munmap(*mmapRet, sbuf.st_size);//内存映射，只会在真正访问内容的时候产生缺页中断将访问的数据从硬盘移入内存（部分移入），映射和取消影射的开销并不大
      //printf("file munmap :OX%p\n",*mmapRet);
      if (ret<0)
        perror("munmap failed!");
      mmapRet=nullptr;
      return ANALYSIS_SUCCESS;
    }
    else //请求的是视频文件
    {                      
      {
        MutexLockGuard gard(videoFilename2mapp_lock);//临界区保护videoFilename2mapp
        if (videoFilename2mapp.find(filename_full)!=videoFilename2mapp.end())//看看所需文件是否已经被映射过了
          {
            mmapRet=videoFilename2mapp[filename_full];    
          }
        if (!mmapRet)//只要还没完成映射，就打开文件开始映射
        {
            src_fd = open(filename_full.c_str(), O_RDONLY, 0);

            if (src_fd < 0) {
              outBuffer_.clear();
              handleError(fd_, 404, "Not Found!");
              return ANALYSIS_ERROR;
            }
            //printf("open: %s success\n",filename_full.c_str());
            mmapRet = make_shared<void*>(mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0));
            close(src_fd);
            printf("video mmap :OX%p\n",*mmapRet);
            if (*mmapRet == (void *)-1) {
              munmap(*mmapRet, sbuf.st_size);
              mmapRet=nullptr;
              perror("munmap failed");
              outBuffer_.clear();
              handleError(fd_, 404, "Not Found!");
              return ANALYSIS_ERROR;
            }
              videoFilename2mapp[filename_full]=mmapRet;//是视频的话就把映射存起来
        }
          printf("%s:use_count:%d\n",filename_full.c_str(),mmapRet.use_count());
      }
      char *src_addr = static_cast<char *>(*mmapRet);
      outBuffer_ += string(src_addr+from, length);//视频的range请求就分片
      return ANALYSIS_SUCCESS;
    }
  }
  return ANALYSIS_ERROR;
}

void HttpData::handleError(int fd, int err_num, string short_msg) {
  short_msg = " " + short_msg;
  char send_buff[4096];
  string body_buff, header_buff;
  body_buff += "<html><title>哎~出错了</title>";
  body_buff += "<body bgcolor=\"ffffff\">";
  body_buff += to_string(err_num) + short_msg;
  body_buff += "<hr><em> HP's Web Server</em>\n</body></html>";

  header_buff += "HTTP/1.1 " + to_string(err_num) + short_msg + "\r\n";
  header_buff += "Content-Type: text/html\r\n";
  header_buff += "Connection: Close\r\n";
  header_buff += "Content-Length: " + to_string(body_buff.size()) + "\r\n";
  header_buff += "Server: HP's Web Server\r\n";
  header_buff += "\r\n";
  // 错误处理不考虑writen不完的情况
  sprintf(send_buff, "%s", header_buff.c_str());
  writen(fd, send_buff, strlen(send_buff));
  sprintf(send_buff, "%s", body_buff.c_str());
  writen(fd, send_buff, strlen(send_buff));
}

void HttpData::handleClose() { //关闭的条件有两个，1超时（实际上只有pop了才算真正删除），2解析出错
  if (mmapRet)//此处才真正关闭视频文件的映射
  {
    weak_ptr<void*> tpr=mmapRet;
    mmapRet=nullptr;
    printf("%s:use_count:%d\n",filename_full.c_str(),tpr.use_count());
      MutexLockGuard gard(videoFilename2mapp_lock);//临界区保护videoFilename2mapp
      if (videoFilename2mapp.find(filename_full)!=videoFilename2mapp.end()&&videoFilename2mapp[filename_full].unique())
      {
        void *mmapRet_t=*videoFilename2mapp[filename_full];
        int ret=munmap(mmapRet_t, sbuf.st_size);
        if (ret<0)
        {
          perror("munmap failed!");
          printf("OX%p %d\n",mmapRet_t,sbuf.st_size);
        }
        else
        {
            videoFilename2mapp.erase(filename_full);
            printf("%s:deleted!\n",filename_full.c_str());
        }
      }
  }
  connectionState_ = H_DISCONNECTED;
  //shared_ptr<HttpData> guard(shared_from_this());
  loop_->removeFromPoller(channel_);
}

void HttpData::newEvent() {
  channel_->setEvents(DEFAULT_EVENT);
  loop_->addToPoller(channel_, DEFAULT_EXPIRED_TIME);
}
