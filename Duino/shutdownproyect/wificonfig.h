const char wificonf[] PROGMEM = R"=====(
<!DOCTYPE html>
<html><head>
    <title> WifiConfig </title>
<style type="text/css">
p {
  background-color: rgb(99, 99, 99);
  height: 20%;
  width: 97%;
  object-position: center;
}
input{
  font-size:larger;
  width: 70%;
  height: 80%;
  background-color: rgb(202, 202, 202); 
  border-left-color:rgb(51, 51, 51); 
  border-bottom-color: rgb(138, 138, 138); 
  border-right-color:rgb(138, 138, 138);
  position: relative;
  float: right;
  top: 10%;
  left: 1%;
  margin-right: 2%

}
font{
  font-size:6vh;
  width: 20%;
  height: 80%;
  position: relative;
  top: 35%;
}

a{
   font-size: 10vh;
   width:100%;
   color: rgb(99, 99, 99);
   position:relative;
   top:30%;
   left:10%
}


section.Toques{
display: inline-block; 
background-color: rgb(116, 116, 116); 
position: relative; 
left: 20%;
top: 10%; 
height: 10vh; 
width: 40vh;
}

font.otro{
  font-size:6vh;
  top: 20%;
  left:10%;
}


@media (orientation: portrait) {
font{
  font-size:6vw;
}

font.otro{
  font-size:6vw;
  top: 20%;
  left:10%;
}


a{
   font-size: 10vw;
}

section.Toques{
 height: 10vh;
  width: 40vh;
}



}





</style>  </head>
<body style="background-color:rgb(59, 59, 59);">

 <header id="main-header" style="height: 20%;background-color:rgb(0, 0, 0) ;">
  <a class="site-name">Configurador WiFi</a>
 </header>

  <div style="width: 100%;height: 80%;"> 
    <p> <font color="#cccccc" style="position:relative;top: 20%"> Config. Modo Station: </font> </p>
    <p>
     <font color="#cccccc";>  SSID: </font> 
     <input type="text" id="SSID" name="SSID"> 
  </p>
    <p> 
      <font color="#cccccc">  Password: </font> 
      <input type="text" id="PASS" name="PASS">
    </p>
    <section class="Toques" id="Inf0" onclick="sendwifiS();" onmouseover="style.backgroundColor='#949494'" onmouseout="style.backgroundColor='#747474'"><font class="otro" color="#222222">Cargar Conf.</font></section>
  </div>
    
<script>
function sendwifiS(){
  var valssid = document.getElementById('SSID').value;
  var valpass = document.getElementById('PASS').value;
  
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
     if (this.readyState == 4 && this.status == 200) {
      var txt = this.responseText;
     }
  };
  xhttp.open("GET", "/WCONF?ssid="+valssid+"&pass="+valpass, true);
  xhttp.send();
}


function pedirconf(){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
     if (this.readyState == 4 && this.status == 200) {
      var txt = this.responseText;
     }
  };
  xhttp.open("GET", "/getWCONF", true);
  xhttp.send();
}
</script>
</body></html>
)=====";
