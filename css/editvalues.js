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
document.getElementById('menu').addEventListener("click", backToMenu);
var strength = document.getElementsByClassName('strength');
for (var i = 0; i < strength.length; i++) {
    strength[i].children[0].addEventListener("click", plusStrength);
}
for (var i = 0; i < strength.length; i++) {
    strength[i].children[1].addEventListener("click", minusStrength);
}
var ammunition = document.getElementsByClassName('ammunition');
for (var i = 0; i < ammunition.length; i++) {
    ammunition[i].children[0].addEventListener("click", plusAmmunition);
}
for (var i = 0; i < ammunition.length; i++) {
    ammunition[i].children[1].addEventListener("click", minusAmmunition);
}
function plusStrength() {
    var xhr = new XMLHttpRequest();
    xhr.open('POST', 'http://localhost/', true);
    xhr.send("plusstrength=" + this.parentNode.parentNode.id + ";");
    xhr.onload = function() {
        xhrSend("editvalues;");
    }
}
function minusStrength() {
    var xhr = new XMLHttpRequest();
    xhr.open('POST', 'http://localhost/', true);
    xhr.send("minusstrength=" + this.parentNode.parentNode.id + ";");
    xhr.onload = function() {
        xhrSend("editvalues;");
    }
}
function plusAmmunition() {
    var xhr = new XMLHttpRequest();
    xhr.open('POST', 'http://localhost/', true);
    xhr.send("plusammunition=" + this.parentNode.parentNode.id + ";");
    xhr.onload = function() {
        xhrSend("editvalues;");
    }
}
function minusAmmunition() {
    var xhr = new XMLHttpRequest();
    xhr.open('POST', 'http://localhost/', true);
    xhr.send("minusammunition=" + this.parentNode.parentNode.id + ";");
    xhr.onload = function() {
        xhrSend("editvalues;");
    }
}
function backToMenu() {
    xhrSend("menu");
    //var strength = document.getElementsByClassName('strength');
    //var ammunition = document.getElementsByClassName('ammunition');
    //var req = "updatevalues:";
    //for (var i = 0; i < inputs.length; i++) {
    //    if (i != inputs.length - 1) {
    //        req += inputs[i].parentNode.id + "=" + inputs[i].value + ",";
    //    }
    //    else {
    //        req += inputs[i].parentNode.id + "=" + inputs[i].value + ";";
    //    }
    //}
    //var xhr = new XMLHttpRequest();
    //xhr.open('POST', 'http://localhost/', true);
    //xhr.send(req);
}