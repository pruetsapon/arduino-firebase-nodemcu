/* 
  Firebasechange.h - Library for check on chaning of data on specific path
*/
#ifndef FirebaseChange_h
#define FirebaseChange_h

#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

class FirebaseChange
{
  public:
  	void begin(String FirebaseProjectName, String ThumbPrint);
    boolean available(String parent);
    String status();
    String payload();
    String getString(String element);
    boolean setString(String parent, String element, String value);
  private:
    String _Payload = "";
    String _Status = "";
    String _FirebaseProjectName = "";
    String _ThumbPrint = "";
    HTTPClient _http;
};

#endif
