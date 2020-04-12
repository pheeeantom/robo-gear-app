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
                div.innerHTML = xhr.response;
                document.body.appendChild(div);
                document.body.removeChild(document.getElementById("comparedistance"));
                var s = document.createElement("script");
                s.src = "http://localhost/checkkill.js";
                document.head.appendChild(s);
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
            div.innerHTML = xhr.response;
            document.body.appendChild(div);
            document.body.removeChild(document.getElementById("comparedistance"));
            var s = document.createElement("script");
            s.src = "http://localhost/checkkill.js";
            document.head.appendChild(s);
        }
        xhr.send("testshot:checkkill=1000;");
    }
}
function noButtonListener() {
    xhrSend("testshot:chooseattacker;");
}
function yesnoKeyListener(e) {
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
                    div.innerHTML = xhr.response;
                    document.body.appendChild(div);
                    document.body.removeChild(document.getElementById("comparedistance"));
                    var s = document.createElement("script");
                    s.src = "http://localhost/checkkill.js";
                    document.head.appendChild(s);
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
                div.innerHTML = xhr.response;
                document.body.appendChild(div);
                document.body.removeChild(document.getElementById("comparedistance"));
                var s = document.createElement("script");
                s.src = "http://localhost/checkkill.js";
                document.head.appendChild(s);
            }
            xhr.send("testshot:checkkill=1000;");
        }
    }
    //if (e.keyCode == 27) {
    //    xhrSend("menu");
    //}
}