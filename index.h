static const char Main_Page[] PROGMEM = R"=====(
 
<!DOCTYPE html>
<html>
<head>
<!-- Required meta tags -->
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
 
    <!-- Bootstrap CSS -->
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-QWTKZyjpPEjISv5WaRU9OFeRpok6YctnYmDr5pNlyT2bRjXh0JMhjY6hW+ALEwIH" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-YvpcrYf0tY3lHB60NNkmXc5s9fDVZLESaAA55NDzOxhy9GkcIdslK1eN7N6jIeHz" crossorigin="anonymous"></script>

</head>
 
<body>  
<div class="container px-4">
<div class="p-5">
    <div class="p-5 mr-5 ml-5">

    <div class="mb-3">
      <label for="exampleInputEmail1" class="form-label">Wifi</label>
      <button type="submit" class="btn btn-primary" onclick="SearchWifi()">Cari</button>      
    </div>    
    <div id="pesan" class="pesan">

    </div>
    <div id="wifi_current">

    </div>
    <div class="table-responsive">
        <table id="dataTableExample" class="table">
            <thead>
                <tr>
                    <th>SSID</th>
                    <th>Strenght</th>
                    <th>Security</th>
                    <th>Action</th>
                </tr>
            </thead>
            <tbody id="tbody">
                <tr>
                    <th>SSID</th>
                    <th>Strenght</th>
                    <th>Security</th>
                    <th>
                        <button type="button" class="btn btn-primary" data-bs-toggle="modal" data-bs-target="#exampleModal">
                            Login
                        </button>
                    </th>
                </tr>
            </tbody>
        </table>
    </div>
</div>
</div>
</div>

<!-- Modal -->
<div class="modal fade" id="exampleModal" tabindex="-1" aria-labelledby="exampleModalLabel" aria-hidden="true">
    <div class="modal-dialog modal-dialog-centered">
      <div class="modal-content">
        <div class="modal-header">
          <h1 class="modal-title fs-5" id="exampleModalLabel">Modal title</h1>
          <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
        </div>
        <div class="modal-body">
          <form action="" method="post">
            <div class="mb-3 input-group flex-nowrap" id="modal-body">
                <span class="input-group-text" id="addon-wrapping">SSID</span>
                <input type="text" class="form-control ssid" placeholder="SSID" aria-label="Username" aria-describedby="addon-wrapping" name="ssid" id="ssid">
            </div>
            <div class="mb-3 input-group flex-nowrap">
                <span class="input-group-text" id="addon-wrapping">Pass</span>
                <input type="text" class="form-control" placeholder="Password" aria-label="password" aria-describedby="addon-wrapping" name="pass" id="pass">
            </div>          
        </div>
        <div class="modal-footer">
          <button type="button" class="btn btn-secondary" data-bs-dismiss="modal">Close</button>
          <button type="button" class="btn btn-primary" onclick="httpXhr()">Save changes</button>
        </div>
        </form>
      </div>
    </div>
  </div>

<script>    
var tbody = document.getElementById("tbody") 
var wifi = document.getElementById("wifi_current") 
var login = document.getElementById("login") 
var getSSID = document.getElementById("ssid") 
var getPass = document.getElementById("pass")
var isConnectWifi = false; 

let text = "";

const url1 = "http://192.168.4.1/wifi";
fetch(url1)
.then(res => res.json())
.then(data => {
  console.log(data);
  // if(data.data[0].status != "ok"){
  //  text+= `<tr>${data.data[0].status}</tr>`
  // } else{
    for(let i=0; i < data.data.length; i++) {
    text += `
              <tr>
                  <td>${data.data[i].SSID}</td>
                  <td>${data.data[i].RSSI}</td>
                  <td>${data.data[i].Encrypt}</td>
                  <td>
                        <button type="button" class="btn btn-primary" data-bs-toggle="modal" data-bs-target="#exampleModal" onclick="SSID('${data.data[i].SSID}')">
                            Login
                        </button>
                  </td>
              </tr>
    `;
}
  // }
    
tbody.innerHTML = text;

})
.catch(error => {console.log(error)})

function Submit(){
    const url = "http://192.168.4.1:8080/login-ssid";    
    fetch(url, {
        method: 'POST',
        headers: {
            'Accept': 'application/json',
            'Content-Type': 'application/x-www-form-urlencoded'
        },
        body: JSON.stringify({
            ssid: getSSID.value,
            password: getPass.value
        })
    }).then(res => res.json())
    .then(data => {
        console.log(data)
    })
    .catch(error => {console.log(error)})
}

function disconnect(){
  var http = new XMLHttpRequest();
    http.open("GET", "http://192.168.4.1/disconnect");
    http.responseType = "json";
    http.send();
    http.onload = function(){
      console.log(http.response);      
      wifi.remove();
      location.reload();      
    }
}

function httpXhr(){    
    var param1 = getSSID.value
    var param2 = getPass.value
    var http = new XMLHttpRequest();
    var url = "http://192.168.4.1/login-ssid";
    var params = "ssid="+param1+"&pass="+param2;
    http.open("POST", url, true);
    http.responseType = "json";

    http.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    http.onreadystatechange = function() {
        if(http.readyState == 4 && http.status == 200) {            
            var response = http.response;
            console.log(response.data[0].isLogin);
            isConnectWifi = response.data[0].isLogin;
            setTimeout(() => {
              checkLoginWifi(isConnectWifi);
            }, 2000);
            }

    }
    http.send(params);
}

function checkLoginWifi(isConnectWifi) {
  if(isConnectWifi){
    var http = new XMLHttpRequest();
    http.open("GET", "http://192.168.4.1/get-status-wifi");
    http.responseType = "json";
    http.send();
    http.onload = function(){
      let html = "";
      console.log(http.response);
      var res = http.response;
      if(res.data[0].isConnectWifi){
        document.cookie = `ssid=${res.data[0].ssid}`;
        document.cookie = `isConnectWifi=${res.data[0].isConnectWifi}`;
        console.log(getCookie("ssid"));
        html=`<tr>
                <td>${getCookie("ssid")}</td>
                <td>
                  <button type="button" class="btn btn-primary" onclick="disconnect()">Disconnect</button>
                </td>
              </tr>`;
        let cookies = document.cookie;
        wifi.innerHTML = html;
        alert("Berhasil Terhubung");
        console.log(cookies);
      }else{
        // setTimeout(() => {
        //       checkLoginWifi(auth);
        //     }, 2000);
        html = `Gagal Terhubung`;
        wifi.innerHTML = html;
        alert("Gagal Terhubung");
      }      
    }
  }else{
    console.log("Gagal");
  }
}
              
function SearchWifi(){
let text = "";

const url = "http://192.168.4.1/wifi";
fetch(url)
.then(res => res.json())
.then(data => {
  console.log(data);
  // if(data.data[0].status != "ok"){
  //  text+= `<tr>${data.data[0].status}</tr>`
  // } else{
    for(let i=0; i < data.data.length; i++) {
    text += `
              <tr>
                  <td>${data.data[i].SSID}</td>
                  <td>${data.data[i].RSSI}</td>
                  <td>${data.data[i].Encrypt}</td>
                  <td>
                        <button type="button" class="btn btn-primary" data-bs-toggle="modal" data-bs-target="#exampleModal" onclick="SSID('${data.data[i].SSID}')">
                            Login
                        </button>
                  </td>
              </tr>
    `;
}
  // }
    
tbody.innerHTML = text;

})
.catch(error => {console.log(error)})
}   

function SSID(ssid){
    getSSID.value = ssid;
}

function getCookie(cname) {
  let name = cname + "=";
  let decodedCookie = decodeURIComponent(document.cookie);
  let ca = decodedCookie.split(';');
  for(let i = 0; i <ca.length; i++) {
    let c = ca[i];
    while (c.charAt(0) == ' ') {
      c = c.substring(1);
    }
    if (c.indexOf(name) == 0) {
      return c.substring(name.length, c.length);
      console.log(c.substring(name.length, c.length));
    }
  }
  return "";
}
</script>
</html>
 
)=====";