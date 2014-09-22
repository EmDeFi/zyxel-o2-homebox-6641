// JavaScript Document
var minHeightGlobal = 0;
var isIE8 = navigator.userAgent.search("MSIE 8") > -1; 
var isIE = navigator.userAgent.search("MSIE") > -1; 
var isOpera = navigator.userAgent.search("Opera") > -1;
var isSafari = navigator.userAgent.search("Safari") > -1;
new Image().src = "./images1/help_red.png";
new Image().src = "./images1/help_bottom.gif";
/*For To2 disable copy and cut in password, Mitrastar ChingLung, 20130628*/
/*
$(function(){
	$("input:password").bind("copy cut paste",function(e){
	   return false;
	})
})
*/

/*getElementsByName() for IE*/
function getElementsByName(tagName, eName){  
	var tags = document.getElementsByTagName(tagName); 
	var returns = new Array();
	
	if (tags != null && tags.length > 0) 
	{  
		for (var i = 0; i < tags.length; i++) 
		{ 
			if (tags[i].getAttribute("name") == eName) 
			{ 
				returns[returns.length] = tags[i];
			}
		}
	}
	
	return returns;  
}

function operaResize()
{
	/* dynamically adjust height of iframe for opera and safari*/
	if(isOpera || isSafari)
	{
		var elem = document.getElementById('defaultId');

		if(elem)
		{
			var height;
			if(elem.className == "bg_body_auto")
			{
				height = elem.offsetHeight + 52;
			}
			else if (elem.className == "bg_body_auto2")
			{
				height = elem.offsetHeight + 82;
			}
			else
			{
				height = 600;
			}
			
			parent.document.getElementById("mainframe").height = height;
		}
	}
}

/*information A-Z*/
function popupwnd(url, toolbar, menubar, locationbar, resize, scrollbars, statusbar, left, top, width, height)
{
   var popupwindow = this.open(url, '', 'toolbar=' + toolbar + ',menubar=' + menubar + ',location=' + locationbar + ',scrollbars=' + scrollbars + ',resizable=' + resize + ',status=' + statusbar + ',left=' + left + ',top=' + top + ',width=1024,height=768');
}

function SetStyle(id, className)
{
   var elem=document.getElementById(id);
   if(elem)
   {
      elem.className=className;
   }
}
 
/* WenJuin@MSTC: add protect Lock, 20130307 */
function ProtectLock()
{
	with(document.forms[0]){
		var loc = 'kennwort_lock.html#';
		loc += 'action=lock'
		var code = 'location="' + loc + '"';
		eval(code);		
	}
}

function fadding()
{
	var spanElement = parent.document.getElementsByTagName("span");
	var inputElement = document.getElementsByTagName("input");
	var aElement = parent.document.getElementsByTagName("a");
	var i;
	
	for (i = 0; i < spanElement.length; i++) 
	{
		if(spanElement[i].id == "headerText")
		{
			spanElement[i].name = "fadding";
			break;
		}
	}
	
	/*tab & button*/
	for (i = 0; i < inputElement.length; i++) 
	{
		if(inputElement[i].type == "button")
		{
			inputElement[i].style.color = "gray";
			inputElement[i].disabled = "disabled";
		}
	}
	
	/*menu & Info A-Z*/
	for (i = 0; i < aElement.length; i++) 
	{
		if(aElement[i].target == "mainframe" || aElement[i].className == "selected_SetupWizard")
		{
			aElement[i].style.color = "gray";
			aElement[i].setAttribute("disabled", "true");
			aElement[i].setAttribute("onclick", "return false");
			
		}
		if(aElement[i].className == "infos_von")
		{
			aElement[i].style.color = "gray";
			aElement[i].removeAttribute("href");
		}
	}
}
function faddingWlReload()
{
	var aElement = parent.document.getElementsByTagName("a");
	var spanElement = parent.document.getElementsByTagName("span");
	var inputElement = document.getElementsByTagName("input");
	var i;
	
	var change = 0;
	
	for (i = 0; i < spanElement.length; i++)
	{
		if(spanElement[i].id == "headerText")
		{
			if(spanElement[i].name == "fadding")
			{
				spanElement[i].name = "none";
				change = 1;
			}
			break;
		}
	}
	/*tab & button*/
	for (i = 0; i < inputElement.length; i++) 
	{
		if(inputElement[i].type == "button")
		{
			inputElement[i].removeAttribute("style");
			inputElement[i].disabled = "";
		}
	}
	if(change == 1)
	{
		/*tab & button*/
		for (i = 0; i < inputElement.length; i++) 
		{
			if(inputElement[i].type == "button")
			{
				inputElement[i].removeAttribute("style");
				inputElement[i].disabled = "";
			}
		}
		/*menu & Info A-Z*/
		for (i = 0; i < aElement.length; i++) 
		{
			//if(aElement[i].target == "mainframe" || aElement[i].className == "selected_SetupWizard")
			if(aElement[i].target == "mainframe" && aElement[i].className != "selected_SetupWizard")
			{
				aElement[i].removeAttribute("style");
				//aElement[i].onclick = "return true";
				aElement[i].setAttribute("onclick","return true");
				aElement[i].removeAttribute("disabled");
			}
			
			//area reset button
			if(aElement[i].className == "selected_SetupWizard")
			{
				aElement[i].removeAttribute("style");
				//aElement[i].onclick = "EinrichtungsassistentOnclick()";
				if(aElement[i].id=="Einrichtungsassistent")
					aElement[i].setAttribute("onclick","EinrichtungsassistentOnclick()");
				else
					aElement[i].setAttribute("onclick","return true");
				
				aElement[i].removeAttribute("disabled");
			}
			
			if(aElement[i].className == "infos_von")
			{
				aElement[i].removeAttribute("style");
				aElement[i].setAttribute("href", "javascript:popupwnd('./Infos_von_index.html','no','no','no','0','no','no','','','1024','768')");
			}	
			
		}
	}	
}	
	
function faddingReload()
{
	var aElement = parent.document.getElementsByTagName("a");
	var spanElement = parent.document.getElementsByTagName("span");
	var i;
	
	var change = 0;
	
	for (i = 0; i < spanElement.length; i++)
	{
		if(spanElement[i].id == "headerText")
		{
			if(spanElement[i].name == "fadding")
			{
				spanElement[i].name = "none";
				change = 1;
			}
			break;
		}
	}
	
	if(change == 1)
	{
		/*menu & Info A-Z*/
		for (i = 0; i < aElement.length; i++) 
		{
			//if(aElement[i].target == "mainframe" || aElement[i].className == "selected_SetupWizard")
			if(aElement[i].target == "mainframe" && aElement[i].className != "selected_SetupWizard")
			{
				aElement[i].removeAttribute("style");
				//aElement[i].onclick = "return true";
				aElement[i].setAttribute("onclick","return true");
				aElement[i].removeAttribute("disabled");
			}
			
			//area reset button
			if(aElement[i].className == "selected_SetupWizard")
			{
				aElement[i].removeAttribute("style");
				//aElement[i].onclick = "EinrichtungsassistentOnclick()";
				if(aElement[i].id=="Einrichtungsassistent")
					aElement[i].setAttribute("onclick","EinrichtungsassistentOnclick()");
				else
					aElement[i].setAttribute("onclick","return true");
				
				aElement[i].removeAttribute("disabled");
			}
			
			if(aElement[i].className == "infos_von")
			{
				aElement[i].removeAttribute("style");
				aElement[i].setAttribute("href", "javascript:popupwnd('./Infos_von_index.html','no','no','no','0','no','no','','','1024','768')");
			}	
			
		}
	}	
}

function LockfaddingReload(protectStatus)
{
	var aElement = parent.document.getElementsByTagName("a");
	var spanElement = parent.document.getElementsByTagName("span");
	var divElement = parent.document.getElementsByTagName("div");
	var i;
	
	var change = 0;
	
	for (i = 0; i < spanElement.length; i++)
	{
		if(spanElement[i].id == "headerText")
		{
			if(spanElement[i].name == "fadding")
			{
				spanElement[i].name = "none";
				change = 1;
			}
			break;
		}
	}
	
	if(change == 1)
	{
		/*menu & Info A-Z*/
		for (i = 0; i < aElement.length; i++) 
		{
			//if(aElement[i].target == "mainframe" || aElement[i].className == "selected_SetupWizard")
			if(aElement[i].target == "mainframe" && aElement[i].className != "selected_SetupWizard")
			{
				aElement[i].removeAttribute("style");
				//aElement[i].onclick = "return true";
				aElement[i].setAttribute("onclick","return true");
				aElement[i].removeAttribute("disabled");
			}
			
			//area reset button
			if(aElement[i].className == "selected_SetupWizard")
			{
				aElement[i].removeAttribute("style");
				//aElement[i].onclick = "EinrichtungsassistentOnclick()";
				if(aElement[i].id=="Einrichtungsassistent")
					aElement[i].setAttribute("onclick","EinrichtungsassistentOnclick()");
				else
					aElement[i].setAttribute("onclick","return true");
				
				aElement[i].removeAttribute("disabled");
			}
			
			if(aElement[i].className == "infos_von")
			{
				aElement[i].removeAttribute("style");
				aElement[i].setAttribute("href", "javascript:popupwnd('./Infos_von_index.html','no','no','no','0','no','no','','','1024','768')");
			}	
			
		}
		
		
		for (i = 0; i < divElement.length; i++) 
		{
			if(divElement[i].id == "lock")
			{
				if(protectStatus == 0)
				{
					divElement[i].setAttribute("style", "display:none");
				}
			}
		}
	}
}


function fadding_setupWizard()
{
	var spanElement = parent.document.getElementsByTagName("span");
	var inputElement = document.getElementsByTagName("input");
	var aElement = parent.document.getElementsByTagName("a");
	var i;
	
	for (i = 0; i < spanElement.length; i++) 
	{
		if(spanElement[i].id == "headerText")
		{
			spanElement[i].name = "fadding";
			break;
		}
	}
	
	/*tab & button*/
	for (i = 0; i < inputElement.length; i++) 
	{
		if(inputElement[i].type == "button")
		{
			inputElement[i].style.color = "gray";
			inputElement[i].disabled = "disabled";
		}
	}
	
	/*Info A-Z*/
	for (i = 0; i < aElement.length; i++) 
	{
		if(aElement[i].className == "infos_von")
		{
			aElement[i].style.color = "gray";
			aElement[i].removeAttribute("href");
			break;
		}
	}
}


function showMsg(Msg)
{
	var element = document.getElementById('help');
	var page = element.getAttribute('name');
	var str = '';
	
	if(element)
	{
		if(isIE8)
		{
			str = "<table cellspacing='0' cellpadding='0' width='170' class='helpTable'>";
			str += "<tr><td class='blue_head_ie8'>Hilfe</td></tr>";
			str += "<tr><td class='blue_text_ie8'>" + Msg + "</td></tr>";
			str += "<tr><td class='blue_text_bottom_ie8'></td></tr>";
			str += "</table>";
		}
		else
		{
			str = "<table cellspacing='0' cellpadding='0' width='170' class='helpTable'>";
			str += "<tr><td class='blue_head'>Hilfe</td></tr>";
			str += "<tr><td class='blue_text'>" + Msg + "</td></tr>";
			str += "</table>";
		}

		element.innerHTML = str;
	}
}

function showErrWithoutHint(errMsg)
{
	var element = document.getElementById('help');
	var str = '';
	
	if(element)
	{
		if(isIE8)
		{
			str = "<table cellspacing='0' cellpadding='0' width='170' class='helpTable'>";
			str += "<tr><td class='red_head_ie8'>Fehler</td></tr>";
			str += "<tr><td class='red_errText_ie8'>" + errMsg + "</td></tr>";
			str += "<tr><td><a href='#' onclick=\"closeErr()\"; style='text-decoration: none;color: #FFFFFF;'><div class='red_tail_ie8'>Schließen</div></a></td></tr>";
			str += "</table>";
		}
		else
		{
			str = "<table cellspacing='0' cellpadding='0' width='170' class='helpTable'>";
			str += "<tr><td class='red_head'>Fehler</td></tr>";
			str += "<tr><td class='red_errText'>" + errMsg + "</td></tr>";
			str += "<tr><td><a href='#' onclick=\"closeErr()\"; style='text-decoration: none;color: #FFFFFF;'><div class='red_tail'>Schließen</div></a></td></tr>";
			str += "</table>";
		}

		element.innerHTML = str;
	}
}

function closeErr()
{
	var element = document.getElementById('help');
	var str = '';
	element.innerHTML = str;
}

function showHintBBI()
{
	var element = document.getElementById('help');
	var msg = getHelpText('hint_BBI');
	
	if(element)
	{
		if(isIE8)
		{
			str = "<table cellspacing='0' cellpadding='0' width='170' class='helpTable'>";
			str += "<tr><td class='red_head_ie8'>Hinweis</td></tr>";
			str += "<tr><td class='red_text_ie8'>" + msg + "</td></tr>";
			str += "<tr><td class='red_text_bottom_ie8'></td></tr>";
			str += "</table>";
		}
		else
		{
			str = "<table cellspacing='0' cellpadding='0' width='170' class='helpTable'>";
			str += "<tr><td class='red_head'>Hinweis</td></tr>";
			str += "<tr><td class='red_text'>" + msg + "</td></tr>";
			str += "</table>";
		}
		
		element.innerHTML = str;
	}
	
}

function showDectRegisterWait(Msg)
{
	var element = document.getElementById('help');
	var msgwait = getHelpText('wait');
	
	if(element)
	{
		if(isIE8)
		{
			str = "<table cellspacing='0' cellpadding='0' width='170' class='helpTable'>";
			str += "<tr><td class='red_head_ie8'>Hinweis</td></tr>";
			str += "<tr><td class='red_text_ie8'>" + msgwait + "</td></tr>";
			str += "<tr><td class='red_text_bottom_ie8'></td></tr>";
			str += "<tr><td><p></td></tr>";
			str += "<tr><td height='18'></td></tr>";
			str += "<tr><td class='blue_head_ie8'>Hilfe</td></tr>";
			str += "<tr><td class='blue_text_ie8'>" + Msg + "</td></tr>";
			str += "<tr><td class='blue_text_bottom_ie8'></td></tr>";
			str += "</table>";
		}
		else
		{
			str = "<table cellspacing='0' cellpadding='0' width='170' class='helpTable'>";
			str += "<tr><td class='red_head'>Hinweis</td></tr>";
			str += "<tr><td class='red_text'>" + msgwait + "</td></tr>";
			str += "<tr><td><p></td></tr>";
			str += "<tr><td height='18'></td></tr>";
			str += "<tr><td class='blue_head'>Hilfe</td></tr>";
			str += "<tr><td class='blue_text'>" + Msg + "</td></tr>";
			str += "</table>";
		}
		
		element.innerHTML = str;
	}
}

function showRebootMsg()
{
	var element = document.getElementById('help');
	var msg = getHelpText('rebootMsg');
	
	if(element)
	{
		if(isIE8)
		{
			str = "<table cellspacing='0' cellpadding='0' width='170' class='helpTable'>";
			str += "<tr><td class='red_head_ie8'>Hinweis</td></tr>";
			str += "<tr><td class='red_text_ie8'>" + msg + "</td></tr>";
			str += "<tr><td class='red_text_bottom_ie8'></td></tr>";
			str += "</table>";
		}
		else
		{
			str = "<table cellspacing='0' cellpadding='0' width='170' class='helpTable'>";
			str += "<tr><td class='red_head'>Hinweis</td></tr>";
			str += "<tr><td class='red_text'>" + msg + "</td></tr>";
			str += "</table>";
		}
		
		element.innerHTML = str;
	}
	
}

function showErr(errMsg)
{
	var element = document.getElementById('help');
	var page = element.getAttribute('name');
	var str, msg;
	
	msg = "Schließen Sie die Fehlermeldung, um den Hilfetext erneut einzublenden.";
	
	
	if(isIE8)
	{
		str = "<table cellspacing='0' cellpadding='0' width='170' class='helpTable'>";
		str += "<tr><td class='red_head_ie8'>Fehler</td></tr>";
		str += "<tr><td class='red_errText_ie8'>" + errMsg + "</td></tr>";
		str += "<tr><td><a href='#' onclick=\"showHelp('" + page + "')\"; style='text-decoration: none;color: #FFFFFF;'><div class='red_tail_ie8'>Schließen</div></a></td></tr>";
		str += "<tr><td height='18'></td></tr>";
		str += "<tr><td class='blue_head_ie8'>Hilfe</td></tr>";
		str += "<tr><td class='blue_text_ie8'>" + msg + "</td></tr>";
		str += "<tr><td class='blue_text_bottom_ie8'></td></tr>";
		str += "</table>";
	}
	else
	{
		str = "<table cellspacing='0' cellpadding='0' width='170' class='helpTable'>";		
		str += "<tr><td class='red_head'>Fehler</td></tr>";
		str += "<tr><td class='red_errText'>" + errMsg + "</td></tr>";
		str += "<tr><td><a href='#' onclick=\"showHelp('" + page + "')\"; style='text-decoration: none;color: #FFFFFF;'><div class='red_tail'>Schließen</div></a></td></tr>";
		str += "<tr><td height='18'></td></tr>";
		str += "<tr><td class='blue_head'>Hilfe</td></tr>";
		str += "<tr><td class='blue_text'>" + msg + "</td></tr>";
		str += "</table>";
	}
		
	element.innerHTML = str;
	

	/*
	//auto adjust height
	var helpHeight = document.getElementById('help').offsetHeight; //offsetHeight = 400;
	var minHeight = document.getElementById('defaultId').style.minHeight; //minHight = 400px;
	
	//minHeightGlobal = minHeight;
	
	minHeight = minHeight.split("px");
	minHeight = minHeight[0];
	minHeight = Number(minHeight);
	//alert(minHeight);
	
	helpHeight = Number(helpHeight);
	//alert(helpHeight);
	
	//length of help msg larger than length of content, lengthen it.
	if(minHeight < helpHeight)
	{
		helpHeight = document.getElementById('help').offsetHeight + 30 + "px"; //bg_bottom_bar:30
		document.getElementById("defaultId").style.minHeight = helpHeight;
	}*/
}


function showWait()
{
	var element = document.getElementById('help');
	var msg = getHelpText('wait');
	
	
	if(isIE8)
	{
		str = "<table cellspacing='0' cellpadding='0' width='170' class='helpTable'>";	
		str += "<tr><td class='red_head_ie8'>Hinweis</td></tr>";
		str += "<tr><td class='red_text_ie8'>" + msg + "</td></tr>";
		str += "<tr><td class='red_text_bottom_ie8'></td></tr>";
		str += "</table>";
	}
	else
	{
		str = "<table cellspacing='0' cellpadding='0' width='170' class='helpTable'>";
		str += "<tr><td class='red_head'>Hinweis</td></tr>";
		str += "<tr><td class='red_text'>" + msg + "</td></tr>";
		str += "</table>";
	}
		
	element.innerHTML = str;
	
	
}
function showHelp()
{
	var element = document.getElementById('help');
	var page = element.getAttribute('name');
	var str, msg;
	
	msg = getHelpText(page);
		
	if(isIE8)
	{
		str = "<table cellspacing='0' cellpadding='0' width='170' class='helpTable'>";
		str += "<tr><td class='blue_head_ie8'>Hilfe</td></tr>";
		str += "<tr><td class='blue_text_ie8'>" + msg + "</td></tr>";
		str += "<tr><td class='blue_text_bottom_ie8'></td></tr>";
		str += "</table>";
	}
	else
	{
		str = "<table cellspacing='0' cellpadding='0' width='170' class='helpTable'>";
		str += "<tr><td class='blue_head'>Hilfe</td></tr>";
		str += "<tr><td class='blue_text'>" + msg + "</td></tr>";
		str += "</table>";
	}
		
	
	element.innerHTML = str;
		
		
	/*auto adjust height*/
   if(minHeightGlobal != 0)
	{
		document.getElementById('defaultId').style.minHeight = minHeightGlobal; //restore to original length
	}
	//alert(minHeightGlobal);
}

function getHelpText(page)
{
	var str = "";
	
	switch(page)
	{
		
		/*Ubersicht*/
		case 'Ubersicht':
			str = "Hier erhalten Sie einen Überblick über den aktuellen Status der o2 HomeBox. <br>Wenn Sie bereits eine Internetverbindung über Ihren PC aufgebaut haben (DFÜ-, bzw. PPPoE-Verbindung), wird Ihnen diese Verbindung hier im Online-Status nicht angezeigt. Konfigurieren Sie die o2 HomeBox ganz einfach mit dem Einrichtungsassistenten.";
			break;
			
		/*Internet*/
		case 'Zugansdaten_ADSL':
			str = "Legen Sie hier die Art der Internet-Verbindung fest. Wenn Sie einen zeitbasierten Tarif (z.B. Tarife mit Minutenabrechnung) nutzen, wählen Sie \"Zeitbasierte Verbindung\", um hohe Verbindungskosten durch eine dauerhafte Verbindung zu vermeiden. Zum  Herstellen oder Trennen einer manuellen Verbindung muss immer diese Bedienoberfläche über Ihren Browser geöffnet werden.";
			break;
		case 'Zugansdaten_VDSL_ETH':
			str = "Ändern Sie hier bei Bedarf Ihre Zugangsdaten. Mit der Übernahme durch den \"Speichern\"-Button überprüft Ihre o2 Box die Verbindung. Den Status können Sie in der Seite \"Übersicht\" und an der Internet-LED Ihres Geräts ablesen. <br>Legen Sie hier auch die Art der Internet-Verbindung fest.";
			break;
		case 'Zugansdaten_single':
			str = "In diesem Menü sind keine Einstellungen nötig.";
			break;
		case 'Weitere':
			str = "Mit PPPoE Pass Through haben Sie die Möglichkeit, angeschlossenen Netzwerkgeräten eine eigene Internet-Verbindung über die o2 HomeBox zu ermöglichen bzw. diese zu unterbinden. Deaktivieren Sie PPPoE Pass Through, können z.B. PCs mit eigenen Internet-Zugangsdaten keine Internet-Verbindung mehr herstellen. <br>Den Zeitpunkt der von o2 netzseitig vorgenommenen Zwangstrennung der Verbindung innerhalb von 24h können Sie hier selbst vorwählen, um die Unterbrechung bestehender Anwendungen, z.B. File-Transfer zu beeinflussen.";
			break;
		case 'IP_Adressen':
			str = "Hier erhalten Sie die detaillierten Verbindungsdaten Ihrer o2 HomeBox.";
			break;
		case 'DynDNS':
			str = "Über DynDNS können Anwendungen und Dienste, für die in der o2 HomeBox Portfreigaben eingerichtet wurden, unter einem festen Domainnamen aus dem Internet erreicht werden, obwohl sich die öffentliche IP-Adresse des Geräts mit jeder Internet-Einwahl ändert.";
			break;
		case 'DynDNS_add':
			str = "Aktivieren Sie das DynDNS-Konto und wählen Sie Ihren Anbieter aus der Liste aus. Tragen Sie die Zugangsdaten ein, die Sie von Ihrem Anbieter bekommen haben.  Klicken Sie abschließend auf <b>Speichern</b>.";
			break;
			
		/*Telefonie*/
		case 'T_Ubersicht':
			str = "Hier erhalten Sie den aktuellen Telefonie-Status Ihrer o2 HomeBox. Ihre o2 HomeBox verfügt über eine integrierte DECT-Basisstation. Wie Sie Ihre DECT-Mobilteile anmelden, erfahren Sie in der Bedienungsanleitung. Ebenfalls ist ein TK-Anlagenmodus integriert Sie können über den S0-Anschluss an der Geräterückseite eine ISDN-Anlage anschließen und den daran angeschlossenen Telekommunikations-geräten Rufnumnern zuweisen. Dies ist ebenfalls bei direkt an Ihre o2 HomeBox angeschlossenen Geräten möglich. Bitte beachten Sie jedoch hierzu Ihre o2 Tarifdetails.";
			break;
		case 'Weckruf':
			str = "Hier können Sie die Weckruf-Funktion Ihrer o2 HomeBox einstellen. Zur. eingestellten Uhrzeit klingeln alle angeschlossenen Endgeräte bis zum Drücken der Annahmetaste eines Endgeräts.";
			break;
		case 'Anruflisten':
			str = "Hier werden Ihnen die 100 zuletzt getätigten, angenommenen und versäumten Anrufe aufgelistet.<br><br>Sie können die Liste bei Bedarf löschen.";
			break;
		case 'Telefonbuch':
			str = "In Ihrem Telefonbuch können Sie bis zu 250 Einträge speichern. Ihre Anrufliste ersetzt automatisch im Telefonbuch hinterlegte Rufnummern mit den entsprechenden Namen.";
			//str = "Sie können bis zu 250 Einträge speichern.";
			//str = "Das Telefonbuch kann als Datei gesichert werden. Gehen die Daten auf Grund eines Defektes oder Zurücksetzens in den Auslieferungszustand verloren, kann das Telefonbuch über diese Datei wieder importiert werden. Sie können bis zu 250 Einträge speichern, exportieren und importieren. Möchten Sie ein Telefonbuch mit mehr als 250 Einträgen importieren, werden nur die ersten 250 Einträge übernommen.";
			//str = "Das Telefonbuch kann als Datei gesichert werden.Gehen die Daten auf Grund eines Defektes  oder Zurücksetzens in den Auslieferungs-<wbr>zustand verloren, kann das Telefonbuch über diese Datei wieder importiert  werden.";
			//str = "Hier finden Sie Ihre persönliche Telefonbuchliste. Sie können Einträge hinzufügen, ändern und löschen.";
			break;
		case 'Telefonbuch_add':
			str = "Vor- und Nachname lhrer Einträge dürfen bis zu 32 Zeichen lang sein (0-9, a-z, A-Z). Bitte geben Sie immer den Vor- und Nachnamen ein. Vorwahl und Rufnummer können jeweils eine Länge von 32 Ziffern haben.";
			break;
		case 'DECT':
			str = "Ihre o2 HomeBox verfügt über eine integrierte TK-Anlage. Sie können über die integrierte DECT-Basisstation bis zu 5 Schurlostelefone anmelden oder zwei Analogtelefone über die hinteren Buchsen anschließen. Die Anmeldung eines DECT-Telefons starten Sie durch Drücken von <b>Mobilteil hinzufügen</b>.";
			break;
		case 'DECT_add':
			str = "Hier können Sie neue Schnurlostefone an die integrierte DECT-Basis anmelden oder bereits angebundene Mobilteile abmelden. Weiterhin können Sie in diesem Menü interne Rufnummern den angemeldeten Mobilteilen zuordnen. Zum Anmelden neuer Schnurlostelefone benötigen Sie die PIN (Standardeinstellung: \"0000\"). Wie Sie lhr Schnurlostelefon mit einer DECT-Basis koppeln erfahren Sie in der dazugehörigen Bedienungsanleitung. Weiterhin können Sie den gekoppelten Telefonen Namen mit max. 16 Zeichen (a-z, A-Z, 0-9) und interne Rufnummern (10 -99) zuorden. ";
			break;
		case 'Analog':
			str = "An Ihre o2 HomeBox können zwei analoge Endgeräte angeschlossen werden. Diesen können Namen (max. 16 Zeichen, a-z, A-Z. 0-9) sowie interne Rufnummern (max. 2 Zeichen, 10-99, keine Doppelvergabe) zugewiesen werden.";
			break;
		case 'Analog_add':
			str = "Der Gerätename darf maximal 16 Zeichen lang sein und aus Klein- und Großbuchstaben (a-z, A-Z) sowie Zahlen (0-9) bestehen. Die interne Rufnummer hat Werte von 10-99. Bitte beachten Sie, dass eine Mehrfachvergabe von internen Rufnummern (DECT, Analog, ISDN) nicht möglich ist.";
			break;
		case 'ISDN':
			str = "Hier können Sie Ihre ISDN-Geräte anmelden, interne Rufnummern vergeben sowie den angeschlossenen Geräten Namen geben (max. 16 Zeichen, a-z, A-Z, 0-9). Für die interne Rufnummernvergabe sind 2 Zeichen (10-99) zulässig. Eine Doppelvergabe ist nicht möglich Hinweis: Bitte vergewissern Sie sich, dass Ihr ISDN-Telefon mit der vergebenen internen Rufnummer angemeldet wird.";
			break;
		case 'ISDN_add':
			str = "Hier können Sie Ihre ISDN-Geräte anmelden, interne Rufnummern vergeben sowie den angeschlossenen Geräten Namen geben (max. 16 Zeichen, a-z, A-Z, 0-9). Für die interne Rufnummernvergabe sind 2 Zeichen (10-99) zulässig. Eine Doppelvergabe ist nicht möglich Hinweis: Bitte vergewissern Sie sich, dass Ihr ISDN-Telefon mit der vergebenen internen Rufnummer angemeldet wird.";
			break;
		case 'Interne':
			str = "Hier können Sie die Rufnummern den einzelnen Telefoniegeräten zuordnen. Interne Rufnummern lassen sich von 10 - 99 vergeben. Zur internen Anwahl betätigen Sie auf Ihrem Telefon: ** und anschließend die gewünschte zweistellige interne Rufnummer. ";
//			str = "Für ISDN-Geräte kann Die Funktion Anklopfen nicht bearbeitet werden, Dies wird durch das angeschlossene ISDN-Telefon vorgegegeben. ";
			break;
		case 'Eingehende':
			str = "Hier können Sie die Rufnummern den einzelnen Telefoniegeräten zuordnen. Interne Rufnummern lassen sich von 10 - 99 vergeben. Zur internen Anwahl betätigen Sie auf Ihrem Telefon: ** und anschließend die gewünschte zweistellige interne Rufnummer. Bei ISDN Telefonen wird die Funktion Anklopfen über das Menü des angeschlossenen ISDN Telefons eingerichtet.";
//			str = "Für ISDN-Geräte kann Die Funktion Anklopfen nicht bearbeitet werden, Dies wird durch das angeschlossene ISDN-Telefon vorgegegeben. ";
			break;
		case 'Ausgehende':
			str = "Hier können Sie die Rufnummern den einzelnen Telefoniegeräten zuordnen.";
			break;
			
		/*Heimnetz*/
		case 'H_Ubersicht':
			str = "Hier finden Sie Informationen zum Netzwerkstatus Ihrer o2 HomeBox sowie eine Übersicht der im Heimnetz bzw. per USB angeschlossenen Geräte.<br>Den Status Ihrer UPnP-Konfiguration können Sie im Menü Sicherheit einsehen.";
			//str = "Hier wird der Netzwerkstatus Ihrer o2 HomeBox sowie der Status der angeschlossenen Heimnetz-Geräte angezeigt.";
			break;
		case 'DHCPv4':
			str = "Hier verwalten Sie die Netzwerk-Einstellungen. Sie finden hier die IP-Adresse und die Subnetzmaske Ihres Gerätes und können diese anpassen. <br>Beachten Sie bei Änderung der IP-Adresse Ihrer o2 HomeBox, dass diese nicht im Adressbereich des DHCP-Servers liegen darf. Dessen Einstellungen verwenden Sie, wenn Sie automatisch IP-Adressen für Ihre angeschlossenen Geräte vergeben möchten. <br>Hinweis: Änderungen auf dieser Seite können dazu führen, dass lhre o2 HomeBox nicht mehr erreichbar ist. (nur für erfahrene Nutzer)";
			break;
		case 'DHCPv4_add':
			str = "Tragen Sie die erforderlichen Daten ein, um einen neuen Eintrag anzulegen. Klicken Sie danach auf <b>Speichern</b>, um die Änderungen zu sichern. Hinweis: Ändern Sie die Einstellungen nur als erfahrener Nutzer.";
			break;
		case 'DHCPv4_edit':
			str = "Markieren Sie einen Eintrag in der Liste, um bestehende Einstellungen zu ändern. Klicken Sie danach auf <b>Speichern</b>, um die Änderungen zu sichern. Hinweis: Ändern Sie die Einstellungen nur als erfahrener Nutzer.";
			break;
		case 'DHCPv6':
			str = "Wählen Sie hier die Einstellung des DHCPv6-Servers. Ändern Sie diese Einstellungen nur als erfahrener Benutzer.";
			//str = "Wählen Sie hier die Einstellung des DHCPv6-Servers.";
			//str = "Wählen Sie hier die Einstellung des DHCPv6-Servers. Hier finden Sie eine Liste der fest zugeordneten IP-Adressen in Ihrem Heimnetzwerk. In Ergänzung zur DHCP-Adressvergabe wählen Sie hier die Einstellungen der lP-Schnittstellen für ausgewählte Geräte in Ihrem Netzwerk. Klicken Sie auf <b>Neu</b>, um ein Gerät hinzuzufügen oder markieren Sie einen Eintrag und klicken Sie auf <b>Bearbeiten</b>, um bestehende Einstellungen zu ändern. Hinweis: Ändern Sie die Einstellungen nur als erfahrener Nutzer.";
			break;
		case 'DHCPv6_add':
			str = "Hier können Sie eine IPv6 Adresse für ein Gerät festsetzen.<p> Tragen Sie die Daten manuell ein oder wählen Sie ein Gerät von der Liste aus, damit werden alle Felder automatisch ausgefüllt.<p>Der IPv6 Präfix wird automatisch von IPv6 Delegierten-Präfix eingerechnet und immer automatisch aktualisiert gewartet.";
			break;
		case 'IPv6':
			str = "Änderungen auf dieser Seite können dazu führen, dass Ihre o2 HomeBox nicht mehr erreichbar ist. Verringern Sie die MTU-Größe (Maximum Transmission Unit), um ggf. Verbindungsprobleme Ihrer angeschlossenen Endgeräte zu lösen. Diese Option zeigt Ihnen die Aktuelle MTU. Die „Aktuelle IPv6 MTU“ wird automatisch aus dem Internet bestimmt oder von der GUI übernommen, je nachdem welcher Wert kleiner ist.";
			break;
		case 'Basisstation':
			str = "Hier ist der Status Ihrer WLAN-Basisstation ersichtlich. Ist WLAN eingeschaltet wird dies durch die Schaltfläche Ausschalten signalisiert und umgekehrt. Schalten Sie hier bei Bedarf die integrierte WLAN- Basisstation  ein oder aus. Alternativ nutzen Sie dafür den WLAN-Taster an der Geräteoberseite. Aktivieren Sie einzeln die beiden, separat arbeitenden WLAN-Module, um Sie Ein-bzw. Auszuschalten und ändern Sie die SSID und stellen Sie den Modus für die WLAN-Sendeleistung ein.<br>Weitere Informationen finden Sie im Benutzer-handbuch. Die DFS-Kanäle werden nur verzögert aktiviert, <font color=\"gray\">dies kann ca. 1 Minute dauern.</font> Ebenso kann es sein, dass ein anderer Kanal als der eingestellte aktiviert wird.";
			break;
		case 'Verschlusselung':
			str = "Verschlüsseln Sie lhre Verbindung, um lhr Funknetz vor unbefugten Zugriffen zu schützen. Ändern Sie zur zusätzlichen Sicherheit den voreingestellten  WLAN-Schlüssel in eine  möglichst lange Kombination von Buchstaben und Ziffern. Verfügt lhr Computer oder vorhandene WLAN-Geräte bereits über das modernere Verschlüsselungsverfahren WPA bzw. WPA2, nutzen Sie unbedingt dieses neue Verfahren. Verbinden Sie lhre Netzwerkgeräte, die WPS Push Button unterstützen, indem Sie den WLAN-Taster der o2 HomeBox bei bereits aktiviertem WLAN für mindestens 5 Sekunden drücken und danach innerhalb von 2 Minuten den WPS-Taster an lhrem Netzwerkgerät.";
			break;
		case 'Nachtschaltung':
			str = "Hier können Sie für jeden Wochentag einen Zeitraum festlegen, zu dem Ihre WLAN-Basisstation automatisch ausgeschaltet werden soll. Jede Regel kann ein- bzw. ausgeschaltet werden. Über Zurücksetzen werden alle Regeln gelöscht.<br> Sollte zum definierten Deaktivierungszeitpunkt noch eine WLAN-Verbindung bestehen, wird diese beibehalten. Möchten Sie in diesem Fall eine Deaktivierung erzwingen, setzen Sie zusätzlich das betreffende Häkchen.";
			break;
		case 'IPv4':
			str = "Durch Port Forwarding wird es Netzwerkgeräten innerhalb eines lokalen Netzes möglich, auch außerhalb dieses Netzes zu arbeiten. Sie sind damit auch für andere Computer erreichbar. Dies ist z. B. wichtig, um Daten über das Internet auszutauschen oder bei Spielen im Netz mit anderen Computern zu kommunizieren. Aktivieren Sie diese Funktion generell durch das Setzen des Häkchens \"Port Forwarding aktivieren\". Einzelne Port Forwardings können Sie über die Liste zulassen bzw. deaktivieren.";
			break;
		case 'UPnP_IPv4':
			str = "Durch Port Forwarding wird es Netzwerkgeräten innerhalb eines lokalen Netzes möglich, auch außerhalb dieses Netzes zu arbeiten. Sie sind damit auch für andere Computer erreichbar. Dies ist z. B. wichtig, um Daten über das Internet auszutauschen oder bei Spielen im Netz mit anderen Computern zu kommunizieren. Aktivieren Sie diese Funktion generell durch das Setzen des Häkchens \"Port Forwarding aktivieren\". Einzelne Port Forwardings können Sie über die Liste zulassen bzw. deaktivieren. Haben Sie die Funktion UPnP- Port-Weiterleitung im Menü Sicherheit IGD/UPnP aktiviert, werden Ihnen die Ports angezeigt, die aktuell von einem Dienst geöffnet wurden.";
			break;
		case 'IPv4_add':
			str = "Erstellen Sie hier neue Port Forwarding-Einträge. Damit diese wirksam werden, müssen Sie diese Regel aktivieren. Übernehmen Sie die Eingaben mit Klick auf <b>Speichern</b>.";
			break;
		case 'IPv4_edit':
			str = "Erstellen Sie hier neue Port Forwarding-Einträge. Damit diese wirksam werden, müssen Sie das Häkchen \"Port Forwarding aktiv\" setzen. Übernehmen Sie die Eingaben mit Klick auf <b>Speichern</b>.";
			break;
		case 'P_IPv6':
			str = "Durch Port Forwarding wird es Netzwerkgeräten innerhalb eines lokalen Netzes möglich, auch außerhalb dieses Netzes zu arbeiten. Sie sind damit auch für andere Computer erreichbar. Dies ist z. B. wichtig, um Daten über das Internet auszutauschen oder bei Spielen im Netz mit anderen Computern zu kommunizieren. Aktivieren Sie diese Funktion generell durch das Setzen des Häkchens \"Port Forwarding aktivieren\". Einzelne Port Forwardings können Sie über die Liste zulassen bzw. deaktivieren.";
			break;
		case 'P_IPv6_add':
			str = "Erstellen Sie hier neue Port Forwarding-Einträge. Damit diese wirksam werden, müssen Sie diese Regel aktivieren. Übernehmen Sie die Eingaben mit Klick auf <b>Speichern</b>.";
			break;
		case 'P_IPv6_edit':
			str = "Erstellen Sie hier neue Port Forwarding-Einträge. Damit diese wirksam werden, müssen Sie das Häkchen \"Port Forwarding aktiv\" setzen. Übernehmen Sie die Eingaben mit Klick auf <b>Speichern</b>.";
			break;
		case 'Massenspeicher':
			str = "Hier sehen Sie den Status der über die USB 2.0-Ports angeschlossenen Massenspeicher. Mit Hilfe eines USB-Hubs können Sie weitere Datenträger an Ihre o2 HomeBox anschließen und im Heimnetz nutzen.<br>Ein FTP-Zugriff über Safari ist nicht möglich, bitte installieren Sie einen alternativen Browser (zum Beispiel Mozilla Firefox für MAC) oder verwenden Sie ein FTP-Programm.";
			//str = "Hier können Sie die Unterstützung für den USB-Geräteanschluss konfigurieren und den Status angeschlossener USB-Geräte sehen. Als USB-Geräte werden Massenspeicher (sogenannte USB-Sticks) zur Übertragung der WLAN-<br>Sicherheitseinstellungen (Drahtlos-netzwerkinstallation) und USB-Drucker erkannt. Ein FTP-Zugriff ist über Safari leider nicht möglich, Installieren Sie einen alternativen Browser (zum Beispiel Mozilla Firefox für MAC) oder verwenden Sie ein FTP-Programm.";
			break;
		case 'Drucker':
			str = "Hier finden Sie eine Übersicht aller von der o2 HomeBox automatisch erkannten Drucker. Sie können bis zu 4 Geräte über einen externen USB-Hub anschließen.<br>Die Installation Ihres Druckers können Sie über die Systemsteuerung/Geräte und Drucker vornehmen. Für Details lesen Sie bitte das Handbuch auf der CD.";
			//str = "Hier sehen Sie den Status von der o2 HomeBox erkannter Drucker. Drucker werden automatisch am USB-Port erkannt und werden mit dem Druckernamen angezeigt. Sie können bis zu 4 Drucker über einen USB-Hub anschließen. Die Installation des Druckers erfolgt über Windows. Geben Sie dazu den hinter dem Drucker angegebenen Pfad (z.B. \\\\\o2.box\\lp0) im Datei-Explorer ein und folgen Sie den Anweisungen.";
			break;
		case 'Nutzerverwaltung':
			str = "Verwalten Sie hier Ihre Freigaben für die an Ihrer o2 HomeBox angeschlossenen Massenspeicher. Über diese Zugriffsberechtigung können Sie unerlaubtes Lesen, Beschreiben oder Löschen von Daten auf Ihren Speichermedien verhindern. Sie können Nutzern sowohl Lese- als auch Schreib-/Leserechte einräumen. Ist kein Nutzer angelegt, haben alle Nutzer vollen Schreib-/Lesezugriff.";
			//str = "Verwalten Sie hier Ihre Freigaben für an der o2 HomeBox angeschlossene Massenspeicher. Sie können neue Nutzer anlegen und Ihnen sowohl Lese also auch Schreib- und Lese rechte für an dem USB-Port angeschlossene Massenspeicher einräumen.";
			break;
		case 'Nutzerverwaltung_add':
			str = "Erstellen und verwalten Sie hier Benutzerkonten für angeschlossene USB-Massenspeicher. Über den Zugriffstyp können Sie Lese- und Schreibrechte für Ihren Massenspeicher vergeben. Das Kennwort muss mind. 5 Zeichen umfassen und kann max. 16 Zeichen lang sein (a-z A-Z 0-9).";
			break;
		case 'Nutzerverwaltung_edit':
			str = "Erstellen und verwalten Sie hier Benutzerkonten für angeschlossene USB-Massenspeicher. Über den Zugriffstyp können Sie Lese- und Schreibrechte für Ihren Massenspeicher vergeben. Das Kennwort muss mind. 5 Zeichen umfassen und kann max. 16 Zeichen lang sein (a-z A-Z 0-9).";
			break;
		case 'MediaCenter':
			str = "Um die Nutzung von Medien im Heimnetzwerk zu ermöglichen, geben Sie zunächst Ordner auf dem Speicher frei, z.B. meine Fotos. Anschließend können Sie die Aktualisierung des Speichers über <b>Jetzt Speicher scannen</b> einrichten bzw. durchführen. Nach Entfernen eines Speichers werden die eingerichteten Ordner weiterhin angezeigt. Die Medien sind dann allerdings nicht mehr verfügbar bis der Speicher wieder mit der HomeBox verbunden wird.";
			break;
		case 'MediaCenter_add':
			str = "Der zu vergebene Name des freizugebenden Ordners darf maximal 16 Zeichen lang sein. Folgende Zeichen dürfen verwendet werden: 0-9, a-z, A-Z.<br>Dateien im Stammverzeichnis des Speichers werden sofort frei gegeben und können nicht mit einer Freigabe verwaltet werden.";
			break;
			
		/*Sicherheit*/
		case 'S_IPv4':
			str = "Hier haben Sie die Möglichkeit, für einzelne Rechner oder Subnetze spezielle Filterregeln einzurichten, die die Firewall zusätzlich verstärken. Bearbeiten Sie bereits angelegte Regeln bzw. legen Sie eine neue Regel an. Diese Funktion sollte nur von erfahrenen Nutzern aktiviert werden.";
			break;
		case 'S_IPv4_add':
			str = "Wählen Sie ein Protokoll über das der zu sperrende Dienst erreichbar ist. In der Heimnetz-Konfiguration können Sie einzelne PCs definieren, für die diese Regel gilt, aber auch das gesamte Subnetz sperren. Eine Angabe des Portbereichs schränkt die Sperre ein. Bleibt die Angabe leer, gilt die Sperre für den gesamten Portbereich. Die Internet (Webseiten)-Konfiguration bezieht sich auf die zu beschränken-de Webseite (Tpy:Einzeln) bzw. ein gesamtes Subnetz.";
			break;
		case 'S_IPv4_edit':
			str = "Wählen Sie ein Protokoll über das der zu sperrende Dienst erreichbar ist. In der Heimnetz-Konfiguration können Sie einzelne PCs definieren, für die diese Regel gilt, aber auch das gesamte Subnetz sperren. Eine Angabe des Portbereichs schränkt die Sperre ein. Bleibt die Angabe leer, gilt die Sperre für den gesamten Portbereich. Die Internet (Webseiten)-Konfiguration bezieht sich auf die zu beschränken-de Webseite (Tpy:Einzeln) bzw. ein gesamtes Subnetz.";
			break;
		case 'S_IPv6':
			str = "Hier haben Sie die Möglichkeit, für einzelne Rechner oder Subnetze spezielle Filterregeln einzurichten, die die Firewall zusätzlich verstärken. Bearbeiten Sie bereits angelegte Regeln bzw. legen Sie eine neue Regel an. Diese Funktion sollte nur von erfahrenen Nutzern aktiviert werden.";
			break;
		case 'S_IPv6_add':
			str = "Wählen Sie ein Protokoll über das der zu sperrende Dienst erreichbar ist. In der Heimnetz-Konfiguration können Sie einzelne PCs definieren, für die diese Regel gilt, aber auch das gesamte Subnetz sperren. Eine Angabe des Portbereichs schränkt die Sperre ein. Bleibt die Angabe leer, gilt die Sperre für den gesamten Portbereich. Die Internet (Webseiten)-Konfiguration bezieht sich auf die zu beschränken-de Webseite (Tpy:Einzeln) bzw. ein gesamtes Subnetz.";
			break;
		case 'S_IPv6_edit':
			str = "Wählen Sie ein Protokoll über das der zu sperrende Dienst erreichbar ist. In der Heimnetz-Konfiguration können Sie einzelne PCs definieren, für die diese Regel gilt, aber auch das gesamte Subnetz sperren. Eine Angabe des Portbereichs schränkt die Sperre ein. Bleibt die Angabe leer, gilt die Sperre für den gesamten Portbereich. Die Internet (Webseiten)-Konfiguration bezieht sich auf die zu beschränken-de Webseite (Tpy:Einzeln) bzw. ein gesamtes Subnetz.";
			break;
		case 'MAC':
			str = "Um Ihr Netz und Ihren lnternetzugang vor unbefugter Benutzung zu schützen, können Sie den Zugang zu Ihrem lokalen Netz sperren. Dies geschieht über eine Liste erlaubter Geräte mit einer eindeutigen Kennung, der sogenannten MAC-Adresse. Deaktivieren Sie die Freigabe einzelner Geräte in der Liste, haben diese keinen Zugriff auf das LAN. Bitte beachten Sie, dass damit auch kein Zugriff auf die HomeBox mehr möglich ist. Diese Funktion sollte nur von erfahrenen Nutzern aktiviert werden.";
			break;
		case 'MAC_add':
			str = "Um Ihr Netz und Ihren lnternetzugang vor unbefugter Benutzung zu schützen, können Sie den Zugang zu Ihrem lokalen Netz sperren. Dies geschieht über eine Liste erlaubter Geräte mit einer eindeutigen Kennung, der sogenannten MAC-Adresse. Deaktivieren Sie die Freigabe einzelner Geräte in der Liste, haben diese keinen Zugriff auf das LAN. Bitte beachten Sie, dass damit auch kein Zugriff auf die HomeBox mehr möglich ist. Diese Funktion sollte nur von erfahrenen Nutzern aktiviert werden.";
			break;
		case 'MAC_edit':
			str = "Um Ihr Netz und Ihren lnternetzugang vor unbefugter Benutzung zu schützen, können Sie den Zugang zu Ihrem lokalen Netz sperren. Dies geschieht über eine Liste erlaubter Geräte mit einer eindeutigen Kennung, der sogenannten MAC-Adresse. Deaktivieren Sie die Freigabe einzelner Geräte in der Liste, haben diese keinen Zugriff auf das LAN. Bitte beachten Sie, dass damit auch kein Zugriff auf die HomeBox mehr möglich ist. Diese Funktion sollte nur von erfahrenen Nutzern aktiviert werden.";
			break;
		case 'UPnP':
			str = "Universal Plug and Play (UPnP) stellt im Internet Gateway Device Protokoll (IGD) eine einfache Möglichkeit zur Verfügung, Ports zu öffnen und Anfragen aus dem Internet an einen Rechner weiterzuleiten. Konfigurieren Sie hier Ihre UPnP-Einstellungen (nur für erfahrene Nutzer).";
			break;
			
		/*System*/
		case 'Kennwort':
			str = "Vergeben Sie hier ein Kennwort für die Bedienoberfläche der o2 HomeBox, um Ihre Daten vor unbefugtem Zugriff zu schützen. Das Kennwort muss mind. 5 und kann max. 16 Zeichen enthalten. <p><b>Achtung:</b> Bei Verlust des Kennwortes muss an der o2 HomeBox ein Werksreset durchgeführt werden, um die Box wieder bedienen zu können. Sie sollten sich daher Ihr Kennwort sicher notieren.";
			break;
		case 'Kennwort_lock':
			str = "Falls Sie Ihre Bedienoberfläche durch ein Kennwort vor unbefugtem Zugriff geschützt haben, geben Sie jetzt Ihr Kennwort ein und klicken Sie auf \"Speichern\". <br>Beachten Sie bitte die korrekte Groß- und Kleinschreibung.";
			break;
		case 'Einstellungen':
			str = "Sie können die Konfiguration der o2 HomeBox in einer Datei speichern. Gehen die Einstellungen der o2 HomeBox aufgrund eines Defektes oder Zurücksetzens verloren, kann der ursprüngliche Zustand wieder über das Laden dieser Konfigurationsdatei hergestellt werden.";
			break;
		case 'Zurucksetzen':
			str = "Starten Sie die o2 HomeBox neu, falls das Gerät nicht reagiert oder Fehler auftreten. Durch <b>Zurücksetzen</b> werden die Internetzugangsdaten sowie Kunden-Einstellungen (z.B. Port Forwarding, DynDNS) gelöscht. Die Telefoniezugangsdaten bleiben erhalten. Oder setzen Sie nur Ihre DECT-Basisstation zurück. Alle Einstellungen bleiben erhalten.<br>Sie können Ihre aktuelle Konfiguration über den Menüpunkt <b>System > Einstellungen sichern</b> als Datei auf Ihrem Rechner abspeichern.";
			break;
		case 'Fernwartung':
			str = "Mit dem Servicezugang ermöglichen Sie dem o2 Kundenservice temporären Zugriff auf die Benutzeroberfläche des Gerätes, um die Funktionen zu prüfen. Klicken Sie nach Aufforderung des Servicemitarbeiters auf <b>Ausführen</b>. Teilen Sie ihm dann auf Nachfrage das im neuen Fenster angezeigte Kennwort mit. Der Zugang wird nach 60 Minuten automatisch getrennt. Über den <b>Trennen</b>-Button können Sie den Zugang jederzeit manuell unterbinden. <p>Sie können auch aus der Ferne Ihre o2 HomeBox über einen WAN-Zugriff bedienen. Weitere Informationen hierzu finden Sie im Benutzerhandbuch.";
			break;
		/*setup wizard*/
		case 'Startseite':
			str = "Ihre o2 HomeBox bietet Ihnen zugleich DSL- und Telefoniemöglichkeiten.";
			break;
		case 'S_Kennwort':
			str = "Vergeben Sie hier ein Kennwort für die Bedienoberfläche der o2 HomeBox, um Ihre Daten vor unbefugtem Zugriff zu schützen. Sie können das Kennwort auch später im Menü \"System\" vergeben oder ändern. Das Kennwort muss mindestens 5 und kann maximal 16 Zeichen enthalten. <p>Achtung: Notieren Sie sich das Kennwort sicher, z.B. auf Ihrer Auftragsbestätigung. Bei Verlust des Kennworts müssen Sie ein Werks-Reset durchführen, um das Gerät wieder bedienen zu können. ";
			break;
		case 'Zugangs-PIN':
			str = "";
			break;
		case 'Zugangsdaten':
			str = "Zur Einrichtung des Internetzugangs geben Sie die o2 Zugangsdaten ein, die Sie mit der Auftragsbestätigung erhalten haben. Ihr Benutzername besteht zu einem Teil aus Ihrer Telefonnummer:[IhreTelefonnummer] @xxx. Das Passwort wird aus Sicherheitsgründen nicht im Klartext angezeigt. Achten Sie auf die korrekte Schreibweise und auf Groß- und Kleinschreibung. Liegen Ihnen die Zugangsdaten nicht vor oder werden die eingegebenen Daten nicht akzeptiert, wenden Sie sich bitte an die Kundenbetreuung. ";
			break;
		case 'Zugangseinstellung':
			str = "Legen Sie hier die Art der Internet-Verbindung fest.";
			//str = "Wenn Sie einen zeitbasierten Tarif (z.B. Tarife mit Minutenabrechnung) nutzen, wählen Sie \"Zeitbasierte Verbindung\", um hohe Verbindungskosten durch eine dauerhafte Verbindung zu vermeiden. Um die Verbindung manuell herzustellen oder zu trennen muss immer diese Bedienoberfläche über den Browser geöffnet bleiben. ";
			break;
		case 'Zusammenfassung':
			str = "Ihre o2 Box bietet Ihnen zugleich DSL- und Telefoniemöglichkeiten. ";
			break;
			
		/*red*/
		case 'wait':
			str = "Änderungen werden übernommen.";
			break;
		case 'hint_BBI':
			str = "Bitte haben Sie einen Moment Geduld.<br>lhre Zugangs-PIN wird geprüft.";
			break;
		case 'fehler':
			str = "Es konnte keine Verbindung hergestellt warden.";
			break;
		case 'rebootMsg':
			str = "Die o2 HomeBox wird neu gestartet. Dies kann einige Minuten dauern.";
			break;
	
	}
	
	return str;
}


function WriteMenuHeader(id)
{
	var selected_red = '';
  
	var Survey_id = '';
	var Internet_id = '', Internet_AccessSetup_id='', Internet_Dyndns_id='';
	var Telephony_id = '', Telephony_Overview_id = '',Telephony_CallList_id = '', Telephony_PhoneBook_id = '', Telephony_Configuration_id = '', Telephony_Numbers_id = '';
	var HomeGroup_id ='', HomeGroup_Survey_id='', HomeGroup_LAN_id='', HomeGroup_WLAN_id='', HomeGroup_PortForwarding_id='', HomeGroup_USB_id='', MediaCenter_id = '';
	var Security_id = '', Security_IpFilter_id='', Security_MacAddressFilter_id='', Security_IgdUpnp_id='';
	var System_id = '', System_Password_id='', System_SaveSettings_id='', System_Back_id='',System_remoteMaintenance_id='';
	
	var SetupWizard = 0;
	var SetupWizard_id = "class=\"selected_SetupWizard\"";
	var SetupWizard_id1  = "class=\"selected_SetupWizard1\"";
	var Red = "class=\"selected_SetupWizard1_bottom\"";
	var SetupWizard_Home_id ='',SetupWizard_Password_id ='',SetupWizard_Access_PIN_id ='',SetupWizard_Access_id='',SetupWizard_Access_Setting_id='',SetupWizard_Summary_id='';

	var displayHome=1, displayPasswd=1, displayPIN=1, displayAccess=1, displaySummary=1; 
	
	switch (id){	
		case 'Survey': Survey_id = "class=\"selected\""; break;
		case 'Internet': Internet_id = "class=\"selected\""; Internet_AccessSetup_id = "class=\"selected_red\"";break;
		case 'Internet_Dyndns': Internet_id = "class=\"selected\""; Internet_Dyndns_id = "class=\"selected_red\"";break;
		
		case 'Telephony_Overwiew': Telephony_id = "class=\"selected\""; Telephony_Overview_id = "class=\"selected_red\"";break;
		case 'Telephony_CallList': Telephony_id = "class=\"selected\""; Telephony_CallList_id= "class=\"selected_red\"";break;
		case 'Telephony_PhoneBook': Telephony_id = "class=\"selected\""; Telephony_PhoneBook_id = "class=\"selected_red\"";break;
		case 'Telephony_Configuration': Telephony_id = "class=\"selected\""; Telephony_Configuration_id= "class=\"selected_red\"";break;
		case 'Telephony_Numbers': Telephony_id = "class=\"selected\""; Telephony_Numbers_id= "class=\"selected_red\"";break;
		
		case 'HomeGroup_Survey': HomeGroup_id = "class=\"selected\""; HomeGroup_Survey_id = "class=\"selected_red\"";break;
		case 'HomeGroup_LAN': HomeGroup_id = "class=\"selected\""; HomeGroup_LAN_id = "class=\"selected_red\"";break;
		case 'HomeGroup_WLAN': HomeGroup_id = "class=\"selected\""; HomeGroup_WLAN_id = "class=\"selected_red\"";break;
		case 'HomeGroup_PortForwarding': HomeGroup_id = "class=\"selected\""; HomeGroup_PortForwarding_id = "class=\"selected_red\"";break;
		case 'HomeGroup_USB': HomeGroup_id = "class=\"selected\""; HomeGroup_USB_id = "class=\"selected_red\"";break;
		case 'MediaCenter': HomeGroup_id = "class=\"selected\""; MediaCenter_id = "class=\"selected_red\"";break;
		
		case 'Security_IpFilter': Security_id = "class=\"selected\""; Security_IpFilter_id = "class=\"selected_red\"";break;
		case 'Security_MacAddressFilter': Security_id = "class=\"selected\""; Security_MacAddressFilter_id = "class=\"selected_red\"";break;
		case 'Security_IgdUpnp': Security_id = "class=\"selected\""; Security_IgdUpnp_id = "class=\"selected_red\"";break;
		
		case 'System_Password': System_id = "class=\"selected\""; System_Password_id = "class=\"selected_red\"";break;
		case 'System_SaveSettings': System_id = "class=\"selected\""; System_SaveSettings_id = "class=\"selected_red\"";break;
		case 'System_Back': System_id = "class=\"selected\""; System_Back_id = "class=\"selected_red\"";break;	
		case 'System_remoteMaintenance': System_id = "class=\"selected\""; System_remoteMaintenance_id = "class=\"selected_red\"";break;	
		
		case 'SetupWizard': break;  
//		case 'SetupWizard_Home':SetupWizard = 1; SetupWizard_Home_id = "class=\"selected_SetupWizard1\"";break;
//		case 'SetupWizard_Password':SetupWizard = 1; SetupWizard_Password_id = "class=\"selected_SetupWizard1\"";break;
//		case 'SetupWizard_Access_PIN':SetupWizard = 1; SetupWizard_Access_PIN_id = "class=\"selected_SetupWizard1\"";break;
//		case 'SetupWizard_Access':SetupWizard = 1; SetupWizard_Access_id = "class=\"selected_SetupWizard1\"";break;
//		case 'SetupWizard_Access_Setting':SetupWizard = 1; SetupWizard_Access_Setting_id = "class=\"selected_SetupWizard1\"";break;
//		case 'SetupWizard_Summary':SetupWizard = 1; SetupWizard_Summary_id = "class=\"selected_SetupWizard1\"";break;
		case 'SetupWizard_Home_Password_PIN_Access_Summary':
			SetupWizard = 1; displayHome=1, displayPasswd=1, displayPIN=1, displayAccess=1, displaySummary=1;	
			SetupWizard_Home_id = "class=\"selected_SetupWizard1\"";
			break;
		case 'SetupWizard_Home_Password_Access_Summary':
			SetupWizard = 1; displayHome=1, displayPasswd=1, displayPIN=0, displayAccess=1, displaySummary=1;	
			SetupWizard_Home_id = "class=\"selected_SetupWizard1\"";
			break;
		case 'SetupWizard_Home_Password_PIN_Summary':
			SetupWizard = 1; displayHome=1, displayPasswd=1, displayPIN=1, displayAccess=0, displaySummary=1;	
			SetupWizard_Home_id = "class=\"selected_SetupWizard1\"";
			break;
		case 'SetupWizard_Home_Access_Summary':
			SetupWizard = 1; displayHome=1, displayPasswd=1, displayPIN=0, displayAccess=1, displaySummary=1;	
			SetupWizard_Home_id = "class=\"selected_SetupWizard1\"";
			break;	
			
		case 'SetupWizard_Password_PIN_Access_Summary':
			SetupWizard = 1; displayHome=1, displayPasswd=1, displayPIN=1, displayAccess=1, displaySummary=1;	
			SetupWizard_Password_id = "class=\"selected_SetupWizard1\"";
			break;
		case 'SetupWizard_Password_Access_Summary':
			SetupWizard = 1; displayHome=1, displayPasswd=1, displayPIN=0, displayAccess=1, displaySummary=1;	
			SetupWizard_Password_id = "class=\"selected_SetupWizard1\"";
			break;
		case 'SetupWizard_Password_PIN_Summary':
			SetupWizard = 1; displayHome=1, displayPasswd=1, displayPIN=1, displayAccess=0, displaySummary=1;	
			SetupWizard_Password_id = "class=\"selected_SetupWizard1\"";
			break;
		case 'SetupWizard_Password_PIN_Access_Summary_locked':
			SetupWizard = 1; displayHome=1, displayPasswd=1, displayPIN=1, displayAccess=1, displaySummary=1;	
			break;
		case 'SetupWizard_Password_Access_Summary_locked':
			SetupWizard = 1; displayHome=1, displayPasswd=1, displayPIN=0, displayAccess=1, displaySummary=1;	
			break;
		case 'SetupWizard_Password_PIN_Summary_locked':
			SetupWizard = 1; displayHome=1, displayPasswd=1, displayPIN=1, displayAccess=0, displaySummary=1;	
			break;
		
		case 'SetupWizard_PIN_Access_Summary':
			SetupWizard = 1; displayHome=1, displayPasswd=1, displayPIN=1, displayAccess=1, displaySummary=1;	
			SetupWizard_Access_PIN_id = "class=\"selected_SetupWizard1\"";
			break;
		case 'SetupWizard_PIN_Summary':
			SetupWizard = 1; displayHome=1, displayPasswd=1, displayPIN=1, displayAccess=0, displaySummary=1;	
			SetupWizard_Access_PIN_id = "class=\"selected_SetupWizard1\"";
			break;
		
		case 'SetupWizard_Access_Summary':
			SetupWizard = 1; displayHome=1, displayPasswd=1, displayPIN=0, displayAccess=1, displaySummary=1;	
			SetupWizard_Access_id = "class=\"selected_SetupWizard1\"";
			break;
		case 'SetupWizard_AccessSetting_Summary':
			SetupWizard = 1; displayHome=1, displayPasswd=1, displayPIN=0, displayAccess=1, displaySummary=1;	
			SetupWizard_Access_Setting_id = "class=\"selected_SetupWizard1\"";
			break;	
		case 'SetupWizard_Summary':
			SetupWizard = 1; displayHome=1, displayPasswd=1, displayPIN=0, displayAccess=1, displaySummary=1;	
			SetupWizard_Summary_id = "class=\"selected_SetupWizard1\"";
			break;
		/* WenJuin@MSTC: Protect Lock, 20130305 */
		case 'ProtectLock':
			SetupWizard = 2;
			break;
		
//		case 'SetupWizard_PIN_Home':SetupWizard = 1; SetupWizard_Home_id = "class=\"selected_SetupWizard1\"";break;
//		case 'SetupWizard_PIN_Password':SetupWizard = 1; SetupWizard_Password_id = "class=\"selected_SetupWizard1\"";break;
//		case 'SetupWizard_Home':SetupWizard = 1; displayPIN = 0; SetupWizard_Home_id = "class=\"selected_SetupWizard1\"";break;
//		case 'SetupWizard_Password':SetupWizard = 1; displayPIN = 0; SetupWizard_Password_id = "class=\"selected_SetupWizard1\"";break;
//		case 'SetupWizard_Activated_Home':SetupWizard = 1; displayPIN = 0; displayPasswd=0; SetupWizard_Home_id = "class=\"selected_SetupWizard1\"";break;
//		case 'SetupWizard_Access_PIN':SetupWizard = 1; displayPasswd = 0; SetupWizard_Access_PIN_id = "class=\"selected_SetupWizard1\"";break;	
//		case 'SetupWizard_Access':SetupWizard = 1; displayPIN = 0; displayPasswd=0; SetupWizard_Access_id = "class=\"selected_SetupWizard1\""; break;
//		case 'SetupWizard_Access_Setting':SetupWizard = 1; displayPIN = 0; displayPasswd=0; SetupWizard_Access_Setting_id = "class=\"selected_SetupWizard1\""; break;
//		case 'SetupWizard_Summary':SetupWizard = 1; displayPIN = 0; displayPasswd=0; SetupWizard_Summary_id = "class=\"selected_SetupWizard1\""; break;														// 		case 'SetupWizard_Only_Summary': SetupWizard = 1; onlyDisplaySummary = 1; SetupWizard_Summary_id = "class=\"selected_SetupWizard1\""; break;
	}
 
	if(SetupWizard == 0)
	{
		/* Survey */
		document.writeln("<div><a href=\"Survey.html\" " + Survey_id + ">Übersicht</a></div>");
		
		/* Internet */
		document.writeln("<div><a href=\"Internet_Access.html\" " + Internet_id + ">Internet</a></div>");
		if(Internet_id != '')
		{
			document.writeln("<a href=\"Internet_Access.html\" " + Internet_AccessSetup_id+ ">Zugang einrichten</a>");
			document.writeln("<a href=\"dyndns.cmd?action=view\" " + Internet_Dyndns_id+ ">Dyndns</a>");
		}
	   
	   /* Telephony */
		document.writeln("<div><a href=\"Telephony_Overview.html\" " + Telephony_id + ">Telefonie</a></div>");
		if(Telephony_id != '')
		{
			document.writeln("<a href=\"Telephony_Overview.html\" " + Telephony_Overview_id+ ">Übersicht</a>");
			document.writeln("<a href=\"Telephony_CallList.html\" " + Telephony_CallList_id+ ">Anruflisten</a>");
			document.writeln("<a href=\"voiceservicephonebook.cmd?action=view\" " + Telephony_PhoneBook_id+ ">Telefonbuch</a>");
			document.writeln("<a href=\"Telephony_DECT.html\" " + Telephony_Configuration_id+ ">Konfiguration</a>");
			document.writeln("<a href=\"Telephony_InternalNumbers.html\" " + Telephony_Numbers_id+ ">Rufnummerplan</a>");
		} 
		
		/* HomeGroup */
		document.writeln("<div><a href=\"HomeGroup_Survey.html\" " + HomeGroup_id + ">Heimnetz</a></div>");
		if(HomeGroup_id != '')
		{
			document.writeln("<a href=\"HomeGroup_Survey.html\" " + HomeGroup_Survey_id + ">Übersicht</a>");
			document.writeln("<a href=\"HomeGroup_LAN_DHCP.html\" " + HomeGroup_LAN_id + ">LAN</a>");
			document.writeln("<a href=\"HomeGroup_WLAN_BaseStation.html\" " + HomeGroup_WLAN_id + ">WLAN</a>");	
			document.writeln("<a href=\"portForwarding.cmd?action=view\" " + HomeGroup_PortForwarding_id + ">Port Forwarding</a>");
			document.writeln("<a href=\"storageservicealicecfg.cmd?view\" " + HomeGroup_USB_id + ">USB</a>");
			document.writeln("<a href=\"HomeGroup_USB_DLNA.html\" " + MediaCenter_id + ">MediaCenter</a>");
		}
		
		/* Security */
		document.writeln("<div><a href=\"security_frwlRule.cmd?action=view\" " + Security_id + ">Sicherheit</a></div>");/*jessica*/
		if(Security_id != '')
		{
			document.writeln("<a href=\"security_frwlRule.cmd?action=view\" " + Security_IpFilter_id + ">IP Filter</a>");/*jessica*/
			document.writeln("<a href=\"scmacflt.cmd?action=view\" " + Security_MacAddressFilter_id + ">MAC-Adressfilter</a>");
			document.writeln("<a href=\"Security_IgdUpnp.html\" " + Security_IgdUpnp_id + ">IGD/UPnP</a>");
		}
		
		/* System */
		document.writeln("<div><a href=\"System_Password.html\" " + System_id + ">System</a></div>");
		if(System_id != '')
		{
			document.writeln("<a href=\"System_Password.html\" " +  System_Password_id + ">Kennwort</a>");
			document.writeln("<a href=\"System_SaveSettings.html\" " + System_SaveSettings_id + ">Einstellungen sichern</a>");
			document.writeln("<a href=\"System_Back.html\" " + System_Back_id + ">Zurücksetzen</a>");
			document.writeln("<a href=\"System_remoteMaintenance.html\" " + System_remoteMaintenance_id + ">Fernwartung</a>");
		}
		
		/* Setup Wizard */
		/*document.writeln("<a href=\"SetupWizard.html\" " + SetupWizard_id + ">Einrichtungsassistent</a>");*/
		/*For To2 specific requirements, __To2__, MitraStar Memphis Chung, 20121226*/
		document.writeln("<a href=\"Einrichtungsassistent.html\" " + SetupWizard_id + ">Einrichtungsassistent</a>");

		/* WenJuin@MSTC: add for Protect Lock ,20130307 */
		document.writeln("</br></br><a href=\"#\" onclick=\"ProtectLock()\" " + SetupWizard_id + ">Zugang schutzen</a>");
	}
	
	else if(SetupWizard == 1)
	{
		/* Setup Wizard */
		if(displayHome == 1)/* 1: display item*/
		{
			if(SetupWizard_Home_id != '')
				document.writeln("<div><a href=\"Einrichtungsassistent.html\" " + SetupWizard_Home_id + ">Startseite</a></div>");
			else
				document.writeln("<div class=\"blue\">Startseite</div>");
		}
		if(displayPasswd == 1)/* 1: display item*/
		{
			if(SetupWizard_Password_id != '')
				document.writeln("<div><a href=\"kennwort.html\" " + SetupWizard_Password_id + ">Kennwort</a></div>");
			else
				document.writeln("<div class=\"blue\">Kennwort</div>");
		}
		if(displayPIN == 1)/* 1: display item*/
		{
			if(SetupWizard_Access_PIN_id != '')
				document.writeln("<div><a href=\"zugangs-PIN.html\" " + SetupWizard_Access_PIN_id + ">Zugangs-PIN</a></div>");
			else
				document.writeln("<div class=\"blue\">Zugangs-PIN</div>");
		}
		if(displayAccess == 1)/* 1: display item*/
		{
			if(SetupWizard_Access_id != '')
				document.writeln("<div><a href=\"zugangsdaten.html\" " + SetupWizard_Access_id + ">Zugangsdaten</a></div>");
			else
				document.writeln("<div class=\"blue\">Zugangsdaten</div>");
		
			if(SetupWizard_Access_Setting_id != '')
				document.writeln("<div><a href=\"zugangseinstellung.html\" " + SetupWizard_Access_Setting_id + ">Zugangseinstellung</a></div>");
			else
				document.writeln("<div class=\"blue\">Zugangseinstellung</div>");
		}
		if(displaySummary == 1)/* 1: display item*/
		{
			if(SetupWizard_Summary_id != '')
				document.writeln("<div><a href=\"Einrichtungsassistent.html\" " + SetupWizard_Summary_id + ">Zusammenfassung</a></div>");
			else
				document.writeln("<div class=\"blue\">Zusammenfassung</div>");
		}	
		/*For To2 specific requirements, __To2__, MitraStar Memphis Chung, 20121226*/		
		/*document.writeln("<div><a href=\"SetupWizard.html\" " + Red + "></a></div>");*/
		document.writeln("<div><a href=\"Einrichtungsassistent.html\" " + Red + "></a></div>");
		
	}

	else
	{
		/* WenJuin@MSTC: Protect Lock, 20130305 */
		/* Survey */
		document.writeln("<div class=\"blue\">bersicht</div>");
		/* Internet */
		document.writeln("<div class=\"blue\">Internet</div>");
	   	/* Telephony */
		document.writeln("<div class=\"blue\">Telefonie</div>");
		/* HomeGroup */
		document.writeln("<div class=\"blue\">Heimnetz</div>");
		/* Security */
		document.writeln("<div class=\"blue\">Sicherheit</div>"); 
		/* System */
		document.writeln("<div class=\"blue\">System</div>");
		/* Setup Wizard */
		document.writeln("<div><a " + SetupWizard_id + ">Einrichtungsassistent</a></div>");
	}
}
