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
function getCookie(name) {
  var value = "; " + document.cookie;
  var parts = value.split("; " + name + "=");
  if (parts.length == 2) return parts.pop().split(";").shift();
}
var delayedMachine = "";
var tds = document.getElementsByClassName('unit');
for (var i = 0; i < tds.length; i++) {
    tds[i].addEventListener("click", tdClickListener);
}
var tdsm = document.getElementsByClassName('machine');
for (var i = 0; i < tdsm.length; i++) {
    tdsm[i].addEventListener("click", tdClickListener);
}
var tdsb = document.getElementsByClassName('blowup');
for (var i = 0; i < tdsb.length; i++) {
    tdsb[i].addEventListener("click", tdClickListener);
}
document.getElementsByTagName("button")[0].addEventListener("click", buttonClickListener);
document.getElementsByTagName("button")[1].addEventListener("click", menuClickListener);
document.getElementById("icon").addEventListener("click", changeSide);
var req = "";
var req2 = "";
var id = "";
var id2 = "";
var ammoreq = "";
var sideIcon;
if (getCookie("side")) {
    sideIcon = getCookie("side");
    if (getCookie("side") == "polaris") {
        document.getElementById("icon").src = "http://localhost/polaris1.jpg";
    }
    else {
        document.getElementById("icon").src = "http://localhost/protectorat1.jpeg";
    }
}
else {
    sideIcon = "polaris";
    document.getElementById("icon").src = "http://localhost/polaris1.jpg";
}
setTimeout(() => { 
    for (var i = 0; i < tdsm.length; i++) {
        var posTop = tdsm[i].getBoundingClientRect().top + window.pageYOffset;
        tdsm[i].getElementsByTagName('img')[1].style = "display: block; position: absolute; left: " + tdsm[i].getBoundingClientRect().left + "px; top: " + posTop + "px; z-index:999;";
        tdsm[i].getElementsByTagName('img')[1].addEventListener("click", changeSideMachine);
    }
}, 100);
var attackobj = "";
var blu = false;
var spoilerVisible = false;
function changeSideMachine() {
    var arrPolaris = JSON.parse(getCookie('polaris-machines'));
    var arrProtectorat = JSON.parse(getCookie('protectorat-machines'));
    if (this.parentNode.parentNode.parentNode.parentNode.parentNode.id == "polaris-machines") {
        for (var i = 0; i < arrPolaris.length; i++) {
            if (Number(this.parentNode.id.slice(3, this.parentNode.id.indexOf("-"))) == arrPolaris[i]) {
                arrProtectorat.push(arrPolaris[i]);
                arrPolaris.splice(i, 1);
            }
        }
    }
    else if (this.parentNode.parentNode.parentNode.parentNode.parentNode.id == "protectorat-machines") {
        for (var i = 0; i < arrProtectorat.length; i++) {
            if (Number(this.parentNode.id.slice(3, this.parentNode.id.indexOf("-"))) == arrProtectorat[i]) {
                arrPolaris.push(arrProtectorat[i]);
                arrProtectorat.splice(i, 1);
            }
        }
    }
    document.cookie = "polaris-machines = " + JSON.stringify(arrPolaris);
    document.cookie = "size-polaris-machines = " + arrPolaris.length;
    document.cookie = "protectorat-machines = " + JSON.stringify(arrProtectorat);
    document.cookie = "size-protectorat-machines = " + arrProtectorat.length;
    xhrSend("testshot:chooseattacker;")
}
function clearAll(th) {
    var side = th.parentNode.parentNode.parentNode;
    var sideTrs = side.children[0].children;
    for (var j = 0; j < sideTrs.length; j++) {
        var sideTds = sideTrs[j].children;
        for (var i = 0; i < sideTds.length; i++) {
            sideTds[i].style = "";
        }
    }
    if (document.getElementById("polaris-machines")) {
        if (side.parentNode.id.includes("machines")) {
            var sideTrs = document.getElementById(side.parentNode.id.slice(0, side.parentNode.id.length - 9)).children[0].children[0].children;
            for (var j = 0; j < sideTrs.length; j++) {
                var sideTds = sideTrs[j].children;
                for (var i = 0; i < sideTds.length; i++) {
                    sideTds[i].style = "";
                }
            }
        }
        else {
            var sideTrs = document.getElementById(side.parentNode.id + "-machines").children[0].children[0].children;
            for (var j = 0; j < sideTrs.length; j++) {
                var sideTds = sideTrs[j].children;
                for (var i = 0; i < sideTds.length; i++) {
                    sideTds[i].style = "";
                }
            }
        }
    }
}
function computeChance() {
    if (req && req2 && !spoilerVisible) {
        var num = document.getElementById(id).dataset.num;
        var side = document.getElementById(id).dataset.side;
        var armor = document.getElementById(id2).dataset.armor;
        var singleChance = (side - armor) / side;
        if (singleChance > 1) {
            singleChance = 1;
        }
        else if (singleChance < 0) {
            singleChance = 0;
        }
        var chance = (1 - Math.pow(1 - singleChance, num)) * 100;
        document.getElementById("chance").innerHTML = "Вероятность пробития: " + chance + "%";
    }
}
function tdClickListener() {
    var allspoilers = document.getElementsByClassName("spoiler");
    for (var i = 0; i < allspoilers.length; i++) {
        allspoilers[i].style = "display: none;";
    }
    var allspoilerslrw = document.getElementsByClassName("spoiler-lrw");
    for (var i = 0; i < allspoilerslrw.length; i++) {
        allspoilerslrw[i].style = "display: none;";
    }
    spoilerVisible = false;
    if (this.id.includes("inf")) {
        if (!blu) {
            clearAll(this);
        }
        else {
            var tds1 = document.getElementsByClassName('machine');
            for (var i = 0; i < tds1.length; i++) {
                tds1[i].style = "";
            }
            var tds2 = document.getElementsByClassName('unit');
            for (var i = 0; i < tds2.length; i++) {
                tds2[i].style = "";
            }
        }
        var posTop = this.getBoundingClientRect().top + window.pageYOffset;
        var spoiler = document.getElementById("spoiler" + this.id.slice(3));
        spoiler.style = "display: block; position: absolute; left: " + this.getBoundingClientRect().left + "px; top: " + posTop + "px; z-index:999; background-color: white; border-radius: 5px;";
        spoilerVisible = true;
        var infsRows = spoiler.children[0].children[0].children;
        for (var i = 0; i < infsRows.length; i++) {
            var infs = infsRows[i].children;
            for (var j = 0; j < infs.length; j++) {
                infs[j].addEventListener("click", infsListener);
            }
        }
    }
    else if (this.id.includes("obj")) {
        if (!blu) {
            clearAll(this);
            if (this.parentNode.parentNode.parentNode.parentNode.id.includes(sideIcon)) {
                if (this.id.slice(0, this.id.indexOf("-")) == id2.slice(0, id2.indexOf("-"))) {
                    alert("Машина не может стрелять в саму себя!");
                }
                else {
                    ammoreq = "machinesammunition=" + this.id.slice(0, this.id.indexOf("-")) + ";";
                    attackobj = this.id;
                    var posTop = this.getBoundingClientRect().top + window.pageYOffset;
                    var spoiler = document.getElementById("spoiler-lrw" + this.className.slice(3,this.className.indexOf(' ')));
                    spoiler.style = "display: block; position: absolute; left: " + this.getBoundingClientRect().left + "px; top: " + posTop + "px; z-index:999; background-color: white; border-radius: 5px;";
                    spoilerVisible = true;
                    var lrwRows = spoiler.children[0].children[0].children;
                    for (var i = 0; i < lrwRows.length; i++) {
                        var lrw = lrwRows[i].children;
                        for (var j = 0; j < lrw.length; j++) {
                            lrw[j].addEventListener("click", lrwListener);
                        }
                    }
                }
            }
            else {
                if (ammoreq.includes(this.id.slice(0, this.id.indexOf("-")))) {
                    alert("Машина не может стрелять в саму себя!");
                }
                else {
                    id2 = this.id;
                    req2 = "settarget=" + id2.slice(0, id2.indexOf("-")) + ";";
                    this.style = "border: 6px solid orange; border-radius: 5px;";
                    computeChance();
                }
            }
        }
        else {
            var tds1 = document.getElementsByClassName('machine');
            for (var i = 0; i < tds1.length; i++) {
                tds1[i].style = "";
            }
            var tds2 = document.getElementsByClassName('unit');
            for (var i = 0; i < tds2.length; i++) {
                tds2[i].style = "";
            }
            id2 = this.id;
            req2 = "settarget=" + id2.slice(0, id2.indexOf("-")) + ";";
            this.style = "border: 6px solid orange; border-radius: 5px;";
            computeChance();
        }
        
    }
    else if (this.id.includes("blu")) {
        ammoreq = "";
        if (blu) {
            if (id == this.id) {
                blu = false;
                var tds1 = document.getElementsByClassName('machine');
                for (var i = 0; i < tds1.length; i++) {
                    tds1[i].style = "";
                }
                var tds2 = document.getElementsByClassName('unit');
                for (var i = 0; i < tds2.length; i++) {
                    tds2[i].style = "";
                }
                this.style = "";
                req = "";
                req2 = "";
                id = "";
                id2 = "";
                document.getElementById("chance").innerHTML = "Вероятность пробития: ?";
            }
            else {
                blu = true;
                var blus = document.getElementsByClassName('blowup');
                for (var i = 0; i < blus.length; i++) {
                    blus[i].style = "";
                }
                req = "setttacker=" + this.id + ";";
                id = this.id;
                this.style = "border: 3px solid orange; border-radius: 5px;";
                computeChance();
            }
        }
        else {
            blu = true;
            var tds1 = document.getElementsByClassName('machine');
            for (var i = 0; i < tds1.length; i++) {
                tds1[i].style = "";
            }
            var tds2 = document.getElementsByClassName('unit');
            for (var i = 0; i < tds2.length; i++) {
                tds2[i].style = "";
            }
            req = "setttacker=" + this.id + ";";
            id = this.id;
            req2 = "";
            id2 = "";
            this.style = "border: 3px solid orange; border-radius: 5px;";
            document.getElementById("chance").innerHTML = "Вероятность пробития: ?";
        }
    }
}
function infsListener() {
    var melee = false;
    if (!blu) {
        if (document.getElementById("inf" + this.parentNode.parentNode.parentNode.parentNode.id.slice(7)).parentNode.parentNode.parentNode.parentNode.id == sideIcon) {
            if (this.className.includes("melee")) {
                alert("Нельзя выбрать ближника атакующим!");
                melee = true;
            }
            else {
                ammoreq = "";
                req = "setttacker=inf" + this.id.slice(4) + ";";
                id = this.id;
            }
        }
        else {
            req2 = "settarget=inf" + this.id.slice(4) + ";";
            id2 = this.id;
        }
    }
    else {
        req2 = "settarget=inf" + this.id.slice(4) + ";";
        id2 = this.id;
    }
    var sp = this.parentNode.parentNode.parentNode.parentNode;
    sp.style = "display: none;";
    spoilerVisible = false;
    if (!melee) {
        var rootInf = document.getElementById("inf" + sp.id.slice(7));
        var infs2 = this.cloneNode(true);
        infs2.id = "inf" + sp.id.slice(7);
        infs2.className = "unit";
        rootInf.parentNode.replaceChild(infs2, rootInf);
        infs2.style = "border: 3px solid orange; border-radius: 5px;";
        infs2.addEventListener("click", tdClickListener);
    }
    this.removeEventListener("click", infsListener);
    computeChance();
}
function lrwListener() {
    id = this.id;
    req = "setttacker=" + id + ";";
    var sp = this.parentNode.parentNode.parentNode.parentNode;
    sp.style = "display: none;";
    spoilerVisible = false;
    clearAll(document.getElementById(attackobj));
    document.getElementById(attackobj).style = "border: 6px solid orange; border-radius: 5px;";
    this.removeEventListener("click", lrwListener);
    computeChance();
};
function getCompareDistance() {
    var xhr = new XMLHttpRequest();
    xhr.open('POST', 'http://localhost/', true);
    xhr.send(req);
    var xhr2 = new XMLHttpRequest();
    xhr2.open('POST', 'http://localhost/', true);
    xhr2.send(req2);
    xhr.onload = function() {
        xhr2.onload = function() {
            var xhr3 = new XMLHttpRequest();
            xhr3.open('POST', 'http://localhost/', true);
            //xhr.responseType = 'Text';
            xhr3.onload = function() {
                var div = document.createElement("div");
                div.id = "comparedistance";
                div.innerHTML = xhr3.response;
                document.body.appendChild(div);
                document.getElementById("send").removeChild(document.getElementById("send").children[0]);
                document.getElementById("send").parentNode.removeChild(document.getElementById("send"));
                document.getElementById("menu").removeChild(document.getElementById("menu").children[0]);
                document.getElementById("menu").parentNode.removeChild(document.getElementById("menu"));
                tds = document.getElementsByClassName('unit');
                for (var i = 0; i < tds.length; i++) {
                    tds[i].removeEventListener("click", tdClickListener);
                }
                tdsm = document.getElementsByClassName('machine');
                for (var i = 0; i < tdsm.length; i++) {
                    tdsm[i].removeEventListener("click", tdClickListener);
                }
                tdsb = document.getElementsByClassName('blowup');
                for (var i = 0; i < tdsb.length; i++) {
                    tdsb[i].removeEventListener("click", tdClickListener);
                }
                document.getElementById("icon").removeEventListener("click", changeSide);
                var s = document.createElement("script");
                s.src = "http://localhost/comparedistance.js";
                document.head.appendChild(s);
            }
            xhr3.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
            xhr3.send("testshot:comparedistance;");
        }
    }
}
function getCheckKill() {
    var xhr = new XMLHttpRequest();
    xhr.open('POST', 'http://localhost/', true);
    xhr.send(req);
    var xhr2 = new XMLHttpRequest();
    xhr2.open('POST', 'http://localhost/', true);
    xhr2.send(req2);
    xhr.onload = function() {
        xhr2.onload = function() {
            var xhr4 = new XMLHttpRequest();
            xhr4.open('POST', 'http://localhost/', true);
            xhr4.onload = function() {
                if (localStorage.getItem("logs")) {
                    localStorage.setItem("logs", localStorage.getItem("logs") + "<div class=\"turn\">" + xhr4.response);
                }
                else {
                    localStorage.setItem("logs", "<div class=\"turn\">" + xhr4.response);
                }
                var xhr3 = new XMLHttpRequest();
                xhr3.open('POST', 'http://localhost/', true);
                //xhr.responseType = 'Text';
                xhr3.onload = function() {
                    var div = document.createElement("div");
                    div.innerHTML = xhr3.response.split("//logs//")[0];
                    document.body.appendChild(div);
                    document.getElementById("send").removeChild(document.getElementById("send").children[0]);
                    document.getElementById("send").parentNode.removeChild(document.getElementById("send"));
                    document.getElementById("menu").removeChild(document.getElementById("menu").children[0]);
                    document.getElementById("menu").parentNode.removeChild(document.getElementById("menu"));
                    tds = document.getElementsByClassName('unit');
                    for (var i = 0; i < tds.length; i++) {
                        tds[i].removeEventListener("click", tdClickListener);
                    }
                    tdsm = document.getElementsByClassName('machine');
                    for (var i = 0; i < tdsm.length; i++) {
                        tdsm[i].removeEventListener("click", tdClickListener);
                    }
                    tdsb = document.getElementsByClassName('blowup');
                    for (var i = 0; i < tdsb.length; i++) {
                        tdsb[i].removeEventListener("click", tdClickListener);
                    }
                    document.getElementById("icon").removeEventListener("click", changeSide);
                    var s = document.createElement("script");
                    s.src = "http://localhost/checkkill.js";
                    document.head.appendChild(s);
                    if (localStorage.getItem("logs")) {
                        localStorage.setItem("logs", localStorage.getItem("logs") + xhr3.response.split("//logs//")[1] + "</div>");
                    }
                    else {
                        localStorage.setItem("logs", xhr3.response.split("//logs//")[1] + "</div>");
                    }
                }
                xhr3.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
                xhr3.send("testshot:checkkill;");
            }
            xhr4.send("logs:getattackerdefender;");
        }
    }
}
function createBackupCookies() {
    if (getCookie("object-machines") && getCookie("size-object-machines")) {
        document.cookie = "old-object-machines = " + getCookie("object-machines");
        document.cookie = "old-size-object-machines = " + getCookie("size-object-machines");
        document.cookie = "old-polaris-machines = " + getCookie("polaris-machines");
        document.cookie = "old-size-polaris-machines = " + getCookie("size-polaris-machines");
        document.cookie = "old-protectorat-machines = " + getCookie("protectorat-machines");
        document.cookie = "old-size-protectorat-machines = " + getCookie("size-protectorat-machines");
    }
}
function buttonClickListener() {
    if (req && req2 && !spoilerVisible) {
        //alert(ammoreq);
        //alert(req);
        //alert(req2);
        var flag = null;
        if (ammoreq != "") {
            var xhr0 = new XMLHttpRequest();
            xhr0.open('POST', 'http://localhost/', true);
            xhr0.send(ammoreq);
            xhr0.onreadystatechange = function() {
                if (xhr0.readyState == XMLHttpRequest.DONE) {
                    if (xhr0.responseText.split("//logs//")[0] == "error") {
                        alert("Не хватает боезапаса!");
                        //flag = false;
                        //xhrSend("menu");
                    }
                    else if (xhr0.responseText.split("//logs//")[0] == "ok") {
                        //flag = true;
                        delayedMachine = xhr0.responseText.split("//logs//")[1];
                        createBackupCookies();
                        getCompareDistance();
                    }
                }
            }
        }
        else if (req.includes("blu")) {
            createBackupCookies();
            getCheckKill();
        }
        else {
            //flag = true;
            createBackupCookies();
            getCompareDistance();
        }
        document.cookie = "side = " + sideIcon;
        //hile (flag == null) { sleep(100); }
        //if (flag) {
        //}
        /*if (id.includes("lrw")) {
            xhrSend("testshot:ammunition;");
        }
        else if (id.includes("inf") || id.includes("blu")) {
            xhrSend("testshot:choosetarget;");
        }*/
    }
    else {
        alert("Не выбраны атакующий или цель или все сразу!");
    }
}
function menuClickListener() {
    xhrSend("menu");
}
function changeSide() {
    var tds1 = document.getElementsByClassName('machine');
    for (var i = 0; i < tds1.length; i++) {
        tds1[i].style = "";
    }
    var tds2 = document.getElementsByClassName('unit');
    for (var i = 0; i < tds2.length; i++) {
        tds2[i].style = "";
    }
    var blus = document.getElementsByClassName('blowup');
    for (var i = 0; i < blus.length; i++) {
        blus[i].style = "";
    }
    blu = false;
    req = "";
    req2 = "";
    id = "";
    id2 = "";
    ammoreq = "";

    if (this.src == "http://localhost/polaris1.jpg") {
        this.src = "http://localhost/protectorat1.jpeg";
        sideIcon = "protectorat";
    }
    else {
        this.src = "http://localhost/polaris1.jpg";
        sideIcon = "polaris";
    }
    document.getElementById("chance").innerHTML = "Вероятность пробития: ?";
}