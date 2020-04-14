function xhrSend (s) {
    var xhr = new XMLHttpRequest();
    xhr.open('POST', 'http://localhost/', true);
    xhr.send(s);
    xhr.onreadystatechange = function() {
        if (xhr.readyState == XMLHttpRequest.DONE) {
            document.open();
            document.write(xhr.responseText);
            document.close();
        }
    }
}
document.getElementById('yes').addEventListener("click", yesButtonListener);
document.getElementById('no').addEventListener("click", noButtonListener);
document.getElementsByTagName('body')[0].addEventListener("keypress", yesnoKeyListener);
        //if (strstr(idAttacker, "lrw") && strstr(idTarget, "obj")) {
if (req.includes("lrw") && req2.includes("obj")) {
    var radio = document.getElementsByTagName('input');
    for (var i = 0; i < 3; i++) {
        radio[i].checked = false;
        radio[i].addEventListener("change", changeRadio);
    }
}
function changeRadio() {
    var is;
    if (this.checked == true) {
        is = true;
    }
    else {
        is = false;
    }
    for (var i = 0; i < 3; i++) {
        radio[i].checked = false;
    }
    if (is == true) {
        this.checked = true;
    }
    else {
        this.checked = false;
    }
}
function yesButtonListener() {
    var xhr0 = new XMLHttpRequest();
    xhr0.open('POST', 'http://localhost/', true);
    xhr0.onload = function() {
        if (localStorage.getItem("logs")) {
            localStorage.setItem("logs", localStorage.getItem("logs") + xhr0.response);
        }
        else {
            localStorage.setItem("logs", xhr0.response);
        }
        localStorage.setItem("logs", localStorage.getItem("logs") + "<p>Тест на дальность пройден</p>");
        if (req.includes("lrw") && req2.includes("obj")) {
            var radio = document.getElementsByTagName('input');
            if (radio[0].checked || radio[1].checked || radio[2].checked) {
                var s;
                if (radio[0].checked) {
                    s = 0;
                }
                else if (radio[1].checked) {
                    s = 1;
                }
                else if (radio[2].checked) {
                    s = 2;
                }
                var xhr = new XMLHttpRequest();
                xhr.open('POST', 'http://localhost/', true);
                xhr.onload = function() {
                    var div = document.createElement("div");
                    div.innerHTML = xhr.response.split("//logs//")[0];
                    document.body.appendChild(div);
                    document.body.removeChild(document.getElementById("comparedistance"));
                    var s = document.createElement("script");
                    s.src = "http://localhost/checkkill.js";
                    document.head.appendChild(s);
                    localStorage.setItem("logs", localStorage.getItem("logs") + xhr.response.split("//logs//")[1]);
                }
                xhr.send("testshot:checkkill=" + s + ";");
            }
            else {
                alert("Выберите дальность!");
            }
        }
        else {
            var xhr = new XMLHttpRequest();
            xhr.open('POST', 'http://localhost/', true);
            xhr.onload = function() {
                var div = document.createElement("div");
                div.innerHTML = xhr.response.split("//logs//")[0];
                document.body.appendChild(div);
                document.body.removeChild(document.getElementById("comparedistance"));
                var s = document.createElement("script");
                s.src = "http://localhost/checkkill.js";
                document.head.appendChild(s);
                localStorage.setItem("logs", localStorage.getItem("logs") + xhr.response.split("//logs//")[1]);
            }
            xhr.send("testshot:checkkill=1000;");
        }
    }
    xhr0.send("logs:getattackerdefender;");
}
function noButtonListener() {
    var xhr = new XMLHttpRequest();
    xhr.open('POST', 'http://localhost/', true);
    xhr.onload = function() {
        if (localStorage.getItem("logs")) {
            localStorage.setItem("logs", localStorage.getItem("logs") + xhr.response);
        }
        else {
            localStorage.setItem("logs", xhr.response);
        }
        localStorage.setItem("logs", localStorage.getItem("logs") + "<p>Тест на дальность провален</p>");
        localStorage.setItem("logs", localStorage.getItem("logs") + "</div>");
        xhrSend("testshot:chooseattacker;");
    }
    xhr.send("logs:getattackerdefender;");
}
function yesnoKeyListener(e) {
    var xhr0 = new XMLHttpRequest();
    xhr0.open('POST', 'http://localhost/', true);
    xhr0.onload = function() {
        if (localStorage.getItem("logs")) {
            localStorage.setItem("logs", localStorage.getItem("logs") + xhr0.response);
        }
        else {
            localStorage.setItem("logs", xhr0.response);
        }
        localStorage.setItem("logs", localStorage.getItem("logs") + "<p>Тест на дальность пройден</p>");
        if (req.includes("lrw") && req2.includes("obj")) {
            if (e.keyCode == 13) {
                var radio = document.getElementsByTagName('input');
                if (radio[0].checked || radio[1].checked || radio[2].checked) {
                    var s;
                    if (radio[0].checked) {
                        s = 0;
                    }
                    else if (radio[1].checked) {
                        s = 1;
                    }
                    else if (radio[2].checked) {
                        s = 2;
                    }
                    var xhr = new XMLHttpRequest();
                    xhr.open('POST', 'http://localhost/', true);
                    xhr.onload = function() {
                        var div = document.createElement("div");
                        div.innerHTML = xhr.response.split("//logs//")[0];
                        document.body.appendChild(div);
                        document.body.removeChild(document.getElementById("comparedistance"));
                        var s = document.createElement("script");
                        s.src = "http://localhost/checkkill.js";
                        document.head.appendChild(s);
                        localStorage.setItem("logs", localStorage.getItem("logs") + xhr.response.split("//logs//")[1]);
                    }
                    xhr.send("testshot:checkkill=" + s + ";");
                }
                else {
                    alert("Выберите дальность!");
                }
            }
        }
        else {
            if (e.keyCode == 13) {
                var xhr = new XMLHttpRequest();
                xhr.open('POST', 'http://localhost/', true);
                xhr.onload = function() {
                    var div = document.createElement("div");
                    div.innerHTML = xhr.response.split("//logs//")[0];
                    document.body.appendChild(div);
                    document.body.removeChild(document.getElementById("comparedistance"));
                    var s = document.createElement("script");
                    s.src = "http://localhost/checkkill.js";
                    document.head.appendChild(s);
                    localStorage.setItem("logs", localStorage.getItem("logs") + xhr.response.split("//logs//")[1]);
                }
                xhr.send("testshot:checkkill=1000;");
            }
        }
    }
    xhr0.send("logs:getattackerdefender;");
    //if (e.keyCode == 27) {
    //    xhrSend("menu");
    //}
}