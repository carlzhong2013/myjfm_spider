/*******************************************************************************
 * global.h - all the global variables are packeged into one class
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdint.h>

#include "config.h"
#include "url.h"
#include "dns_cache.h"
#include "squeue.h"
#include "page.h"
#include "shared_pointer.h"
#include "thread_pool.h"

_START_MYJFM_NAMESPACE_

class Logger;

class Global {
public:
  Global();
  ~Global();

  RES_CODE init(String&, String&);
  RES_CODE parse_config();

  // for consistency, we use reference to bring back what we want
  RES_CODE get_cur_path(String&);
  RES_CODE get_log_path(String&);
  RES_CODE get_err_path(String&);
  RES_CODE set_log_path(String& path);
  RES_CODE set_err_path(String& path);
  RES_CODE get_save_path(String&);
  RES_CODE get_depth(uint32_t&);

  RES_CODE get_downloader_num(uint32_t&);
  RES_CODE get_extractor_num(uint32_t&);
  RES_CODE get_scheduler_num(uint32_t&);
  RES_CODE get_dnser_num(uint32_t&);

  RES_CODE get_extractor_queue(uint32_t, 
      SharedPointer<SQueue<SharedPointer<Page> > >&);

  RES_CODE get_downloader_queue(uint32_t, 
      SharedPointer<SQueue<SharedPointer<Url> > >&);

  RES_CODE get_scheduler_queue(uint32_t, 
      SharedPointer<SQueue<SharedPointer<Url> > >&);

  RES_CODE get_dnser_queue(uint32_t, 
      SharedPointer<SQueue<SharedPointer<Url> > >&);

  RES_CODE get_dns_cache(SharedPointer<DnsCache>&);

  RES_CODE set_downloader_threadpool(SharedPointer<ThreadPool>&);
  RES_CODE set_extractor_threadpool(SharedPointer<ThreadPool>&);
  RES_CODE set_scheduler_threadpool(SharedPointer<ThreadPool>&);
  RES_CODE set_dnser_threadpool(SharedPointer<ThreadPool>&);

  RES_CODE get_downloader_threadpool(SharedPointer<ThreadPool>&);
  RES_CODE get_extractor_threadpool(SharedPointer<ThreadPool>&);
  RES_CODE get_scheduler_threadpool(SharedPointer<ThreadPool>&);
  RES_CODE get_dnser_threadpool(SharedPointer<ThreadPool>&);

  RES_CODE get_logger(Logger*&);

  bool get_to_be_shutdown();
  RES_CODE set_to_be_shutdown(bool);

  RES_CODE get_request_header(String&);

  //RES_CODE get_dns_timeout(uint32_t&);
  RES_CODE get_create_connection_timeout(uint32_t&);
  RES_CODE get_send_timeout(uint32_t&);
  RES_CODE get_recv_timeout(uint32_t&);

  //RES_CODE get_name_server(String&);

private:
  RES_CODE load_default_file_types();

  RES_CODE assemble_request_header();

  //RES_CODE check_name_server();

  RES_CODE set_cur_path(String&);
  RES_CODE set_seed_urls(Vector<String>&);
  RES_CODE set_file_types(Vector<String>&);
  RES_CODE set_save_path(String&);
  RES_CODE set_depth(String&);

  RES_CODE set_downloader_num(String&);
  RES_CODE set_extractor_num(String&);
  RES_CODE set_scheduler_num(String&);
  RES_CODE set_dnser_num(String&);

  //RES_CODE set_dns_timeout(String&);
  RES_CODE set_create_connection_timeout(String&);
  RES_CODE set_send_timeout(String&);
  RES_CODE set_recv_timeout(String&);

  //RES_CODE set_name_server(String&);

  //Mutex _mutex;
  // if there exists multi-threads, should guarantee consistency by mutex
  volatile bool _has_init;

  String _config_file;

  // current work path
  String _cur_path;

  // the path saved all the web pages and all the indexes
  String _save_path;

  String _log_path;

  String _err_path;

  // the depth of downloading recursively
  uint32_t _depth;

  Logger *_logger;

  uint32_t _downloader_num;
  uint32_t _extractor_num;
  uint32_t _scheduler_num;
  uint32_t _dnser_num;

  bool _to_be_shutdown;

  // the types of file which downloader can download
  Vector<String> _file_types;

  // the seed urls read from configure file
  Vector<String> _seed_urls;

  // extractors' page queue. This queue is used by extractors and downloaders.
  // The downloaders produce fresh pages downloaded from Internet, then pass
  // them to extractors
  Vector<SharedPointer<SQueue<SharedPointer<Page> > > > _extractor_queues;

  // dnsers' url queue. This queue is used by dnsers and extractors.
  // The extractors produce fresh urls extracted from web pages, then, pass
  // them to dnsers
  Vector<SharedPointer<SQueue<SharedPointer<Url> > > > _dnser_queues;

  // schedulers' url queue. This queue is used by schedulers and dnsers.
  // The dnsers produce urls that have been dnsed, and, 
  // the schedulers consume urls, they judge whether can pass them to 
  // the downloaders immediately
  Vector<SharedPointer<SQueue<SharedPointer<Url> > > > _scheduler_queues;

  // downloaders' url queue. Each downloader has one queue.
  // The queue is shared between this downloader and all schedulers.
  Vector<SharedPointer<SQueue<SharedPointer<Url> > > > _downloader_queues;

  SharedPointer<ThreadPool> _downloader_threadpool;
  SharedPointer<ThreadPool> _extractor_threadpool;
  SharedPointer<ThreadPool> _scheduler_threadpool;
  SharedPointer<ThreadPool> _dnser_threadpool;

  //uint32_t _dns_timeout;
  uint32_t _create_connection_timeout;
  uint32_t _send_timeout;
  uint32_t _recv_timeout;

  String _request_header;
  String _user_agent;
  String _sender;

#if 0
  static Map<String, String> _MIME;
#endif
  SharedPointer<DnsCache> _dns_cache;

  //String _name_server;
};

_END_MYJFM_NAMESPACE_

#endif

