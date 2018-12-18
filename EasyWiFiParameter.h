#ifndef EasyWifiParameter_h
#define EasyWifiParameter_h


class EasyWifiParameter {
  public:
    /** 
        Create custom parameters that can be added to the EasyWifi setup web page
        @id is used for HTTP queries and must not contain spaces nor other special characters
    */
    EasyWifiParameter(const char *custom);
    EasyWifiParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
    EasyWifiParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);
    ~EasyWifiParameter();

    const char *getID();
    const char *getValue();
    const char *getPlaceholder();
    int         getValueLength();
    const char *getCustomHTML();

  private:
    const char *_id;
    const char *_placeholder;
    char       *_value;
    int         _length;
    const char *_customHTML;

    void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

    friend class EasyWifi;
};

#endif // !EasyWifi
