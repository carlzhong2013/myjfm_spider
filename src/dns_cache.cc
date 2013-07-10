/*******************************************************************************
 * dns_cache.cc - cache the mapping from site to ips
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "config.h"
#include "dns_cache.h"
#include "log.h"

_START_MYJFM_NAMESPACE_

DnsCache::DnsCache() {
  _dns.clear();
}

DnsCache::~DnsCache() {
  _dns.clear();
}

RES_CODE DnsCache::find(String& site, Vector<String>& ips) {
  ips.clear();
  Map<String, Vector<String> >::iterator itr = _dns.find(site);

  if (itr == _dns.end()) {
    return S_NOT_FOUND;
  }

  int i;
  for (i = 0; i < ips.size(); ++i) {
    ips.push_back((itr->second)[i]);
  }

  return S_OK;
}

RES_CODE DnsCache::insert(String& site, Vector<String>& ips) {
  Vector<String> tmp;
  if (find(site, tmp)) {
    return S_ALREADY_EXIST;
  }

  Pair<String, Vector<String> > value(site, ips);
  _dns.insert(value);
  return S_OK;
}

RES_CODE DnsCache::update(String& site, Vector<String>& ips) {
  Vector<String> tmp;
  if (!find(site, tmp)) {
    return S_NOT_FOUND;
  }

  _dns[site] = ips;
  return S_OK;
}

RES_CODE DnsCache::dns_query(String& site, 
    Vector<String>& ips, IPTYPE& ip_type) {
  ips.clear();

  char buffer[20];
  struct hostent* entry = gethostbyname(site.c_str());

  if (!entry) {
    LOG(ERROR, "Can't get the IPs of the site: %s", site.c_str());
    return S_FAIL;
  }

  if (entry->h_addrtype == AF_INET && entry->h_length == 4) {
    ip_type = IPv4;

    while (*(entry->h_addr_list)) {
      inet_ntop(AF_INET, *(entry->h_addr_list), buffer, sizeof(buffer));
      entry->h_addr_list += 1;
      ips.push_back(CHARS2STR(buffer));
    }
  } else if (entry->h_addrtype == AF_INET6 && entry->h_length == 6) {
    ip_type = IPv6;

    while (*(entry->h_addr_list)) {
      inet_ntop(AF_INET6, *(entry->h_addr_list), buffer, sizeof(buffer));
      entry->h_addr_list += 1;
      ips.push_back(CHARS2STR(buffer));
    }
  } else {
    LOG(ERROR, "Invalid address!");
    ip_type = IP_DUMMY;
    return S_UNKNOWN_PROTOCOL;
  }

  return S_OK;
}

_END_MYJFM_NAMESPACE_

