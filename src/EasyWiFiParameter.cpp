#include "EasyWiFiParameter.h"

EasyWifiParameter::EasyWifiParameter(const char *custom) {
  _id = NULL;
  _placeholder = NULL;
  _length = 0;
  _value = NULL;
  _customHTML = custom;
}

EasyWifiParameter::EasyWifiParameter(const char *id, const char *placeholder, const char *defaultValue, int length) {
  init(id, placeholder, defaultValue, length, "");
}

EasyWifiParameter::EasyWifiParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom) {
  init(id, placeholder, defaultValue, length, custom);
}

void EasyWifiParameter::init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom) {
  _id = id;
  _placeholder = placeholder;
  _length = length;
  _value = new char[length + 1];
  for (int i = 0; i < length + 1; i++) {
    _value[i] = 0;
  }
  if (defaultValue != NULL) {
    strncpy(_value, defaultValue, length);
  }

  _customHTML = custom;
}

EasyWifiParameter::~EasyWifiParameter() {
  if (_value != NULL) {
    delete[] _value;
  }
}

const char* EasyWifiParameter::getValue() {
  return _value;
}
const char* EasyWifiParameter::getID() {
  return _id;
}
const char* EasyWifiParameter::getPlaceholder() {
  return _placeholder;
}
int EasyWifiParameter::getValueLength() {
  return _length;
}
const char* EasyWifiParameter::getCustomHTML() {
  return _customHTML;
}
