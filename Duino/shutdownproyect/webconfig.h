const char pagename[] PROGMEM = R"=====(
<html><head>
    <title> WifiConfig </title>
<style type="text/css">
/* Estilos generales */
body {
  background-color: rgb(59, 59, 59);
  margin: 0;
  padding: 0;
  font-size: 16px;
  line-height: 1.5;
}

/* Encabezado */
#main-header {
  height: 10vh;
  background-color: rgb(0, 0, 0);
  display: flex;
  justify-content: center;
  align-items: center;
}

.site-name {
  color: white;
  font-size: 3vh;
  margin: 0;
}

/* Contenido */
.container {
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  height: 90vh;
}

.title {
  color: #cccccc;
  font-size: 6vw;
  text-align: center;
  margin-bottom: 10%;
}

.input-label {
  color: #cccccc;
  font-size: 4vw;
  margin-bottom: 2%;
  width: 70%;
}

.input {
  font-size: 3.5vw;
  width: 70%;
  height: 10%;
  background-color: rgb(202, 202, 202);
  border-left-color: rgb(51, 51, 51);
  border-bottom-color: rgb(138, 138, 138);
  border-right-color: rgb(138, 138, 138);
}

.button {
  display: flex;
  justify-content: center;
  align-items: center;
  background-color: rgb(116, 116, 116);
  color: white;
  height: 10vh;
  /*width: 40vh;*/
  max-width: 100%; /* Esto establece el ancho máximo */
  width: auto; /* Esto permite que el contenido se ajuste automáticamente */

  font-size: 4vw;
  cursor: pointer;
  margin-top: 10%;
}

button.Toques {
  display: inline-block; 
  background-color: rgb(116, 116, 116); 
  position: relative; 
  left: 20%;
  top: 10%; 
  height: 8vh; 
  width: 30vh;
  border: none;
  border-radius: 5px;
  color: white;
  font-size: 4vh;
  cursor: pointer;
}

#ip-list {
  border: 1px solid #ccc;
  padding: 5px;
  background-color: #f0f0f0;
}

.ip-list-container {
  max-width: 80%; /* Cambia el valor para ajustar el ancho máximo */
  margin: 0 auto; /* Esto centrará el contenedor en la pantalla */
}

.container {
  margin-bottom: 50px; /* Cambia el valor para ajustar el margen inferior */
}

/* Estilo para la lista */
ul {
  list-style-type: none; /* Eliminamos los puntos de la lista */
  margin: 0;
  padding: 0;
  overflow: auto; /* Hacemos que la lista tenga un scrollbar si hay demasiados elementos */
  border: 1px solid #ddd; /* Añadimos un borde */
  border-radius: 5px; /* Añadimos un borde redondeado */
}

/* Estilo para cada elemento de la lista */
li {
  padding: 12px 16px;
  border-bottom: 2px solid #b5b5b5;/* Añadimos un borde inferior */
  font-size: 18px;
  border-radius: 0.5em;
}

/* Estilo para el elemento seleccionado */
li.selected {
  background-color: #ddd; /* Cambiamos el color de fondo del elemento seleccionado */
}

@media (orientation: portrait) {
  button.Toques {
    height: 10vh;
    width: 40vh;
    font-size: 4vw;
  }
}

/* Responsive */
@media (max-width: 768px) {
  .site-name {
    font-size: 5vw;
  }
  .title {
    font-size: 8vw;
  }
  .input-label {
    font-size: 6vw;
  }
  .input {
    font-size: 5vw;
  }
  .button {
    font-size: 6vw;
  }
}

@media (min-width: 768px) and (max-width: 1024px) {
  .site-name {
    font-size: 2.5vh;
  }
  .title {
    font-size: 4vw;
  }
  .input-label {
    font-size: 3vw;
  }
  .input {
    font-size: 2.5vw;
  }
  .button {
    font-size: 3vw;
  }
}

@media (min-width: 1024px) {
  .site-name {
    font-size: 3vh;
  }
  .title {
    font-size: 3vw;
  }
  .input-label {
    font-size: 2vw;
  }
  .input {
    font-size: 1.8vw;
  }
  .button {
    font-size: 2.2vw;
  }
}
</style>  </head>


<body style="background-color:rgb(59, 59, 59);">
  <header id="main-header" style="height: 20%;background-color:rgb(0, 0, 0) ;">
    <a class="site-name">Configurador WiFi</a>
  </header>

  <div style="width: 100%;height: 50%;"> 
    <p> <font color="#cccccc" style="position:relative;top: 20%"> Config. Modo Station: </font> </p>
    <p>
      <font color="#cccccc"> SSID: </font> 
      <input type="text" id="SSID" name="SSID" maxlength="15"> 
    </p>
    <p> 
      <font color="#cccccc"> Password: </font> 
      <input type="text" id="PASS" name="PASS" maxlength="15">
    </p>
    <button class="Toques" id="Inf0" onclick="sendwifiS();" onmouseover="style.backgroundColor='#949494'" onmouseout="style.backgroundColor='#747474'">
      <font class="otro" color="#222222">Cargar Conf.</font>
    </button>
  </div>

 <div style="width: 100%;height: 30%;"> 
   <ul id="ip-list">
  </ul>


  <button onclick="agregarDireccionIP()">Agregar direccion IP</button>
  <button onclick="eliminarDireccionIP()">Eliminar direccion IP</button>

  <script>
  var lista = document.getElementById("ip-list");
  lista.addEventListener("click", function(e) {
    var item = e.target;
    if (item.tagName == "LI") {
      var items = lista.getElementsByTagName("li");
      for (var i = 0; i < items.length; i++) {
        items[i].classList.remove("seleccionado");
      }
      item.classList.add("seleccionado");
    }
  });
  </script>
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
  document.getElementById("Inf0").innerHTML = "<font class=\"otro\" color=\"#FFFFFF\">Cargardo</font>";
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

count=0;
function agregarDireccionIP() {
  var ip = prompt("Ingrese la direccion IP:");
  // Si el usuario cancela el prompt o no ingresa una dirección IP, no se agrega nada a la lista
  if (ip == null || ip == "") {
    return;
  }
  let lista = document.getElementById("ip-list");
  
  // Verificar si la IP es válida
  if (validarIP(ip)) {
    let elemento = document.createElement("li");
    elemento.textContent = ip;
    elemento.id = "ip" + count;  // Asignar un ID único a cada elemento
    elemento.onclick = function() { highlightIP(elemento.id); };  // Agregar el atributo onclick   
    lista.appendChild(elemento);
    count++;
    actualizarips();
  }
  else{
    alert("IP no valida, BOBO que mira")
  }
}

function actualizarips(){
  var ipList = document.getElementById("ip-list");
  var ips = [];
  var params = ""
  var Nips = ipList.children.length
  for (var i = 0; i < Nips-1; i++) {
      var ip = ipList.children[i].textContent.trim();
      ips.push(ip);
      params+=i.toString()+"="+ip+"&"
    }
  var last = ipList.children.length-1
  if(ipList.children.length>0){
    var ip = ipList.children[last].textContent.trim();
    ips.push(ip);
    params+=last.toString()+"="+ip

    var xhttp = new XMLHttpRequest();
    xhttp.open("POST", "/refresh", true);
    xhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    xhttp.send(params);
  }
  else{
    var xhttp = new XMLHttpRequest();
    xhttp.open("POST", "/clear", true);
    xhttp.send();
  }
}



function eliminarDireccionIP() {
  var lista = document.getElementById("ip-list");
  var items = lista.getElementsByTagName("li");
  var seleccionado = -1;
  for (var i = 0; i < items.length; i++) {
    if (items[i].classList.contains("seleccionado")) {
      seleccionado = i;
      break;
    }
  }
  if (seleccionado >= 0) {
    lista.removeChild(items[seleccionado]);
  }
  actualizarips();
}
function validarIP(ip) {
  // Expresión regular para comprobar que la dirección IP tenga el formato correcto
  var regexIP = /^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$/;
  
  if(regexIP.test(ip)) { // Si la dirección IP es válida
    // Separamos la dirección IP en sus cuatro partes
    var partesIP = ip.split(".");
    
    // Comprobamos que cada parte de la dirección IP esté dentro del rango adecuado
    for(var i=0; i<4; i++) {
      var parte = parseInt(partesIP[i]);
      if(parte < 0 || parte > 255) { // Si alguna parte está fuera de rango, la dirección IP no es válida
        return false;
      }
    }
    // Si hemos llegado hasta aquí, la dirección IP es válida
    return true;
  } else { // Si la dirección IP no tiene el formato adecuado, no es válida
    return false;
  }
}
function highlightIP(element) {
  var ipList = document.getElementById("ip-list");
  for (var i = 0; i < ipList.children.length; i++) {
    ipList.children[i].style.backgroundColor = "white";
  }
  elemento = document.getElementById(element);
  elemento.style.backgroundColor = "yellow";
}


function getipaddress(){
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "/getdata", true);
    xhttp.send();
    xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var txt = this.responseText;
      console.log(txt);
      count=0;
      datos = JSON.parse(xhttp.response)
      cantidad = Object.keys(datos).length
      for (var i=0; i<cantidad; i++){
        ip = datos[i]
      let lista = document.getElementById("ip-list");
        if (validarIP(ip)) {
          let elemento = document.createElement("li");
          elemento.textContent = ip;
          elemento.id = "ip" + count;  // Asignar un ID único a cada elemento
          elemento.onclick = function() { highlightIP(elemento.id); };  // Agregar el atributo onclick   
          lista.appendChild(elemento);
          count++;
        }
      }
    }
}}

getipaddress();


</script>
</body></html>

)=====";
