var request = null; 
//var BASE_URL = "http://localhost/cgi-bin/ajax/HistogramViewer.exe?";
//var BASE_URL = "http://cdftest.cnaf.infn.it:8081/cgi-bin/cms/ajax/HistogramViewer.exe?";
var BASE_URL = "http://lxcmsf1.cern.ch:1977/urn:xdaq-application:lid=15/HistogramViewer?"
var canvasList = new Array();
var slideList = new Array();
var slideShowSpeed = 5000;
var index = 0;
var nSlides = 0;
var timerID;
var MAX_SLIDES = 20;
var lastMatchIndex = -1;
var firstMatchIndex = -1;

/* Wrapper function for constructing a request object. 
   Parameters: 
     <reqType>: The HTTP request type, such as GET or POST. 
     <url>: The URL of the server program. 
     <asynch>: Whether to send the request asynchronously or not. 
     <respHandle>: The name of the function that will handle the response. 

     Any fifth parameters, represented as arguments[4], are the data a 
     POST request is designed to send. 
*/ 
function httpRequest(reqType, url, asynch, respHandle) { 
//  try {
//     netscape.security.PrivilegeManager.enablePrivilege("UniversalBrowserRead");
//    } catch (e) {
//     alert("Permission UniversalBrowserRead denied.");
//    }

  if (window.XMLHttpRequest) {   // Mozilla-based browsers 
   // alert("creating the request");
    request = new XMLHttpRequest(); 
	if (request.overrideMimeType)
    	{
    	 request.overrideMimeType('text/xml');
   		 }
  } 
  else if (window.ActiveXObject) { 
    request = new ActiveXObject("Msxml2.XMLHTTP"); 
    if (!request) { 
      request = new ActiveXObject("Microsoft.XMLHTTP"); 
    } 
  } 
  // very unlikely, but we test for a null request 
  // if neither ActiveXObject was initialized 
  if (request) { 
    // if the reqType parameter is POST, then the 
    // 5th argument to the function is the POSTed data 
    if (reqType.toLowerCase() != "post") { 
      initReq(reqType, url, asynch, respHandle); 
    }  
    else { 
      // the POSTed data 
      var args = arguments[4]; 
      if (args != null && args.length > 0) { 
         initReq(reqType, url, asynch, respHandle, args); 
      } 
    } 
  } 
  else { 
    alert("Your browser does not permit the use of all "+ 
          "of this application's features!"); 
  } 
} 
// Initialize a request object that is already constructed 
function initReq(reqType, url, bool, respHandle) { 
  try { 
    // Specify the function that will handle the HTTP response 
   //   alert("I call this "+reqType+url);
    request.onreadystatechange = respHandle; 
//	alert("before open");
    request.open(reqType, url, true); 
//	alert("It is open");
    // if the reqType parameter is POST, then the 
    // 5th argument to the function is the POSTed data 
   if (reqType.toLowerCase() == "post") { 
       request.setRequestHeader("Content-Type", 
            "application/x-www-form-urlencoded; charset=UTF-8"); 
       request.send(arguments[4]); 
     }  
     else { 
      request.send(null); 
     } 
   } 
   catch (errv) { 
     alert ( 
        "The application cannot contact " + 
        "the server at the moment. " + 
        "Please try again in a few seconds.\\n" + 
        "Error detail: " + errv.message); 
  } 
}
//input field's event handlers 
window.onload=function() { 
  var queryString = 'command=filelist';
  //alert(window.location.href);
  var url = BASE_URL+queryString; 
  //alert("I call httpRequest "+url);
  httpRequest('GET', url, true, FillFileList); 
} 
document.write('<script src="/temporary/web/request_fname.js"><\/script>');
document.write('<script src="/temporary/web/request_histolist.js"><\/script>');
document.write('<script src="/temporary/web/common_actions.js"><\/script>');
document.write('<script src="/temporary/web/request_plot.js"><\/script>');
