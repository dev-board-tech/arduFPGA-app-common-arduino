#ifndef __WEB_BROWSER_H__
#define __WEB_BROWSER_H__

#if defined(ESP8266) || defined(ESP32)
//#include "def.h"
#if USE_HTTPS
#if defined(ESP8266)
#include <ESP8266WebServerSecure.h>
#else
#include <WebServerSecure.h>
#endif
#else
#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif
#endif

class webBrowser {
public:
  webBrowser(int sesion);
  ~webBrowser();
#if USE_HTTPS
  void cd(BearSSL::ESP8266WebServerSecure *server);
  void ls(BearSSL::ESP8266WebServerSecure *server);
  void mkdir(BearSSL::ESP8266WebServerSecure *server);
  void rm(BearSSL::ESP8266WebServerSecure *server);
  void read(BearSSL::ESP8266WebServerSecure *server);
#else
#if defined(ESP8266)
  void cd(ESP8266WebServer *server);
  void ls(ESP8266WebServer *server);
  void mkdir(ESP8266WebServer *server);
  void rm(ESP8266WebServer *server);
  void read(ESP8266WebServer *server);
#else
  void cd(WebServer *server);
  void ls(WebServer *server);
  void mkdir(WebServer *server);
  void rm(WebServer *server);
  void read(WebServer *server);
#endif
#endif

  int sesion;

private:
  String path;
};
#endif // !defined(ESP8266) || defined(ESP32)

#endif