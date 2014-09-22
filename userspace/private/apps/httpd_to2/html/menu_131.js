// ## Degree of indentation from the left.
flipIndentation = "5px";

// ## Padding inbetween menu items.
flipVerticalPadding = "4px";

// ## Margin between the left of the browser and the menu.
flipLeftMargin = "16px";

// ## Margin between the top of the browser and the menu.
flipTopMargin = "10px";

// ## Allow multiple menus to fold out without closing all the other open ones.
flipOpenMultipleMenus = false;

// ## Preserve the current state of the menu (requires cookies).
flipSaveMenuState = false;

// ## Use custom images for bullets
flipImages = true;

// ## Images to use (specify full path)
//flipImg_open = "icon_radio_filled.gif";
//flipImg_closed = "icon_radio_empty.gif";
//flipImg_static = "icon_error_h15.gif";
flipImg_open = "";
flipImg_closed = "";
flipImg_static = "";

// ## Initialise all flipMenus onload
flipInitOnLoad = true;

// ## Message to display in status bar while loading
flipLoadingMessage = "Loading...";

// ######################################################################

function alterSize(someSize, alterAmount) {
  someSize = String(someSize);
  var tmpNr = parseFloat(someSize.replace(/\D/g, ""));
  var tmpChar = someSize.replace(/\d/g, "");
  return isNaN(tmpNr) ? someSize : ((tmpNr + alterAmount) + tmpChar);
}

isIE = (String(navigator.appVersion).indexOf("MSIE") > -1);
if (!isIE) flipIndentation = alterSize(flipIndentation, -16);
if (!isIE) flipLeftMargin = alterSize(flipLeftMargin, -16);

if (flipImages) {
  aFlipPreloads = [];
  aFlipPreloads[0] = new Image;
  aFlipPreloads[0].src = flipImg_open;
  aFlipPreloads[1] = new Image;
  aFlipPreloads[1].src = flipImg_closed;
  aFlipPreloads[2] = new Image;
  aFlipPreloads[2].src = flipImg_static;
}

function addEvent(someObj, someEvent, someFunction) {
  if (someObj.addEventListener) {
    someObj.addEventListener(someEvent, someFunction, true);
    return true;
  } else if (someObj.attachEvent) return someObj.attachEvent("on" + someEvent, someFunction); else return false;
}

function openCloseFlip(theItem, newSetting, openParents) {
  if (theItem.flipID) {
    if (openParents) {
      var tmpItem = theItem;
      while (tmpItem.parentElement || tmpItem.parentNode) {
        tmpItem = (tmpItem.parentElement) ? tmpItem.parentElement : tmpItem.parentNode;
        openCloseFlip(tmpItem, newSetting);
      }
    }
    if ((theItem.className == "flipFolderOpen" && newSetting == "closed") || (theItem.className == "flipFolderClosed" && newSetting == "open")) {
      if (!theItem.childrenInitialised) {
        var childNodes=getChildNodes(theItem);
        for (var j = 0; j < childNodes.length; j++)
          if (childNodes[j].nodeName == "UL" && !childNodes[j].initialised) initFlip(childNodes[j]);
        theItem.childrenInitialised = true;
      }
      if (getChildNodes(theItem.getElementsByTagName("UL")[0]).length > 0)theItem.getElementsByTagName("UL")[0].style.display = (newSetting == "open") ? "" : "none";
      theItem.className = newSetting == "open" ? "flipFolderOpen" : "flipFolderClosed";
    }
  }
}

function openFlip(theItem, openParents) {
  openCloseFlip(theItem, "open", openParents);
}

function closeFlip(theItem, closeParents) {
  openCloseFlip(theItem, "closed", closeParents);
}

function toggleFlip(theElement) {
  if (theElement.flipID) {
    var theItem = theElement;
    var isContained = true;
  }
  else {
    if (theElement && theElement.button > 0) return false;
    var theItem = (isIE) ? event.srcElement : theElement.target;
    var isContained = false;
    if (theItem.className == "flipFolderOpen" || theItem.className == "flipFolderClosed") isContained = true;
    else while (theItem.parentElement || theItem.parentNode) {
      if (theItem.className == "flipStaticSel" || theItem.className == "flipStatic" || theItem.className == "flipFolderOpen" || theItem.className == "flipFolderClosed") {
        isContained = (theItem.className == "flipFolderOpen" || theItem.className == "flipFolderClosed");
        break;
      }
      theItem = (theItem.parentElement) ? theItem.parentElement : theItem.parentNode;
    }
  }
  var toOpenFlip = (isContained && theItem.className == "flipFolderClosed");
  if (!flipOpenMultipleMenus && (toOpenFlip || theItem.className == "flipStatic")) {
    if (theItem.parentElement || theItem.parentNode) {
      var childNodes = getChildNodes((theItem.parentElement) ? theItem.parentElement : theItem.parentNode);
      for (var i = 0; i < childNodes.length; i++) closeFlip(childNodes[i]);
    }
  }

  staticSel(theItem);
  if (toOpenFlip)openFlip(theItem);
  staticSel(getChildNodes(theItem.getElementsByTagName("UL")[0])[0]);
}

function staticSel(theItem) {
 
  if (theItem.className == "flipStatic" || theItem.className == "") {
   
    if (theItem.parentElement || theItem.parentNode) {
      var childNodes = getChildNodes((theItem.parentElement) ? theItem.parentElement : theItem.parentNode);
      for (var i = 0; i < childNodes.length; i++)
        if ("flipStaticSel" == childNodes[i].className)childNodes[i].className = "flipStatic";
    }
    theItem.className = "flipStaticSel";
  }
}

function getChildNodes(parent) {
  var rows = [];
  var childNodes = parent.childNodes;
  for (i = 0; i < childNodes.length; i++) {
    if (childNodes[i].nodeType == 1 && childNodes[i].tagName) {
      rows.push(childNodes[i]);
    }
  }
  return rows;
}


function setAllFlips(startElement, newSetting) {
  if (typeof startElement == "undefined") var startElement = document;
  if (typeof newSetting == "undefined") var newSetting = "closed";

  var aUL = startElement.getElementsByTagName("UL");
  for (var i = 0; i < aUL.length; i++) {
    var parentFlip = aUL[i].parentElement ? aUL[i].parentElement : aUL[i].parentNode;
    openCloseFlip(parentFlip, newSetting);
  }
}

function openAllFlips(startElement) {
  setAllFlips(startElement, "open");
}

function closeAllFlips(startElement) {
  setAllFlips(startElement, "closed");
}

function initFlip(startElement) {
  if (!document.createElement) return false;
  if (!startElement || !startElement.nodeName) {
    var aUL = document.getElementsByTagName("UL");
    for (var i = 0; i < aUL.length; i++) {
      if (flipLoadingMessage != "") window.status = flipLoadingMessage + " " + parseInt((i / (aUL.length - 1)) * 100, 10) + "%";
      var curUL = aUL[i];
      if (curUL.className == "flipMenu") {
        initFlip(curUL);

        // ## Fix text selecting problem in Mozilla
        curUL.onselectstart = new Function("return false");
        curUL.onmousedown = new Function("return false");
        curUL.onclick = new Function("return true");
      }
    }

    if (flipSaveMenuState) loadMenuState();

    if (flipLoadingMessage != "") window.status = "";
    return true;
  }

  if (startElement.className == "flipMenu") startElement.style.display = "";

  var childNodes=getChildNodes(startElement);

  if (!childNodes || childNodes.length == 0) return false;

  if (typeof flipIDCur == "undefined") flipIDCur = 0;
  if (!startElement.initialised) {
    var aUL = startElement.getElementsByTagName("UL");
    for (var i = 0; i < aUL.length; i++) aUL[i].style.display = "none";
  }

  for (var i = 0; i < childNodes.length; i++) {
    var curNode = childNodes[i];
    if (curNode.nodeName == "LI") {
      flipIDCur++;
      curNode.flipID = flipIDCur;

      var nodeHasChildren = curNode.getElementsByTagName("UL").length > 0;
      if (nodeHasChildren) {
        if (flipImages && curNode.flipClosed) curNode.style.listStyleImage = "url(" + curNode.flipClosed + ")";
        if (curNode.className == null || curNode.className == "") curNode.className = "flipFolderClosed";
      } else {
        if (0 == i)curNode.className = "flipStaticSel";
        else curNode.className = "flipStatic";
        if (flipImages && !curNode.style.listStyleImage) {
          if (!curNode.flipStatic) curNode.flipStatic = flipImg_static;
          curNode.style.listStyleImage = "url(" + curNode.flipStatic + ")";
        }
      }
      if (!curNode.flipOpen) curNode.flipOpen = flipImg_open;
      if (!curNode.flipClosed) curNode.flipClosed = flipImg_closed;
      if (curNode.flipIsOpen) openFlip(curNode);
    }
  }

  startElement.initialised = true;
  document.getElementById("menu").style.display = "";
}

function rootOfFlip(flipID, startElement) {
  var childNodes=getChildNodes(startElement);
  function containsFlip(childNodes, flipID) {
    var flipFound = false;
    var i = 0;
    while (i < childNodes.length && !flipFound) {
      var curNode = childNodes[i];
      flipFound = (curNode.flipID == flipID) ? true : containsFlip(curNode, flipID);
      i++;
    }
    return flipFound;
  }

  var rootFlip = null;

  if (!startElement || !startElement.nodeName) {
    var aUL = document.getElementsByTagName("UL");
    var i = 0;
    while (rootFlip == null && i < aUL.length) {
      var curUL = aUL[i];
      if (curUL.nodeName == "UL" && curUL.className == "flipMenu") rootFlip = rootOfFlip(flipID, curUL);
      i++;
    }
    return rootFlip;
  }

  if (childNodes) for (var i = 0; i < childNodes.length; i++) {
    var curNode = childNodes[i];
    if (curNode.flipID == flipID || containsFlip(curNode, flipID)) rootFlip = curNode;
  }

  return rootFlip;
}

function getCookie(cookieName) {
  var allCookies = document.cookie;
  var indexStr = allCookies.indexOf(cookieName + "=");
  if (indexStr == -1) return "";
  indexStr = allCookies.indexOf("=", indexStr) + 1;
  var endStr = allCookies.indexOf(";", indexStr);
  if (endStr == -1) endStr = allCookies.length;
  return unescape(allCookies.substring(indexStr, endStr));
}

function inArray(someID, someArray) {
  for (var i = 0; i < someArray.length; i++) if (someArray[i] == someID) return true;
  return false;
}

function getMenuState(startElement) {
  var childNodes=getChildNodes(startElement);
  if (!childNodes || childNodes.length == 0) return "";

  var openItems = "";
  var aUL = startElement.getElementsByTagName("UL");
  for (var i = 0; i < aUL.length; i++) {
    var curNode = aUL[i];
    var parentFlip = (curNode.parentElement) ? curNode.parentElement : curNode.parentNode;
    if (curNode.style.display == "" && parentFlip.flipID) openItems += " " + parentFlip.flipID;
  }
  return openItems;
}

function putMenuState(startElement) {
  var childNodes=getChildNodes(startElement);
  if (!childNodes || childNodes.length == 0) return false;

  var aUL = startElement.getElementsByTagName("UL");
  for (var i = 0; i < aUL.length; i++) {
    var curNode = aUL[i];
    var parentFlip = (curNode.parentElement) ? curNode.parentElement : curNode.parentNode;
    if (inArray(parentFlip.flipID, aOpenItems)) {
      openFlip(parentFlip);
      if (typeof prevFlipRoot == "undefined") {
        var testRoot = rootOfFlip(parentFlip.flipID);
        if (testRoot.flipID == parentFlip.flipID) prevFlipRoot = testRoot;
      }
    }
  }
}

function saveMenuState() {
  if (flipSaveMenuState) {
    var aUL = document.getElementsByTagName("UL");
    for (var i = 0; i < aUL.length; i++) {
      var curUL = aUL[i];
      var curID = curUL.id ? curUL.id : i;
      if (curUL.className == "flipMenu") document.cookie = cookiePrefix + curID + "=" + getMenuState(curUL) + ";";
    }
  }
}

function loadMenuState() {
  var aUL = document.getElementsByTagName("UL");
  for (var i = 0; i < aUL.length; i++) {
    var curUL = aUL[i];
    var curID = curUL.id ? curUL.id : i;
    if (curUL.className == "flipMenu") {
      var savedState = String(getCookie(cookiePrefix + curID));
      if (savedState != "") {
        aOpenItems = savedState.split(" ");
        putMenuState(curUL);
      }
    }
  }

  addEvent(window, "unload", saveMenuState);
}

function clearMenuState(flipMenuID) {
  if (typeof flipMenuID == "undefined") {
    var aUL = document.getElementsByTagName("UL");
    for (var i = 0; i < aUL.length; i++) {
      var curUL = aUL[i];
      var curID = curUL.id ? curUL.id : i;
      if (curUL.className == "flipMenu") document.cookie = cookiePrefix + curID + "=;";
    }
  } else document.cookie = cookiePrefix + flipMenuID + "=;";
}

cookiePrefix = document.location.pathname + "_";

addEvent(document, "click", toggleFlip);
if (flipInitOnLoad)  addEvent(window, "load", initFlip);