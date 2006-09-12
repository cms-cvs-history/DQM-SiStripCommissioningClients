function RequestModuleList() {
  var queryString = "file=" + GetFile();

    queryString += "&command=mod_list";
 // Get Module Number
  var obj = document.getElementById("module_numbers");
  var value="NULL";
  if (obj.selectedIndex>=0) 
  	 value =  obj.options[obj.selectedIndex].value;
  queryString += '&module='+value;
    var url = BASE_URL+queryString; 
//     alert(" url = " + url);
    httpRequest("GET", url, true, FillModuleList);     
}
function RequestHistoList() {
  var queryString = "file=" + GetFile();

  queryString += "&command=histo_list";
 // Get Module Number
  var obj = document.getElementById("module_numbers");
  var value="NULL";
  if (obj.selectedIndex>=0) 
  	 value =  obj.options[obj.selectedIndex].value;
  queryString += '&module='+value;
    var url = BASE_URL+queryString; 
//     alert(" url = " + url);
    httpRequest("GET", url, true, FillHistoList);     
}






function FillModuleList() {
  if (request.readyState == 4) {
    if (request.status == 200) {
      try {
        var doc = request.responseXML;
        var root = doc.documentElement;

        // Module Number select box
        var aobj = document.getElementById("module_numbers");
        aobj.options.length = 0;

        var mrows = root.getElementsByTagName('ModuleNum');
//        alert(" rows = " + mrows.length);
        for (var i = 0; i < mrows.length; i++) {
          var mnum  = mrows[i].childNodes[0].nodeValue;
          var aoption = new Option(mnum, mnum);
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
      alert("FillHistoList:  ERROR:"+request.readyState+", "+request.status);
    }
  }
}
function FillHistoList() {
  if (request.readyState == 4) {
    if (request.status == 200) {
      try {
        var doc = request.responseXML;
        var root = doc.documentElement;

        // Histogram  select box
        var bobj = document.getElementById("histolistarea");
        bobj.options.length = 0;

        var hrows = root.getElementsByTagName('Histo');
//        alert(" rows = " + hrows.length);
        for (var j = 0; j < hrows.length; j++) {
          var name  = hrows[j].childNodes[0].nodeValue;
          var boption = new Option(name, name);
          try {
            bobj.add(boption, null);
          }
          catch (e) {
            bobj.add(boption, -1);
          }
        }


      }
      catch (err) {
        alert ("Error detail: " + err.message); 
      }
    } 
    else {
      alert("FillHistoList:  ERROR:"+request.readyState+", "+request.status);
    }
  }
}

function FillRefFileList() {
  if (request.readyState == 4) {
    if (request.status == 200) {
      try {
        var doc = request.responseXML;
        var root = doc.documentElement;

        // File Name select box
        var aobj = document.getElementById("ref_filename");
        aobj.options.length = 0;

        var rows = root.getElementsByTagName('name');
/*      alert(" rows = " + rows.length);*/
        for (var i = 0; i < rows.length; i++) {
          var name  = rows[i].childNodes[0].nodeValue;
          if (name == GetFile()) continue;
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
function FillSummaryHistoList() {
  if (request.readyState == 4) {
    if (request.status == 200) {
      try {
        var doc = request.responseXML;
        var root = doc.documentElement;

        // Histogram  select box
        var obj = document.getElementById("histolistarea");
        obj.options.length = 0;

        var hrows = root.getElementsByTagName('SummaryHisto');
//        alert(" rows = " + hrows.length);
        for (var j = 0; j < hrows.length; j++) {
          var name  = hrows[j].childNodes[0].nodeValue;
          var option = new Option(name, name);
          try {
            obj.add(option, null);
          }
          catch (e) {
            obj.add(option, -1);
          }
        }
      }
      catch (err) {
        alert ("Error detail: " + err.message); 
      }
    } 
    else {
      alert("FillSummaryHistoList:  ERROR:"+request.readyState+", "+request.status);
    }
  }
}
