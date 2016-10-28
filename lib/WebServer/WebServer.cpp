#include "WebServer.h"

WebServer::WebServer(Config *aConf) {
  conf = aConf;
  initEthernet();
  initServer();
}

void WebServer::initEthernet() {
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  IPAddress ip(192, 168, 2, 241);
  Ethernet.begin(mac, ip);
}

void WebServer::initServer() {
  const int PORT = 23;
  server = new EthernetServer(PORT);
  server->begin();
}

void WebServer::handleClient() {
  EthernetClient client = server->available();
  if (client) {
    String data = "";
    data.reserve(client.available());

    while (client.available()) {
      char ch = client.read();
      data += String(ch);
    }

    data.toLowerCase();
    data.trim();

    handleRequest(client, data);
  }
}

void WebServer::handleRequest(EthernetClient &client, String data) {
  if (data.equals("get")) {
    handleGet(client, data);
  } else if (data.startsWith("set ")) {
    data.remove(0, 4); // skip "set "
    handleSet(client, data);
  }

  delay(1);
  client.stop();
}

void WebServer::handleGet(EthernetClient &client, String data) {
  String response = "";
  response.reserve(200);

  response += "{";
  appendFirst(response, "tz", conf->tz);
  append(response, "tw", conf->tw);
  append(response, "tkw", conf->tkw);
  append(response, "tskw", conf->tskw);
  append(response, "tco", conf->tco);
  append(response, "tb", conf->tb);
  append(response, "ztb", conf->ztb);
  append(response, "tcwu", conf->tcwu);
  append(response, "tkg", conf->tkg);
  append(response, "tp", conf->tp);
  append(response, "ztcwu", conf->ztcwu);
  append(response, "kg", conf->kg);
  append(response, "ptw", conf->ptw);
  append(response, "zth", conf->zth);
  append(response, "tcwuh", conf->tcwuh);
  append(response, "minrb", conf->minrb);
  append(response, "maxrb", conf->maxrb);
  append(response, "ztos", conf->ztos);
  append(response, "ztzs", conf->ztzs);
  append(response, "ipa", conf->ipa);
  response += "}";

  if (client) {
    client.println(response);
    client.flush();
  }
}

void WebServer::handleSet(EthernetClient &client, String data) {
  while (data.indexOf('=') != -1) {
    int equalSign = data.indexOf('=');
    int delimeter = data.indexOf(';');

    if (equalSign == -1 || delimeter == -1) {
      return;
    }

    String name = data.substring(0, equalSign);
    String str = data.substring(equalSign + 1, delimeter);
    float value = str.toFloat();

    if (value == 0.00 && !str.startsWith("0.00")) {
      return;
    }

    conf->setProperty(name, value);
    data.remove(0, delimeter);
    data.trim();
  }
}

void WebServer::appendFirst(String &response, String name, float value) {
  response += "\"" + name + "\":" + String(value);
}

void WebServer::append(String &response, String name, float value) {
  response += ",\"" + name + "\":" + String(value);
}
