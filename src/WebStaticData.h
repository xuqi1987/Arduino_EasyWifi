#ifndef WEBSTATICDATA_h
#define WEBSTATICDATA_h

static const char _HTTP_HEAD[] PROGMEM  = R"(
<!DOCTYPE html><html lang='zh'>
 <head> 
  <meta http-equiv='Content-Type' content='text/html; charset=utf-8' /> 
  <meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no' /> 
  <title>{v}</title> 
)";

static const char _HTTP_STYLE[] PROGMEM  = R"(
<style type='text/css'>
*{margin:0;padding:0;box-sizing:border-box;-webkit-box-sizing:border-box;-moz-box-sizing:border-box;-webkit-font-smoothing:antialiased;-moz-font-smoothing:antialiased;-o-font-smoothing:antialiased;font-smoothing:antialiased;text-rendering:optimizeLegibility}body{font-family:'Open Sans',Helvetica,Arial,sans-serif;font-weight:300;font-size:12px;line-height:30px;color:#777;background:#0CF}.container{max-width:400px;width:100%;margin:0 auto;position:relative}#contact input[type='text'],#contact input[type='button'],#contact button[type='submit']{font:400 12px/16px 'Open Sans',Helvetica,Arial,sans-serif}#contact{background:#f9f9f9;padding:25px;margin:5px 0}#contact h3{color:#F96;display:block;font-size:30px;font-weight:400}#contact h4{margin:5px 0 15px;display:block;font-size:13px}fieldset{border:medium none!important;margin:0 0 10px;min-width:100%;padding:0;width:100%}#contact input[type='text'],#contact input[type='button'],#contact select{width:100%;border:1px solid #CCC;background:#FFF;margin:0 0 5px;padding:10px}#contact select{font-size:15px}#contact input[type='text']:hover,#contact input[type='button']:hover,#contact select:hover{-webkit-transition:border-color .3s ease-in-out;-moz-transition:border-color .3s ease-in-out;transition:border-color .3s ease-in-out;border:1px solid #AAA}#contact button[type='submit']{cursor:pointer;width:100%;border:0;background:#0CF;color:#FFF;margin:0 0 5px;padding:10px;font-size:15px}#contact button[type='submit']:hover{background:#09C;-webkit-transition:background .3s ease-in-out;-moz-transition:background .3s ease-in-out;transition:background-color .3s ease-in-out}#contact button[type='submit']:active{box-shadow:inset 0 1px 3px rgba(0,0,0,0.5)}#contact input:focus{outline:0;border:1px solid #999}::-webkit-input-placeholder{color:#888}:-moz-placeholder{color:#888}::-moz-placeholder{color:#888}:-ms-input-placeholder{color:#888}
</style>
)";
static const char _HTTP_SCRIPT[] PROGMEM = R"(
<script type='text/javascript'>
function on_change(l){document.getElementById('ap_name').value=l.value; document.getElementById('wifi_pass').focus();};
</script> 
)";

static const char _HTTP_SCRIPT_RESET[] PROGMEM = R"(
<script type='text/javascript'>
var t=10;function showTime(){t-=1;document.getElementById('custom_title').innerHTML='保存中..('+ t + ')' ;if(t==0){document.getElementById('custom_title').innerHTML='连接成功后,会自动断开连接!'}else {setTimeout('showTime()',1000)} };showTime();
</script> 
)";

static const char _HTTP_HEAD_END[] PROGMEM = R"(
 <body> 
  <div class='container'> 
   <div id='contact'> 
    <h3 id='custom_title'>设置{v}</h3> 
)";

static const char _HTTP_AP_LIST[] PROGMEM = R"(
 <h4 >请选择需要连接的热点:</h4> 
   <select id='ap_list' name='ap_list' onchange='on_change(this)'> 
        {i}
	</select> 
)";

static const char _HTTP_AP_LIST_ITEM[] PROGMEM = R"(
 <option value='{n}'>  {n}</option>
)";

static const char _HTTP_FORM[] PROGMEM = R"(
    <form action='save' method='post'> 
     <fieldset> 
      <input id='ap_name' placeholder='WiFi名字' type='text' name='ap_name' value= '{v}' tabindex='1' required='' autofocus='' /> 
     </fieldset> 
     <fieldset> 
      <input id='wifi_pass' placeholder='WiFi密码' type='text' name='wifi_pass' tabindex='2' required='' /> 
     </fieldset>
)";

static const char _HTTP_FORM_PARAM[] PROGMEM = R"(
     <fieldset> 
      <input id='{i}' placeholder='{p}' type='text' name='{n}' value='{v}' required='' /> 
     </fieldset> 
)";

static const char _HTTP_SERVER_LIST[] PROGMEM = R"(
     <fieldset> 
	<select id='service_name' name='service_name'> 
		<option value='Switch'>开关</option>
		<option value='Lightbulb'>灯泡</option> 
		<option value='Ws2812b-Lightbulb'>ws2812b彩灯</option>
		<option value='DHT11'>DHT11温湿度</option> 
		<option value='IRremote'>红外</option> 
		<option value='OccupancySensor'>人体传感器</option>
		<option value='AirQualitySensor'>空气质量传感器</option>
		<option value='Fan'>风扇</option> 
		<option value='HumiditySensor'>湿度</option>
		<option value='LightSensor'>光照传感器</option>
		<option value='TemperatureSensor'>温度</option>
	</select>
     </fieldset> 
)";

static const char _HTTP_FORM_END[] PROGMEM = R"(

     <fieldset> 
      <button name='submit' type='submit' id='contact-submit' data-submit='...Sending'>设置并重启</button> 
     </fieldset> 
    </form> 
   </div> 
)";

static const char _HTTP_END[] PROGMEM = R"(
  </div> 
 </body>
</html>
)";

#endif