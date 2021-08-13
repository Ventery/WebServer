
#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <map>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "Timer.h"
#include <sys/stat.h>
#include "Util.h"

class EventLoop;
class TimerNode;
class Channel;

enum ProcessState {
  STATE_PARSE_URI = 1,
  STATE_PARSE_HEADERS,
  STATE_RECV_BODY,
  STATE_ANALYSIS,
  STATE_FINISH
};

enum URIState {
  PARSE_URI_AGAIN = 1,
  PARSE_URI_ERROR,
  PARSE_URI_SUCCESS,
};

enum HeaderState {
  PARSE_HEADER_SUCCESS = 1,
  PARSE_HEADER_AGAIN,
  PARSE_HEADER_ERROR
};

enum AnalysisState { ANALYSIS_SUCCESS = 1, ANALYSIS_ERROR };

enum ParseState {
  H_START = 0,
  H_KEY,
  H_COLON,
  H_SPACES_AFTER_COLON,
  H_VALUE,
  H_CR,
  H_LF,
  H_END_CR,
  H_END_LF
};

enum ConnectionState { H_CONNECTED = 0, H_DISCONNECTING, H_DISCONNECTED };

enum HttpMethod { METHOD_POST = 1, METHOD_GET, METHOD_HEAD };

enum HttpVersion { HTTP_10 = 1, HTTP_11 };

class MimeType {
 private:
  static void init();
  static std::unordered_map<std::string, std::string> mime;
  MimeType();
  MimeType(const MimeType &m);

 public:
  static std::string getMime(const std::string &suffix);

 private:
  static pthread_once_t once_control;
};

class HttpData : public std::enable_shared_from_this<HttpData> {
 public:
  HttpData(EventLoop *loop, int connfd);
  ~HttpData() 
  { 
    close(fd_); 
    printf("in destructor %d closed!\n",fd_);
  }
  void reset();
  void seperateTimer();
  void linkTimer(std::shared_ptr<TimerNode> mtimer) {
    // shared_ptr重载了bool, 但weak_ptr没有
    timer_ = mtimer;
  }
  std::shared_ptr<Channel> getChannel() { return channel_; }
  EventLoop *getLoop() { return loop_; }
  void handleClose();
  void newEvent();

 private:
  EventLoop *loop_;
  std::shared_ptr<Channel> channel_;
  int fd_;
  std::string inBuffer_;
  std::string outBuffer_;
  bool error_;
  ConnectionState connectionState_;

  HttpMethod method_;
  HttpVersion HTTPVersion_;
  std::string fileName_;
  std::string path_;
  std::string application_default_prefix;
  std::string application_path_prefix;
  int nowReadPos_;
  ProcessState state_;
  ParseState hState_;
  int if_Ranges_;//支持google浏览器的播放器拖动
  int src_fd;
  std::shared_ptr<void*> mmapRet;//是一个指向指针的指针，获取所指的指针应该用*操作符
  struct stat sbuf;
  std::string para_;
  std::string filename_full;//文件的全名（带绝对路径）


  bool keepAlive_;
  std::map<std::string, std::string> headers_;
  std::map<std::string, int> Vedioname2Num;
  std::weak_ptr<TimerNode> timer_;
  std::vector<std::string> file_lists;
  unsigned seed;
  void setDefault();
  void handleRead();
  void handleWrite();
  void handleConn();
  void handleError(int fd, int err_num, std::string short_msg);
  URIState parseURI();
  HeaderState parseHeaders();
  AnalysisState analysisRequest();
  friend void FindFiles(HttpData *,std::string ,std::vector<std::string> &);
};