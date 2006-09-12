function DrawSelectedHistos() {
  var getpdf = false;
  var args = arguments[0];
  if (args != null && args.length > 0) {
    getpdf = true;
  }
  if (document.getElementById("SingleModuleHisto").checked) {
    PlotSingleModuleHisto();
  } else if (document.getElementById("SummaryHisto").checked) {
    PlotSummaryHisto();
  }
}
//
//  -- Plot Single Module Histograms
//    
function PlotSingleModuleHisto() {

  var queryString = 'command=plot_as_module'+'&file='+GetFile();
  
  // Collect other options
  // Histogram Names 
  var histos =GetSelectedHistos();
  if (histos == null) {
    alert("Plot(s) not defined!");
    return;
  }
  
  var nhist = histos.length;
  for (var i = 0; i < nhist; i++) {
    queryString += '&hist='+histos[i];
  }

  // Get Module Number
  var obj = document.getElementById("module_numbers");
  var value =  obj.options[obj.selectedIndex].value;
  queryString += '&module='+value;

  // Get Canavs
  var canvas = document.getElementById("drawingcanvas");
  if (canvas == null) {
    alert("Canvas is not defined!");
    return;
  }

  // Rows and columns
  var nr = 1;
  var nc = 1;
  if (nhist == 1) {
    // logy option
    if (document.getElementById("logy").checked) {
      queryString += '&logy=true';
    }
    obj = document.getElementById("x-low");
    value = parseFloat(obj.value);
    if (!isNaN(value)) queryString += '&xmin=' + value;

    obj = document.getElementById("x-high");
    value = parseFloat(obj.value);
    if (!isNaN(value)) queryString += '&xmax=' + value;
  } else {
    if (document.getElementById("multizone").checked) {
      obj = document.getElementById("nrow");
      nr =  parseInt(obj.value);
      if (isNaN(nr)) {
        nr = 1;
      }
      obj = document.getElementById("ncol");
      nc = parseInt(obj.value);
      if (isNaN(nc)) {
        nc = 2;       
      }
    }
    if (nr*nc < nhist) {
      if (nhist <= 10) {
        nc = 2;
      } else if (nhist <= 20) {
        nc = 3;
      } else if (nhist <= 30) {
         nc = 4;
      } 		
       nr = Math.ceil(nhist*1.0/nc);
    }
  }
  queryString += '&cols=' + nc + '&rows=' + nr;       

  // check for reference option
  if (document.getElementById("comp_ref").checked) {
    var ref_fname = GetRefFile();
    queryString += '&ref_file=' + ref_fname;
  }
 
//  var url = BASE_URL+queryString+'&t='+Math.random();
  var url = BASE_URL+queryString;
  //alert(" url = " + url);  
  canvas.src = url;
}  
//
//  -- Plot Summary Histograms
//    
function PlotSummaryHisto(){

  var queryString = 'command=plot_summary'+'&file='+GetFile();
  
  // Collect other options
  // Histogram Names 
  var histos =GetSelectedHistos();
  if (histos == null) {
    alert("Plot(s) not defined!");
    return;
  }  
  var nhist = histos.length;
  for (var i = 0; i < nhist; i++) {
    queryString += '&hist='+histos[i];
  }
  // Get Canavs
  var canvas = document.getElementById("drawingcanvas");
  if (canvas == null) {
    alert("Canvas is not defined!");
    return;
  }
  var url = BASE_URL+queryString;
  alert(" url = " + url);  
}

