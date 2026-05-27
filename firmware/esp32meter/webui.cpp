#include <Arduino.h>
#include <WebServer.h>
#include "config.h"
#include "measure.h"
#include "webui.h"

// ---------------------------------------------------------------------------
// Page HTML — entirely self-contained, no CDN dependencies
// ---------------------------------------------------------------------------

static const char PAGE[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32 Meter</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',sans-serif;background:#f1f5f9;color:#0f172a;min-height:100vh}
header{background:#0f172a;color:#f8fafc;padding:14px 20px;display:flex;align-items:center;justify-content:space-between;position:sticky;top:0}
h1{font-size:1.1rem;font-weight:600;letter-spacing:.02em}
.dot{width:10px;height:10px;border-radius:50%;background:#22c55e;transition:background .4s}
.dot.off{background:#475569}
main{padding:16px;max-width:480px;margin:0 auto;display:grid;gap:12px}
.card{background:#fff;border-radius:14px;padding:20px 22px;box-shadow:0 1px 4px rgba(0,0,0,.08);border:2px solid transparent;transition:border-color .2s,color .2s}
.card.beep{border-color:#22c55e}
.lbl{font-size:.7rem;font-weight:700;letter-spacing:.1em;text-transform:uppercase;color:#64748b;margin-bottom:10px}
.val{font-size:2.4rem;font-weight:200;line-height:1;color:#0f172a;font-variant-numeric:tabular-nums}
.card.beep .val{color:#16a34a}
.sub{font-size:.82rem;color:#94a3b8;margin-top:8px;min-height:1.2em}
.badge{display:inline-flex;align-items:center;padding:2px 10px;border-radius:20px;font-size:.75rem;font-weight:700}
.badge.green{background:#dcfce7;color:#15803d}
button{display:block;margin-top:14px;padding:10px 22px;border:none;border-radius:10px;background:#0ea5e9;color:#fff;font-size:.95rem;font-weight:600;cursor:pointer}
button:active{background:#0284c7}
button:disabled{background:#cbd5e1;cursor:default}
</style>
</head>
<body>
<header>
  <h1>ESP32 Meter</h1>
  <div class="dot" id="dot"></div>
</header>
<main>
  <div class="card">
    <div class="lbl">Voltage</div>
    <div class="val" id="voltage">&#x2014;</div>
    <div class="sub" id="vsub">Connect probes to measure DC voltage (0&#x2013;33 V)</div>
  </div>
  <div class="card" id="cr">
    <div class="lbl">Resistance</div>
    <div class="val" id="resistance">&#x2014;</div>
    <div class="sub" id="rsub">Connect probes across a resistor</div>
  </div>
  <div class="card">
    <div class="lbl">Diode / LED</div>
    <div class="val" id="dvf">&#x2014;</div>
    <div class="sub" id="dtype">Red probe on anode (+), black on cathode (&#x2212;)</div>
  </div>
  <div class="card">
    <div class="lbl">Capacitance</div>
    <div class="val" id="cap">&#x2014;</div>
    <div class="sub" id="capsub">Connect a capacitor then press Measure</div>
    <button id="capbtn" onclick="measureCap()">Measure</button>
  </div>
</main>
<script>
function fmtR(o){
  if(o>=1e6)return(o/1e6).toFixed(2)+' MΩ';
  if(o>=1e3)return(o/1e3).toFixed(2)+' kΩ';
  return o.toFixed(1)+' Ω';
}

function poll(){
  fetch('/data').then(function(r){return r.json();}).then(function(d){
    document.getElementById('dot').className='dot';

    document.getElementById('voltage').textContent=d.voltage.toFixed(2)+' V';

    var cr=document.getElementById('cr');
    var rsub=document.getElementById('rsub');
    if(d.open){
      document.getElementById('resistance').textContent='Open';
      rsub.textContent='';
      cr.className='card';
    } else if(d.continuity){
      document.getElementById('resistance').textContent=fmtR(d.resistance);
      rsub.innerHTML='<span class="badge green">● CONTINUITY<\/span>';
      cr.className='card beep';
    } else {
      document.getElementById('resistance').textContent=fmtR(d.resistance);
      rsub.textContent=d.r_range;
      cr.className='card';
    }

    document.getElementById('dvf').textContent=d.diode_vf.toFixed(2)+' V';
    document.getElementById('dtype').textContent=d.diode_type;

  }).catch(function(){
    document.getElementById('dot').className='dot off';
  });
}

function measureCap(){
  var btn=document.getElementById('capbtn');
  var val=document.getElementById('cap');
  var sub=document.getElementById('capsub');
  btn.disabled=true;
  btn.textContent='Measuring…';
  val.textContent='…';
  sub.textContent='';
  fetch('/cap',{method:'POST'}).then(function(r){return r.json();}).then(function(d){
    val.textContent=d.display;
    sub.textContent='';
    btn.disabled=false;
    btn.textContent='Measure again';
  }).catch(function(){
    val.textContent='Error';
    btn.disabled=false;
    btn.textContent='Measure';
  });
}

poll();
setInterval(poll, 2000);
</script>
</body>
</html>
)HTML";

// ---------------------------------------------------------------------------
// Route handlers
// ---------------------------------------------------------------------------

static WebServer* _srv = nullptr;

static void handleRoot() {
    _srv->send_P(200, "text/html", PAGE);
}

static void handleData() {
    float v    = measureVoltage();
    RResult r  = measureResistance();
    DiodeResult d = measureDiode();

    String j = "{";
    j += "\"voltage\":"    + String(v, 3);
    j += ",\"resistance\":" + String(r.ohms, 1);
    j += ",\"continuity\":" + String(r.continuity  ? "true" : "false");
    j += ",\"open\":"       + String(r.openCircuit ? "true" : "false");
    j += ",\"r_range\":\""  + String(r.range) + "\"";
    j += ",\"diode_vf\":"   + String(d.vf, 3);
    j += ",\"diode_type\":\"" + String(d.type) + "\"";
    j += "}";

    _srv->send(200, "application/json", j);
}

static void handleCap() {
    CapResult c = measureCapacitance();
    String disp = formatCapacitance(c);

    String j = "{";
    j += "\"display\":\""  + disp + "\"";
    j += ",\"timeout\":"   + String(c.timeout  ? "true" : "false");
    j += ",\"too_small\":" + String(c.tooSmall ? "true" : "false");
    j += "}";

    _srv->send(200, "application/json", j);
}

static void handleCaptive() {
    _srv->sendHeader("Location", "http://192.168.4.1/");
    _srv->send(302, "text/plain", "");
}

// ---------------------------------------------------------------------------

void setupWebServer(WebServer& server) {
    _srv = &server;
    server.on("/",     HTTP_GET,  handleRoot);
    server.on("/data", HTTP_GET,  handleData);
    server.on("/cap",  HTTP_POST, handleCap);
    server.onNotFound(handleCaptive);
}
