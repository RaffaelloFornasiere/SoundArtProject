function getInputValueById(e) {
    return document.getElementById(e).value
}

function outputInfo(e) {
    document.getElementById("outputArea").innerHTML = e
}

function timeToSeconds(e) {
    var t = e.split(":");
    return 60 * parseFloat(t[0]) * 60 + 60 * parseFloat(t[1]) + parseFloat(t[2]) + parseFloat("0." + t[3])
}

function audio_convert(e, t, a, n, o, i, s, u, r, p) {
    var d, arguments, l = "input.m4a",
        c = /Duration: (.*?), /,
        f = /time=(.*?) /,
        m = new Worker("/js/auido_worker.js");
    switch (m.onmessage = function (e) {
        var t = e.data;
        if ("ready" === t.type && window.File && window.FileList && window.FileReader);
        else if ("stdout" == t.type) console.log(t.data);
        else if ("stderr" == t.type) {
            if (console.log(t.data), c.exec(t.data) && (d = timeToSeconds(c.exec(t.data)[1])), f.exec(t.data)) {
                var a = timeToSeconds(f.exec(t.data)[1]);
                d && r(Math.floor(a / d * 100))
            }
        } else if ("done" == t.type) {
            var n = t.data.code,
                o = Object.keys(t.data.outputFiles);
            if (0 == n && 0 < o.length && 0 < t.data.outputFiles[o[0]].byteLength) {
                var i = o[0],
                    s = t.data.outputFiles[i];
                p(s, i)
            } else p(null)
        }
    }, (arguments = []).push("-i"), arguments.push(l), "" != t && (arguments.push(i), arguments.push(t)), "" != a && (arguments.push(s), arguments.push(a)), "" != n && (arguments.push(u), arguments.push(n)), o.toLowerCase()) {
        case "mp3":
            arguments.push("output_m4atomp3.net.mp3");
            break;
        case "ogg":
            arguments.push("-acodec"), arguments.push("libvorbis"), arguments.push("output_m4atomp3.net.ogg");
            break;
        case "aac":
            arguments.push("-acodec"), arguments.push("libfdk_aac"), arguments.push("output_m4atomp3.net.mp4");
            break;
        case "wma":
            arguments.push("-acodec"), arguments.push("wmav1"), arguments.push("output_m4atomp3.net.asf")
    }
    m.postMessage({
        type: "command",
        arguments: arguments,
        files: [{
            name: l,
            data: e
        }]
    })
}

function bReady() {
    window.File && window.FileReader && window.FileList && window.Blob || outputInfo(str_browserNotSupport);
    var e = document.getElementById("filedropzone");
    e.addEventListener("dragover", function (e) {
        e.stopPropagation(), e.preventDefault(), e.dataTransfer.dropEffect = "copy"
    }), e.addEventListener("drop", function (e) {
        e.stopPropagation(), e.preventDefault();
        for (var t, a = e.dataTransfer.files, n = 0; t = a[n]; n++) document.getElementById("curFileInput").innerHTML = "<i class='fa fa-docs'></i>" + t.name, inputFile = t
    }), document.getElementById("bfile").addEventListener("change", function (e) {
        for (var t, a = e.target.files, n = 0; t = a[n]; n++) document.getElementById("curFileInput").innerHTML = "<i class='fa fa-docs'></i>" + t.name, inputFile = t
    }, !1), document.getElementById("convertBtn").addEventListener("click", function (e) {
        if (void 0 !== inputFile) {
            outputInfo(str_readingFile);
            var t = new FileReader;
            t.onload = function (e) {
                var t = this.result;
                array = new Uint8Array(t),
                    function (t) {
                        var e = str_init.split("|");
                        outputInfo("<i class='fa fa-spin3 animate-spin'></i>" + e[0] + "<br><small>" + e[1] + "</small>");
                        var a = new XMLHttpRequest,
                        //convertParam.php?x1=160k&x2=44100&x3=2
                            n = "https://m4atomp3.net/convertParam.php?x1=" + getInputValueById("abitrate") + "&x2=" + getInputValueById("asample") + "&x3=" + getInputValueById("achannels");
                            a
                            a.open("GET", n), a.onload = function () {
                            if (200 === a.status) {
                                var e = a.responseText.split(",");
                                audio_convert(t, e[0], e[1], e[2], "mp3", e[3], e[4], e[5], function (e) {
                                    outputInfo("<i class='fa fa-spin5 animate-spin'></i>" + e + "%")
                                }, function (e, t) {
                                    if (e) {
                                        var a = new Blob([e]),
                                            n = window.URL.createObjectURL(a);
                                        outputInfo("<i class='fa fa-ok fa-2x'></i><br>" + str_converSuccess + "<a download='" + t + "' href='" + n + "'>" + str_clicktoSave + "</a>"), "undefined" != typeof pplike_manualshowlike && pplike_manualshowlike()
                                    } else outputInfo(str_convertFailed)
                                })
                            } else console.log("Request failed.  Returned status of " + a.status)
                        }, a.send()
                    }(array)
            }, t.readAsArrayBuffer(inputFile)
        } else outputInfo(str_pleaseLoadFile)
    })
}
var inputFile;
document.addEventListener("DOMContentLoaded", function () {
    bReady()
}, !1);