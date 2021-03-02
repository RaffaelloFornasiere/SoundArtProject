#include <Arduino.h>

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>

<input id="the-file" name="file" type="file" oninput="sendFile()" />



<script>

function sendFile()
{
var fileInput = document.getElementById('the-file');
console.log(fileInput.files); // A FileList with all selected files

var file = fileInput.files[0];
console.log(file.fileName); // "my-holiday-photo.jpg"
console.log(file.size); // 1282632
console.log(file.type); // image/jpeg

var xhr = new XMLHttpRequest();
xhr.open('POST', '/upload/uri', true);
xhr.send(file); // Simple!
}
</script>

</body>
</html>
)=====";