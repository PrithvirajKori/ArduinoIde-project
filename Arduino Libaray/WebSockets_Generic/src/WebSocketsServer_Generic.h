/****************************************************************************************************************************
  WebSocketsServer_Generic.h - WebSockets Library for boards
  
  Based on and modified from WebSockets libarary https://github.com/Links2004/arduinoWebSockets
  to support other boards such as  SAMD21, SAMD51, Adafruit's nRF52 boards, etc.
  
  Built by Khoi Hoang https://github.com/khoih-prog/WebSockets_Generic
  Licensed under MIT license
   
  @original file WebSocketsServer.h
  @date 20.05.2015
  @author Markus Sattler

  Copyright (c) 2015 Markus Sattler. All rights reserved.
  This file is part of the WebSockets for Arduino.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  Version: 2.3.2

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  2.1.3   K Hoang      15/05/2020 Initial porting to support SAMD21, SAMD51, nRF52 boards, such as AdaFruit Feather nRF52832,
                                  nRF52840 Express, BlueFruit Sense, Itsy-Bitsy nRF52840 Express, Metro nRF52840 Express, etc.
  2.2.1   K Hoang      18/05/2020 Bump up to sync with v2.2.1 of original WebSockets library
  2.2.2   K Hoang      25/05/2020 Add support to Teensy, SAM DUE and STM32. Enable WebSocket Server for new supported boards.
  2.2.3   K Hoang      02/08/2020 Add support to W5x00's Ethernet2, Ethernet3, EthernetLarge Libraries. 
                                  Add support to STM32F/L/H/G/WB/MP1 and Seeeduino SAMD21/SAMD51 boards.
  2.3.1   K Hoang      07/10/2020 Sync with v2.3.1 of original WebSockets library. Add ENC28J60 EthernetENC library support
  2.3.2   K Hoang      12/11/2020 Add RTL8720DN Seeed_Arduino_rpcWiFi library support
 *****************************************************************************************************************************/

#pragma once

#include "WebSockets_Generic.h"

#ifndef WEBSOCKETS_SERVER_CLIENT_MAX
#define WEBSOCKETS_SERVER_CLIENT_MAX (5)
#endif

class WebSocketsServer : protected WebSockets
{
  public:
#ifdef __AVR__
    typedef void (*WebSocketServerEvent)(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
    typedef bool (*WebSocketServerHttpHeaderValFunc)(String headerName, String headerValue);
#else
    typedef std::function<void(uint8_t num, WStype_t type, uint8_t * payload, size_t length)> WebSocketServerEvent;
    typedef std::function<bool(String headerName, String headerValue)> WebSocketServerHttpHeaderValFunc;
#endif

    WebSocketsServer(uint16_t port, String origin = "", String protocol = "arduino");
    virtual ~WebSocketsServer(void);

    //KH New Debug
    void displayClientData(WSclient_t *client, bool onlyTCPValid = true);
    
    void begin(void);
    void close(void);

#if(WEBSOCKETS_NETWORK_TYPE != NETWORK_ESP8266_ASYNC)
    void loop(void);
#else
    // Async interface not need a loop call
    void loop(void) __attribute__((deprecated)) {}
#endif

    void onEvent(WebSocketServerEvent cbEvent);
    void onValidateHttpHeader(
      WebSocketServerHttpHeaderValFunc validationFunc,
      const char * mandatoryHttpHeaders[],
      size_t mandatoryHttpHeaderCount);

    bool sendTXT(uint8_t num, uint8_t * payload, size_t length = 0, bool headerToPayload = false);
    bool sendTXT(uint8_t num, const uint8_t * payload, size_t length = 0);
    bool sendTXT(uint8_t num, char * payload, size_t length = 0, bool headerToPayload = false);
    bool sendTXT(uint8_t num, const char * payload, size_t length = 0);
    bool sendTXT(uint8_t num, String & payload);

    bool broadcastTXT(uint8_t * payload, size_t length = 0, bool headerToPayload = false);
    bool broadcastTXT(const uint8_t * payload, size_t length = 0);
    bool broadcastTXT(char * payload, size_t length = 0, bool headerToPayload = false);
    bool broadcastTXT(const char * payload, size_t length = 0);
    bool broadcastTXT(String & payload);

    bool sendBIN(uint8_t num, uint8_t * payload, size_t length, bool headerToPayload = false);
    bool sendBIN(uint8_t num, const uint8_t * payload, size_t length);

    bool broadcastBIN(uint8_t * payload, size_t length, bool headerToPayload = false);
    bool broadcastBIN(const uint8_t * payload, size_t length);

    bool sendPing(uint8_t num, uint8_t * payload = NULL, size_t length = 0);
    bool sendPing(uint8_t num, String & payload);

    bool broadcastPing(uint8_t * payload = NULL, size_t length = 0);
    bool broadcastPing(String & payload);

    void disconnect(void);
    void disconnect(uint8_t num);

    void setAuthorization(const char * user, const char * password);
    void setAuthorization(const char * auth);

    int connectedClients(bool ping = false);
    
    bool clientIsConnected(uint8_t num);

    void enableHeartbeat(uint32_t pingInterval, uint32_t pongTimeout, uint8_t disconnectTimeoutCount);
    void disableHeartbeat();

#if (WEBSOCKETS_NETWORK_TYPE == NETWORK_ESP8266) || (WEBSOCKETS_NETWORK_TYPE == NETWORK_ESP8266_ASYNC)\
     || (WEBSOCKETS_NETWORK_TYPE == NETWORK_ESP32) || (WEBSOCKETS_NETWORK_TYPE == NETWORK_RTL8720DN)
    IPAddress remoteIP(uint8_t num);
#endif

  protected:
  
    // KH Debug
    uint8_t currentActiveClient = 0xFF;
    //////
    
    uint16_t _port;
    String     _origin;
    String    _protocol;
    String    _base64Authorization;    ///< Base64 encoded Auth request
    String *  _mandatoryHttpHeaders;
    size_t    _mandatoryHttpHeaderCount;

    WEBSOCKETS_NETWORK_SERVER_CLASS * _server;

    WSclient_t _clients[WEBSOCKETS_SERVER_CLIENT_MAX];

    WebSocketServerEvent _cbEvent;
    WebSocketServerHttpHeaderValFunc _httpHeaderValidationFunc;

    bool _runnning;

    uint32_t  _pingInterval;
    uint32_t  _pongTimeout;
    uint8_t   _disconnectTimeoutCount;

    bool newClient(WEBSOCKETS_NETWORK_CLASS * TCPclient);

    void messageReceived(WSclient_t * client, WSopcode_t opcode, uint8_t * payload, size_t length, bool fin);

    void clientDisconnect(WSclient_t * client);
    bool clientIsConnected(WSclient_t * client);

#if(WEBSOCKETS_NETWORK_TYPE != NETWORK_ESP8266_ASYNC)
    void handleNewClients(void);
    void handleClientData(void);
#endif

    void handleHeader(WSclient_t * client, String * headerLine);

    void handleHBPing(WSclient_t * client);    // send ping in specified intervals

    /**
           called if a non Websocket connection is coming in.
           Note: can be override
           @param client WSclient_t *  ptr to the client struct
    */
    virtual void handleNonWebsocketConnection(WSclient_t * client)
    {
      WS_LOGDEBUG1("[WS-Server handleHeader] no Websocket connection close. Client =", client->num); 
      
      client->tcp->write(
        "HTTP/1.1 400 Bad Request\r\n"
        "Server: arduino-WebSocket-Server\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 32\r\n"
        "Connection: close\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n"
        "This is a Websocket server only!");
        
      clientDisconnect(client);
    }

    /**
           called if a non Authorization connection is coming in.
           Note: can be override
           @param client WSclient_t *  ptr to the client struct
    */
    virtual void handleAuthorizationFailed(WSclient_t * client)
    {
      client->tcp->write(
        "HTTP/1.1 401 Unauthorized\r\n"
        "Server: arduino-WebSocket-Server\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 45\r\n"
        "Connection: close\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "WWW-Authenticate: Basic realm=\"WebSocket Server\""
        "\r\n"
        "This Websocket server requires Authorization!");
        
      clientDisconnect(client);
    }

    /**
           called for sending a Event to the app
           @param num uint8_t
           @param type WStype_t
           @param payload uint8_t
           @param length size_t
    */
    virtual void runCbEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
    {
      if (_cbEvent)
      {
        _cbEvent(num, type, payload, length);
      }
    }

    /*
           Called at client socket connect handshake negotiation time for each http header that is not
           a websocket specific http header (not Connection, Upgrade, Sec-WebSocket-*)
           If the custom httpHeaderValidationFunc returns false for any headerName / headerValue passed, the
           socket negotiation is considered invalid and the upgrade to websockets request is denied / rejected
           This mechanism can be used to enable custom authentication schemes e.g. test the value
           of a session cookie to determine if a user is logged on / authenticated
    */
    virtual bool execHttpHeaderValidation(String headerName, String headerValue)
    {
      if (_httpHeaderValidationFunc)
      {
        //return the value of the custom http header validation function
        return _httpHeaderValidationFunc(headerName, headerValue);
      }
      
      //no custom http header validation so just assume all is good
      return true;
    }

  private:
    /*
           returns an indicator whether the given named header exists in the configured _mandatoryHttpHeaders collection
           @param headerName String ///< the name of the header being checked
    */
    bool hasMandatoryHeader(String headerName);
};

#include "WebSocketsServer_Generic-Impl.h"

