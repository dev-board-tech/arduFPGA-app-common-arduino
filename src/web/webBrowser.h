#ifndef __WEB_BROWSER_H__
#define __WEB_BROWSER_H__

//#include "def.h"
#if USE_HTTPS
#include <ESP8266WebServerSecure.h>
#else
#include <ESP8266WebServer.h>
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
  void cd(ESP8266WebServer *server);
  void ls(ESP8266WebServer *server);
  void mkdir(ESP8266WebServer *server);
  void rm(ESP8266WebServer *server);
  void read(ESP8266WebServer *server);
#endif

  int sesion;

private:
  String path;
};

#endif