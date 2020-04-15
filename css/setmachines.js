var polarisMoney = 0;
var protectoratMoney = 0;
let arr = new Array();
/*var infs = document.getElementsByClassName("unit");
for (var i = 0; i < infs.length; i++) {
    infs[i].addEventListener("click", chooseUnit);
    infs[i].style = "opacity: 0.5;";
}*/
/*var ti = document.getElementsByClassName('input-infantry');
for (var i = 0; i < t.length; i++) {
    ti[i].addEventListener('change', changeInfantry);
    ti[i].value = "0";
}*/
var t = document.getElementsByClassName('input');
for (var i = 0; i < t.length; i++) {
    t[i].value = "0";
}
var plus = document.getElementsByClassName('plus');
for (var i = 0; i < plus.length; i++) {
    plus[i].addEventListener("click", plusListener);
}
var minus = document.getElementsByClassName('minus');
for (var i = 0; i < minus.length; i++) {
    minus[i].addEventListener("click", minusListener);
}
var tds = document.getElementsByTagName('img');
for (var i = 0; i < tds.length; i++) {
    tds[i].style = "opacity: 0.5;";
}
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
function getSiblingByTag(tag, th) {
    var sibling = th;
    do {
        if (sibling.nodeName == tag) {
            return sibling;
        }
        sibling = sibling.previousSibling;
    }while (sibling);
}
document.getElementById('setmachines').addEventListener("click", clickButton);
function clickButton() {
    var emptyPolaris = true;
    var emptyProtectorat = true;
    var polaris = document.getElementById("polaris").getElementsByClassName("input-infantry");
    for (var i = 0; i < polaris.length; i++) {
        if (Number(polaris[i].innerHTML) > 0) {
            emptyPolaris = false;
        }
    }
    var protectorat = document.getElementById("protectorat").getElementsByClassName("input-infantry");
    for (var i = 0; i < protectorat.length; i++) {
        if (Number(protectorat[i].innerHTML) > 0) {
            emptyProtectorat = false;
        }
    }
    if (!emptyProtectorat && !emptyPolaris) {
        var inputs = document.getElementsByClassName('input');
        var req = "fillarraymachines:";
        for (var i = 0; i < inputs.length; i++) {
            if (i != inputs.length - 1) {
                req += inputs[i].parentNode.parentNode.id + "=" + inputs[i].innerHTML + ",";
            }
            else {
                req += inputs[i].parentNode.parentNode.id + "=" + inputs[i].innerHTML + ";";
            }
        }
        var xhr = new XMLHttpRequest();
        xhr.open('POST', 'http://localhost/', true);
        xhr.send(req);
        document.cookie = "infantry = " + JSON.stringify(arr);
        xhrSend("menu");
    }
    else {
        alert("Выберите хотя бы один отряд из каждой стороны!");
    }
}
function plusListener() { 
    var reg = /^\d+$/;
    if (document.getElementById("limit").value.match(reg)) {
        if (this.parentNode.parentNode.parentNode.parentNode.parentNode.parentNode.id == "polaris") {
            if (polarisMoney + Number(this.parentNode.parentNode.dataset.cost) > Number(document.getElementById("limit").value)) {
                alert("Превышен лимит!");
            }
            else {
                var input = getSiblingByTag("SPAN", this);
                input.innerHTML = Number(input.innerHTML) + 1;
                this.parentNode.parentNode.getElementsByTagName('img')[0].style = "opacity: 1;";
                polarisMoney += Number(this.parentNode.parentNode.dataset.cost);
                document.getElementById("polaris-money").innerHTML = polarisMoney;
            }
        }
        else {
            if (protectoratMoney + Number(this.parentNode.parentNode.dataset.cost) > Number(document.getElementById("limit").value)) {
                alert("Превышен лимит!");
            }
            else {
                var input = getSiblingByTag("SPAN", this);
                input.innerHTML = Number(input.innerHTML) + 1;
                this.parentNode.parentNode.getElementsByTagName('img')[0].style = "opacity: 1;";
                protectoratMoney += Number(this.parentNode.parentNode.dataset.cost);
                document.getElementById("protectorat-money").innerHTML = protectoratMoney;
            }
        }
        if (this.parentNode.parentNode.className == "unit") {
            var unitInArr = false;
            for (i = 0; i < arr.length; i++) {
                if (this.parentNode.parentNode.id == "inf" + arr[i]) {
                    unitInArr = true;
                }
            }
            if (!unitInArr) {
                arr.push(Number(this.parentNode.parentNode.id.slice(3)));
            }
        }
    }
    else {
        alert("Некорректно введен лимит!");
    }
}
function minusListener() {
    var input = getSiblingByTag("SPAN", this);
    input.innerHTML = Number(input.innerHTML) - 1;
    if (Number(input.innerHTML) < 0) {
        input.innerHTML = "0";
    }
    else {
        if (this.parentNode.parentNode.parentNode.parentNode.parentNode.parentNode.id == "polaris") {
            polarisMoney -= Number(this.parentNode.parentNode.dataset.cost);
            document.getElementById("polaris-money").innerHTML = polarisMoney;
        }
        else {
            protectoratMoney -= Number(this.parentNode.parentNode.dataset.cost);
            document.getElementById("protectorat-money").innerHTML = protectoratMoney;
        }
    }
    if (Number(input.innerHTML) == 0) {
        this.parentNode.parentNode.getElementsByTagName('img')[0].style = "opacity: 0.5;";
        for (i = 0; i < arr.length; i++) {
            if (this.parentNode.parentNode.id == "inf" + arr[i]) {
                arr.splice(i, 1);
            }
        }
    }
}
/*function changeInfantry() {
    if (Number(this.value) > 0) {

    }
}*/
/*function chooseUnit() {
    var unitInArr = false;
    for (i = 0; i < arr.length; i++) {
        if (this.id == "inf" + arr[i]) {
            this.style = "opacity: 0.5;";
            arr.splice(i, 1);
            unitInArr = true;
            if (this.parentNode.parentNode.parentNode.parentNode.id == "polaris") {
                polarisMoney -= Number(this.dataset.cost);
                document.getElementById("polaris-money").innerHTML = polarisMoney;
            }
            else {
                protectoratMoney -= Number(this.dataset.cost);
                document.getElementById("protectorat-money").innerHTML = protectoratMoney;
            }
        }
    }
    if (!unitInArr) {
        this.style = "opacity: 1;";
        arr.push(Number(this.id.slice(3)));
        if (this.parentNode.parentNode.parentNode.parentNode.id == "polaris") {
            polarisMoney += Number(this.dataset.cost);
            document.getElementById("polaris-money").innerHTML = polarisMoney;
        }
        else {
            protectoratMoney += Number(this.dataset.cost);
            document.getElementById("protectorat-money").innerHTML = protectoratMoney;
        }
    }
}*/