/*******************************************************************************
 * utility.h - the utility function
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "config.h"

#define ASSERT(exp) assert(exp)

_START_MYJFM_NAMESPACE_

class Utility {
public:
  static void ltrim(char* str);
  static String ltrim(const String& str);

  static void rtrim(char* str);
  static String rtrim(const String& str);

  static void trim(char* str);
  static String trim(const String& str);
  
  static void toupper(char* str);
  static String toupper(const String& str);
  static void toupper_inplace(String& str);

  static void tolower(char* str);
  static String tolower(const String& str);
  static void tolower_inplace(String& str);

  static void split(const String& str, 
      const String& separator, 
      Vector<String>& container);
  
  static String get_file_full_path(String path);

  template <class T>
  static RES_CODE str2integer(const char* str, T& number) {
    if (!str) {
      return S_FAIL;
    }
    
    Stringstream ss;
    T tmp;
    bool ret = false;
    ss << str;
    if (ss.str().length() > 0) {
      try {
        ss >> tmp;
        if (ss.eof() && !ss.fail()) {
          ret = true;
          number = tmp;
        }
      } catch (std::ios_base::failure&) {
      }
    }
    
    if (ret) {
      return S_OK;
    } else {
      return S_FAIL;
    }
  }

  template <class T>
  static RES_CODE integer2str(T& number, String& str) {
    Stringstream ss;
    bool ret = false;
    ss << number;
    if (ss.str().length() > 0) {
      try {
        ss >> str;
        if (ss.eof() && !ss.fail()) {
          ret = true;
        }
      } catch (std::ios_base::failure&) {
      }
    }
    
    if (ret) {
      return S_OK;
    } else {
      return S_FAIL;
    }
  }

  template <class T>
  static RES_CODE integer2str(T&number, char* str) {
    if (!str) {
      return S_FAIL;
    }
    
    String tmp;
    if (Utility::str2integer(number, tmp) != S_OK) {
      return S_FAIL;
    }

    strcpy(str, tmp.c_str());
    return S_OK;
  }

  template <class T>
  static RES_CODE str2integer(String& str, T& number) {
    return Utility::str2integer(str.c_str(), number);
  }

  static bool is_hex_digit(char p);
  static RES_CODE str2hex(const char* str, uint32_t& number);
  static RES_CODE str2hex(String& str, uint32_t number);
  static char* strdupn(const char* src, int n);
  static char* strdup(const char* src);
  static int strcmp(const char* str1, const char* str2);
  static int strcasecmp(const char* str1, const char* str2);
  static int strncasecmp(const char* str1, const char* str2, uint32_t n);
  static RES_CODE escape(String& str, String& escstr);
};

_END_MYJFM_NAMESPACE_

#endif

