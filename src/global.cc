/*******************************************************************************
 * global.cc - all the global variables are packeged into one class
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "global.h"
#include "utility.h"
#include "logger.h"
#include "url.h"
#include "page.h"
#include "md5.h"

_START_MYJFM_NAMESPACE_

#define CHECK_HAS_INIT() \
  do { \
    if (!_has_init) { \
      Cerr << "[FATAL] glob has not been initialized" << Endl; \
      abort(); \
    } \
  } while (0)

Global::Global() : 
  _has_init(false), 
  _config_file(""), 
  _cur_path(""), 
  _save_path(""), 
  _log_path(""), 
  _err_path(""), 
  _depth(5), 
  _logger(NULL), 
  _downloader_num(5), 
  _extractor_num(5), 
  _scheduler_num(1), 
  _dnser_num(1), 
  _to_be_shutdown(false), 
  _downloader_threadpool(NULL), 
  _extractor_threadpool(NULL), 
  _scheduler_threadpool(NULL), 
  _dnser_threadpool(NULL), 
  _create_connection_timeout(2), 
  _send_timeout(5), 
  _recv_timeout(30), 
  _request_header(""), 
  _user_agent("myjfm_spider"), 
  _sender("myjfm_spider@xxx.com"), 
  _dns_cache(NULL) {
  _file_types.clear();
  _seed_urls.clear();
  _extractor_queues.clear();
  _downloader_queues.clear();
  _scheduler_queues.clear();
  _dnser_queues.clear();
}

Global::~Global() {
  // write all the logs that are still in the buffer onto the disk
  // and close the log file
  delete _logger;
}

RES_CODE Global::init(String& v_cur_path, String& config_file_name) {
  if (_has_init) {
    Cerr << "[ERROR] init() failed. the glob has been initialized" << Endl;
    return S_HAS_INIT;
  }

  if (config_file_name == "") {
    Cerr << "[FATAL] init() failed. config_file_name is empty" << Endl;
    abort();
  }

  uint32_t i = 0;

  // It should be initialized here and JUST ONCE!!!
  _has_init = true;

  _cur_path = v_cur_path;
  // the default save path is current path
  _save_path = _cur_path;
  _config_file = config_file_name;
  _depth = 5;
  _downloader_num = 5;
  _extractor_num = 5;
  _scheduler_num = 1;
  _dnser_num = 1;

  _seed_urls.clear();

  load_default_file_types();
  parse_config();
  assemble_request_header();

  // initialize all the extractors' queues
  for (i = 0; i < _extractor_num; ++i) {
    _extractor_queues.push_back(
        SharedPointer<SQueue<SharedPointer<Page> > >
        (new SQueue<SharedPointer<Page> >()));
  }

  // initialize all the downloaders' queues
  for (i = 0; i < _downloader_num; ++i) {
    _downloader_queues.push_back(
        SharedPointer<SQueue<SharedPointer<Url> > >
        (new SQueue<SharedPointer<Url> >()));
  }

  // initialize all the schedulers' queues
  for (i = 0; i < _scheduler_num; ++i) {
    _scheduler_queues.push_back(
        SharedPointer<SQueue<SharedPointer<Url> > >
        (new SQueue<SharedPointer<Url> >()));
  }

  // initialize all the dnsers' queues
  for (i = 0; i < _dnser_num; ++i) {
    _dnser_queues.push_back(
        SharedPointer<SQueue<SharedPointer<Url> > >
        (new SQueue<SharedPointer<Url> >()));
  }

  // init memory pool
  MemoryPool::get_instance();

  // put all seed urls into url queue of one of the dnsers
  for (i = 0; i < _seed_urls.size(); ++i) {
    SharedPointer<Url> url_p(new Url(_seed_urls[i]));
    MD5 md5;
    if (url_p->get_md5(md5) != S_OK) {
      continue;
    }
    
    uint32_t index = 0;
    md5.shuffle(_dnser_num, index);
    _dnser_queues[index]->push(url_p);
  }

  // initialize the dns cache
  SharedPointer<DnsCache> tmp_dns_cache(new DnsCache());
  _dns_cache = tmp_dns_cache;

  _logger = new Logger(10000);
  _logger->init();

  return S_OK;
}

RES_CODE Global::load_default_file_types() {
  CHECK_HAS_INIT();

  _file_types.clear();
  _file_types.push_back(".htm");
  _file_types.push_back(".html");

  return S_OK;
}

RES_CODE Global::assemble_request_header() {
  CHECK_HAS_INIT();

  _request_header = "User-Agent: ";
  _request_header += _user_agent + " " + _sender;
  _request_header += "\r\nAccept: */*\r\n";
  _request_header += "Accept-Charset: utf-8\r\n";
  _request_header += "Accept-Encoding: gzip,deflate\r\n";
  _request_header += "Accept-Language: en,zh\r\n";
  _request_header += "Connection: keep-alive\r\n\r\n";

  return S_OK;
}

RES_CODE Global::parse_config() {
  CHECK_HAS_INIT();

  if (_config_file == "") {
    Cerr << "[FATAL] parse_config() failed. _config_file is empty" << Endl;
    abort();
  } else {
    FILE* config_file_p = fopen(_config_file.c_str(), "r");

    if (config_file_p == NULL) {
      Cerr << "[FATAL] fopen() failed." << Endl;
      Cerr << "[FATAL] can not open the file " << _config_file << Endl;
      abort();
    }

    char buffer[1024];
    while (fgets(buffer, 1023, config_file_p)) { 
      Utility::trim(buffer);
      if (buffer[0] == '\0' || buffer[0] == '#') {
        continue;
      }

      String line = buffer;
      String separator = " \t";
      Vector<String> key_and_value;
      Utility::split(line, separator, key_and_value);

      if (key_and_value.size() < 2) {
        continue;
      }

      if (key_and_value[0] == "SAVEPATH") {
        set_save_path(key_and_value[1]);
      } else if (key_and_value[0] == "LOGFILEPATH") {
        set_log_path(key_and_value[1]);
      } else if (key_and_value[0] == "ERRFILEPATH") {
        set_err_path(key_and_value[1]);
      } else if (key_and_value[0] == "DEPTH") {
        set_depth(key_and_value[1]);
      } else if (key_and_value[0] == "DOWNLOADERS") {
        set_downloader_num(key_and_value[1]);
      } else if (key_and_value[0] == "EXTRACTORS") {
        set_extractor_num(key_and_value[1]);
      } else if (key_and_value[0] == "SCHEDULERS") {
        set_scheduler_num(key_and_value[1]);
      } else if (key_and_value[0] == "DNSERS") {
        set_dnser_num(key_and_value[1]);
      } else if (key_and_value[0] == "FILETYPES") {
        set_file_types(key_and_value);
      } else if (key_and_value[0] == "SEEDURLS") {
        set_seed_urls(key_and_value);
      } else if (key_and_value[0] == "CREATE_CONNECTION_TIMEOUT") {
        set_create_connection_timeout(key_and_value[1]);
      } else if (key_and_value[0] == "SEND_TIMEOUT") {
        set_send_timeout(key_and_value[1]);
      } else if (key_and_value[0] == "RECV_TIMEOUT") {
        set_recv_timeout(key_and_value[1]);
      } else {
        continue;
      }
    }

    fclose(config_file_p);

    if (_seed_urls.empty()) {
      Cerr << "[FATAL] The _seed_urls is empty. Stop crawling" << Endl;
      exit(1);
    }

    if (access(_save_path.c_str(), R_OK | W_OK | X_OK) != 0) {
      Cerr << "[FATAL] The _save_path can't be written" << Endl;
      exit(1);
    }
  }

  return S_OK;
}

RES_CODE Global::set_seed_urls(Vector<String>& seed_urls) {
  _seed_urls.clear();

  uint32_t i;
  for (i = 1; i < seed_urls.size(); ++i) {
    _seed_urls.push_back(seed_urls[i]);
  }

  return S_OK;
}

RES_CODE Global::set_file_types(Vector<String>& file_types) {
  _file_types.clear();

  uint32_t i;
  for (i = 1; i < file_types.size(); ++i) {
    _file_types.push_back(file_types[i]);
  }

  return S_OK;
}

RES_CODE Global::set_downloader_num(String& downloader_num) {
  CHECK_HAS_INIT();
  _downloader_num = atoi(downloader_num.c_str());

  return S_OK;
}

RES_CODE Global::get_downloader_num(uint32_t& downloader_num) {
  CHECK_HAS_INIT();
  downloader_num = _downloader_num;

  return S_OK;
}

RES_CODE Global::set_extractor_num(String& extractor_num) {
  CHECK_HAS_INIT();
  _extractor_num = atoi(extractor_num.c_str());

  return S_OK;
}

RES_CODE Global::get_extractor_num(uint32_t& extractor_num) {
  CHECK_HAS_INIT();
  extractor_num = _extractor_num;

  return S_OK;
}

RES_CODE Global::set_scheduler_num(String& scheduler_num) {
  CHECK_HAS_INIT();
  _scheduler_num = atoi(scheduler_num.c_str());

  return S_OK;
}

RES_CODE Global::get_scheduler_num(uint32_t& scheduler_num) {
  CHECK_HAS_INIT();
  scheduler_num = _scheduler_num;

  return S_OK;
}

RES_CODE Global::set_dnser_num(String& dnser_num) {
  CHECK_HAS_INIT();
  _dnser_num = atoi(dnser_num.c_str());

  return S_OK;
}

RES_CODE Global::get_dnser_num(uint32_t& dnser_num) {
  CHECK_HAS_INIT();
  dnser_num = _dnser_num;

  return S_OK;
}

RES_CODE Global::set_downloader_threadpool(SharedPointer<ThreadPool>& ptr) {
  if (ptr.is_null()) {
    return S_BAD_ARG;
  }

  _downloader_threadpool = ptr;

  return S_OK;
}

RES_CODE Global::get_downloader_threadpool(SharedPointer<ThreadPool>& ptr) {
  ptr = _downloader_threadpool;
  return S_OK;
}

RES_CODE Global::set_extractor_threadpool(SharedPointer<ThreadPool>& ptr) {
  if (ptr.is_null()) {
    return S_BAD_ARG;
  }

  _extractor_threadpool = ptr;

  return S_OK;
}

RES_CODE Global::get_extractor_threadpool(SharedPointer<ThreadPool>& ptr) {
  ptr = _extractor_threadpool;
  return S_OK;
}

RES_CODE Global::set_scheduler_threadpool(SharedPointer<ThreadPool>& ptr) {
  if (ptr.is_null()) {
    return S_BAD_ARG;
  }

  _scheduler_threadpool = ptr;

  return S_OK;
}

RES_CODE Global::get_scheduler_threadpool(SharedPointer<ThreadPool>& ptr) {
  ptr = _scheduler_threadpool;
  return S_OK;
}

RES_CODE Global::set_dnser_threadpool(SharedPointer<ThreadPool>& ptr) {
  if (ptr.is_null()) {
    return S_BAD_ARG;
  }

  _dnser_threadpool = ptr;

  return S_OK;
}

RES_CODE Global::get_dnser_threadpool(SharedPointer<ThreadPool>& ptr) {
  ptr = _dnser_threadpool;
  return S_OK;
}

RES_CODE Global::get_cur_path(String& cur_path) {
  CHECK_HAS_INIT();
  cur_path = _cur_path;

  return S_OK;
}

RES_CODE Global::set_cur_path(String& cur_path) {
  CHECK_HAS_INIT();
  _cur_path = cur_path;

  return S_OK;
}

RES_CODE Global::get_save_path(String& save_path) {
  CHECK_HAS_INIT();
  save_path = _save_path;

  return S_OK;
}

RES_CODE Global::set_save_path(String& path) {
  CHECK_HAS_INIT();
  _save_path = path;

  return S_OK;
}

RES_CODE Global::get_log_path(String& path) {
  CHECK_HAS_INIT();
  path = _log_path;

  return S_OK;
}

RES_CODE Global::set_log_path(String& path) {
  CHECK_HAS_INIT();
  _log_path = path;

  return S_OK;
}

RES_CODE Global::get_err_path(String& path) {
  CHECK_HAS_INIT();
  path = _err_path;

  return S_OK;
}

RES_CODE Global::set_err_path(String& path) {
  CHECK_HAS_INIT();
  _err_path = path;

  return S_OK;
}

RES_CODE Global::get_depth(uint32_t& dep) {
  CHECK_HAS_INIT();
  dep = _depth;

  return S_OK;
}

RES_CODE Global::set_depth(String& dep) {
  CHECK_HAS_INIT();
  _depth = atoi(dep.c_str());

  return S_OK;
}

RES_CODE Global::get_extractor_queue(uint32_t id, 
    SharedPointer<SQueue<SharedPointer<Page> > >& queue) {
  CHECK_HAS_INIT();

  uint32_t extractor_num = 0;
  get_extractor_num(extractor_num);

  if (id >= extractor_num) {
    queue = SharedPointer<SQueue<SharedPointer<Page> > >(NULL);
    return S_OUT_RANGE;
  }

  queue = _extractor_queues[id];

  return S_OK;
}

RES_CODE Global::get_downloader_queue(uint32_t id, 
    SharedPointer<SQueue<SharedPointer<Url> > >& queue) {
  CHECK_HAS_INIT();

  uint32_t downloader_num = 0;
  get_downloader_num(downloader_num);

  if (id >= downloader_num) {
    queue = SharedPointer<SQueue<SharedPointer<Url> > >(NULL);
    return S_OUT_RANGE;
  }

  queue = _downloader_queues[id];

  return S_OK;
}

RES_CODE Global::get_scheduler_queue(uint32_t id, 
    SharedPointer<SQueue<SharedPointer<Url> > >& queue) {
  CHECK_HAS_INIT();

  uint32_t scheduler_num = 0;
  get_scheduler_num(scheduler_num);

  if (id >= scheduler_num) {
    queue = SharedPointer<SQueue<SharedPointer<Url> > >(NULL);
    return S_OUT_RANGE;
  }

  queue = _scheduler_queues[id];

  return S_OK;
}

RES_CODE Global::get_dnser_queue(uint32_t id, 
    SharedPointer<SQueue<SharedPointer<Url> > >& queue) {
  CHECK_HAS_INIT();

  uint32_t dnser_num = 0;
  get_dnser_num(dnser_num);

  if (id >= dnser_num) {
    queue = SharedPointer<SQueue<SharedPointer<Url> > >(NULL);
    return S_OUT_RANGE;
  }

  queue = _dnser_queues[id];

  return S_OK;
}

RES_CODE Global::get_logger(Logger*& logger) {
  CHECK_HAS_INIT();
  logger = _logger;

  return S_OK;
}

bool Global::get_to_be_shutdown() {
  CHECK_HAS_INIT();
  return _to_be_shutdown;
}

RES_CODE Global::set_to_be_shutdown(bool to_be_shutdown) {
  CHECK_HAS_INIT();
  _to_be_shutdown = to_be_shutdown;
  return S_OK;
}

RES_CODE Global::get_request_header(String& request_header) {
  CHECK_HAS_INIT();
  request_header += _request_header;

  return S_OK;
}

RES_CODE Global::set_create_connection_timeout(String& timeout) {
  CHECK_HAS_INIT();
  _create_connection_timeout = atoi(timeout.c_str());

  return S_OK;
}

RES_CODE Global::set_send_timeout(String& timeout) {
  CHECK_HAS_INIT();
  _send_timeout = atoi(timeout.c_str());

  return S_OK;
}

RES_CODE Global::set_recv_timeout(String& timeout) {
  CHECK_HAS_INIT();
  _recv_timeout = atoi(timeout.c_str());

  return S_OK;
}

RES_CODE Global::get_create_connection_timeout(uint32_t& timeout) {
  CHECK_HAS_INIT();
  timeout = _create_connection_timeout;

  return S_OK;
}

RES_CODE Global::get_send_timeout(uint32_t& timeout) {
  CHECK_HAS_INIT();
  timeout = _send_timeout;

  return S_OK;
}

RES_CODE Global::get_recv_timeout(uint32_t& timeout) {
  CHECK_HAS_INIT();
  timeout = _recv_timeout;

  return S_OK;
}

RES_CODE Global::get_dns_cache(SharedPointer<DnsCache>& dns_cache) {
  CHECK_HAS_INIT();
  dns_cache = _dns_cache;

  return S_OK;
}

#undef CHECK_HAS_INIT

_END_MYJFM_NAMESPACE_

