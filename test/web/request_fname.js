function FillFileList() {
  if (request.readyState == 4) {
    if (request.status == 200) {
      try {
        var doc = request.responseXML;
        var root = doc.documentElement;

        // File Name select box
        var aobj = document.getElementById("filename");
        aobj.options.length = 0;

        var rows = root.getElementsByTagName('name');
/*      alert(" rows = " + rows.length);*/
        for (var i = 0; i < rows.length; i++) {
          var name  = rows[i].childNodes[0].nodeValue;
          var aoption = new Option(name, name);
          try {
            aobj.add(aoption, null);
          }
          catch (e) {
            aobj.add(aoption, -1);
          }
        }
      }
      catch (err) {
        alert ("Error detail: " + err.message); 
      }
    } 
    else {
      alert("FillFileList:  ERROR:"+request.readyState+", "+request.status);
    }
  }
}
