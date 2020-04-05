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
document.getElementById('ok').addEventListener("click", okButtonListener);
document.getElementsByTagName('body')[0].addEventListener("keypress", menuKeyListener);
function okButtonListener() {
    xhrSend("menu");
}
function menuKeyListener(e) {
    if (e.keyCode == 13) {
        xhrSend("menu");
    }
}