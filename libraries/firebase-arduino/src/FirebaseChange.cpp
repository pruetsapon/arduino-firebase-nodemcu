#include "FirebaseChange.h"

void FirebaseChange::begin(String FirebaseProjectName, String ThumbPrint){
	_FirebaseProjectName = FirebaseProjectName;
	_ThumbPrint = ThumbPrint;
  _http.setReuse(true);
}

boolean FirebaseChange::available(String parent = ""){
  _http.begin(_FirebaseProjectName + parent + ".json", _ThumbPrint);
  _http.setUserAgent("Mozilla /5.0");
  _http.addHeader("Content-Type","application/json");

  int httpCode = _http.GET();

  if(httpCode > 0){
    _Status = "Firebase success code: " + String(httpCode);
    if(httpCode == HTTP_CODE_OK){
      String payload = _http.getString();
      if(_Payload != payload){
        _Payload = payload;
        return true;
      }
      return false;
    }
  } else {
    _Status = "Firebase error: " + _http.errorToString(httpCode);
    return false;
  }
}

boolean FirebaseChange::setString(String parent, String element, String value){
  _http.begin(_FirebaseProjectName + parent + ".json?x-http-method-override=PATCH", _ThumbPrint);

  String data = "{\"" + element + "\":\"" + value + "\"}";

  int httpCode = _http.POST(data);

  if(httpCode > 0){
    _Status = "Firebase success code: " + String(httpCode);
    if(httpCode == HTTP_CODE_OK){
      String payload = _http.getString();
      _Payload = payload;
      return true;
    }
  } else {
    _Status = "Firebase error: " + _http.errorToString(httpCode);
  }
}

String FirebaseChange::getString(String element){
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(_Payload);
  if(!root.success()){
    return "";
  }
  return root[element];
}

String FirebaseChange::status(){
	return _Status;
}

String FirebaseChange::payload(){
	return _Payload;
}
