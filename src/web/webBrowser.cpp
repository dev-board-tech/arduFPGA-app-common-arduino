#if defined(ESP8266)// || defined(ESP32)

#include "webBrowser.h"

#include "Arduino.h"
#include <time.h>
#include <FS.h>

#include "util/util.h"

static String timeToString(time_t t) {
  String timeStr;
  struct tm *lt = localtime(&t);
  char str[32];
  strftime(str, sizeof str, "%Y/%m/%d %H-%M-%S", lt);
  timeStr = str;
  return timeStr;
}

//format bytes
static String formatBytes(size_t bytes){
  if (bytes < 1024){
    return String(bytes)+"B";
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0)+"KB";
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0)+"MB";
  } else {
    return String(bytes/1024.0/1024.0/1024.0)+"GB";
  }
}

webBrowser::webBrowser(int sesion) {
  this->sesion = sesion;
}

webBrowser::~webBrowser() {
  
}

#if USE_HTTPS
void webBrowser::cd(BearSSL::ESP8266WebServerSecure *server) {
#else
#if defined(ESP8266)
void webBrowser::cd(ESP8266WebServer *server) {
#else
void webBrowser::cd(WebServer *server) {
#endif
#endif
  String p = path;
  if(server->arg(0)[0] == '/') {
    p = server->arg(0);
  } else {
    p += server->arg(0);
  }
  if(SPIFFS.exists(p)) {
    if(p.charAt(p.length() - 1) != '/')
      p += "/";
    path = p;
    server->send(200, "text/plain", path);
    return;
  }
  server->send(200, "text/plain", "No such file or directory");
}

#if USE_HTTPS
void webBrowser::ls(BearSSL::ESP8266WebServerSecure *server) {
#else
#if defined(ESP8266)
void webBrowser::ls(ESP8266WebServer *server) {
#else
void webBrowser::ls(WebServer *server) {
#endif
#endif
  server->setContentLength(CONTENT_LENGTH_UNKNOWN);
  server->send(200, "text/plain", "");
  Dir dir = SPIFFS.openDir(path);
  bool filesFound = false;
  while (dir.next()) {
    if(dir.isFile()) {
      File entry = dir.openFile("r");
      String content;
      String name = entry.name();
      name.replace(server->arg(0), "");
      if(name.charAt(0) == '/') {
        name.remove(0, 1);
      }
      content += name;
      content += " Size: ";
      content += timeToString(entry.getCreationTime());
      content += " Modified:";
      content += timeToString(entry.getLastWrite());
      content += formatBytes(entry.size());
      content += " Creation:";
      content += "\n";
      server->sendContent(content);
      entry.close();
      filesFound = true;
    } else if(dir.isDirectory()) {
      String content;
      String name = dir.fileName();
      name.replace(server->arg(0), "");
      if(name.charAt(0) == '/') {
        name.remove(0, 1);
      }
      content += name;
      content += " Creation:";
      content += timeToString(dir.fileCreationTime());
      content += "\n";
      server->sendContent(content);
      filesFound = true;
    }
  }
  if(!filesFound)
    server->sendContent(" ");
  server->client().stop();
}

#if USE_HTTPS
void webBrowser::mkdir(BearSSL::ESP8266WebServerSecure *server) {
#else
#if defined(ESP8266)
void webBrowser::mkdir(ESP8266WebServer *server) {
#else
void webBrowser::mkdir(WebServer *server) {
#endif
#endif
  String p = path;
  p += server->arg(0);
  if(SPIFFS.mkdir(p)) {
    server->send(200, "text/plain", " ");
    return;
  }
  String r = "mkdir: cannot create directory ‘";
  r += server->arg(0);
  r += "’: File exists";
  server->send(200, "text/plain", r);
}

#if USE_HTTPS
void webBrowser::rm(BearSSL::ESP8266WebServerSecure *server) {
#else
void webBrowser::rm(ESP8266WebServer *server) {
#endif
  String p = path;
  if(server->arg(0)[0] == '/') {
    p = server->arg(0);
  } else {
    p += server->arg(0);
  }
  if(server->argName(1).compareTo("-r")) {
    if(!SPIFFS.rmdir(p)) {
      server->send(200, "text/plain", "Not exists");
    }
  } else {
    if(!SPIFFS.remove(p)) {
      server->send(200, "text/plain", "Not exists");
    }
  }
}

#if USE_HTTPS
void webBrowser::read(BearSSL::ESP8266WebServerSecure *server) {
#else
#if defined(ESP8266)
void webBrowser::read(ESP8266WebServer *server) {
#else
void webBrowser::read(WebServer *server) {
#endif
#endif
  String p = path;
  if(server->arg(0)[0] == '/') {
    p = server->arg(0);
  } else {
    p += server->arg(0);
  }
  if(!SPIFFS.exists(p)) {
    server->send(200, "text/plain", "Not found.");
    return;
  }
  File pFile = SPIFFS.open(p, "r");
  if(!pFile.size()) {
    server->send(200, "text/plain", " ");
    return;
  }
  server->setContentLength(CONTENT_LENGTH_UNKNOWN);
  server->send(200, "text/plain", "");
  char buf[64];
  char hexBuf[3];
  while(1) {
    int lenRead = pFile.readBytes(buf, sizeof(buf));
    if(!lenRead)
      return;
    int cnt = 0;
    for(; cnt < lenRead; cnt++) {
      util_char_to_hex(hexBuf, buf[cnt]);
      server->sendContent(hexBuf);
    }
    server->sendContent("\n");
  }
  server->client().stop();
}

#endif // !defined(ESP8266) || defined(ESP32)

