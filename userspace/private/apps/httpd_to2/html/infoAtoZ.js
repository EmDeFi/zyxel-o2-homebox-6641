// JavaScript Document
/*information A-Z*/
function selectLetter(letter)
{
		var i;
		for(i=0;i<26;i++)
		{
			var istr = i.toString();
			document.getElementById(istr).className = 'style1'; 
		}
		document.getElementById(letter).className = 'styleSelected'; 
}
function selectEntry(letter, number)
{
		var i;
		for(i=0;i<number;i++)
		{
			var istr = i.toString();
			document.getElementById(istr).style.color = '#005896'; 
			document.getElementById(istr).style.fontWeight = 'normal'; 
		}
		document.getElementById(letter).style.color = '#19224F'; 
		document.getElementById(letter).style.fontWeight = 'bold'; 
}
function showInfo(word)
{	
	var element = document.getElementById('description');
	var str = "";
	switch(word)
	{
		/*A*/
		case 'ADSL': str = getDescription('ADSL');break;
		case 'Analoge_Endgeräte': str = getDescription('Analoge_Endgeräte');break;
		case 'Angeschlossene_USB_Drucker': str = getDescription('Angeschlossene_USB_Drucker');break;
		case 'Angeschlossene_USB_Massenspeicher': str = getDescription('Angeschlossene_USB_Massenspeicher');break;
		case 'Art_der_Internetverbindunge': str = getDescription('Art_der_Internetverbindunge');break;
		/*B*/
		case 'Browser': str = getDescription('Browser');break;
		/*C*/
		case 'CAT_iq': str = getDescription('CAT_iq');break;
		/*D*/
		case 'DECT': str = getDescription('DECT');break;
		case 'DHCP_Server': str = getDescription('DHCP_Server');break;
		case 'DLNA': str = getDescription('DLNA');break;
		case 'DNS': str = getDescription('DNS');break;
		case 'DSL_Modem': str = getDescription('DSL_Modem');break;		
		case 'DynDNS': str = getDescription('DynDNS');break;
		/*E*/
		case 'Einrichtungsassistent': str = getDescription('Einrichtungsassistent');break;
		case 'Ethernet': str = getDescription('Ethernet');break;
		/*F*/
		case 'Firewall': str = getDescription('Firewall');break;
		case 'Firmware': str = getDescription('Firmware');break;
		case 'Funkkanal': str = getDescription('Funkkanal');break;
		/*G*/
		case 'Gateway': str = getDescription('Gateway');break;
		case 'GUI': str = getDescription('GUI');break;
		/*H*/
		case 'Halten_eines_Gesprächs': str = getDescription('Halten_eines_Gesprächs');break;
		case 'Handbuch': str = getDescription('Handbuch');break;
		case 'Heimnetz': str = getDescription('Heimnetz');break;
		/*I*/
		case 'IAD': str = getDescription('IAD');break;
		case 'IP_Adresse': str = getDescription('IP_Adresse');break;
		case 'IPv4': str = getDescription('IPv4');break;
		case 'IPv6': str = getDescription('IPv6');break;
		case 'ISDN': str = getDescription('ISDN');break;
		/*J*/
		/*K*/
		case 'Kennwort_Schutz': str = getDescription('Kennwort_Schutz');break;
		case 'Kennwort_Schutz_konfigurieren': str = getDescription('Kennwort_Schutz_konfigurieren');break;
		/*L*/
		case 'LAN': str = getDescription('LAN');break;
		case 'LAN_Anschlüsse': str = getDescription('LAN_Anschlüsse');break;
		case 'LED_Anzeige': str = getDescription('LED_Anzeige');break;
		/*M*/
		case 'MAC_Adresse': str = getDescription('MAC_Adresse');break;
		case 'Makeln': str = getDescription('Makeln');break;
		case 'MediaCenter': str = getDescription('MediaCenter');break;
		case 'MSN': str = getDescription('MSN');break;
		/*N*/
		case 'Name_der_WLAN_Basisstation': str = getDescription('Name_der_WLAN_Basisstation');break;
		case 'NAT': str = getDescription('NAT');break;
		case 'Netzteil': str = getDescription('Netzteil');break;
		case 'Netzwerk': str = getDescription('Netzwerk');break;
		case 'Neustart': str = getDescription('Neustart');break;
		/*O*/
		case 'Offline': str = getDescription('Offline');break;
		case 'Online': str = getDescription('Online');break;
		/*P*/
		case 'PBX': str = getDescription('PBX');break;
		case 'Port': str = getDescription('Port');break;
		case 'Port_Forwarding': str = getDescription('Port_Forwarding');break;
		case 'PPPoE_Pass_Through': str = getDescription('PPPoE_Pass_Through');break;
		/*Q*/
		/*R*/
		case 'Reset': str = getDescription('Reset');break;
		case 'Router': str = getDescription('Router');break;
		case 'Rückfrage': str = getDescription('Rückfrage');break;
		/*S*/	
		case 'Sendeleistung': str = getDescription('Sendeleistung');break;
		case 'SSID': str = getDescription('SSID');break;
		case 'Status_Freischaltung': str = getDescription('Status_Freischaltung');break;
		case 'Subnetzmaske': str = getDescription('Subnetzmaske');break;
		/*T*/
		case 'TAE': str = getDescription('TAE');break;
		case 'TCP_IP': str = getDescription('TCP_IP');break;
		case 'Telefonanlage': str = getDescription('Telefonanlage');break;
		/*U*/
		case 'USB': str = getDescription('USB');break;
		/*V*/
		case 'VDSL': str = getDescription('VDSL');break;
		case 'Verschlüsselung': str = getDescription('Verschlüsselung');break;
		case 'VoIP': str = getDescription('VoIP');break;
		/*W*/
		case 'WEP': str = getDescription('WEP');break;
		case 'WLAN': str = getDescription('WLAN');break;
		case 'WLAN_aktivieren': str = getDescription('WLAN_aktivieren');break;
		case 'WLAN_Basisstation_verschlüsseln': str = getDescription('WLAN_Basisstation_verschlüsseln');break;
		case 'WLAN_Modus': str = getDescription('WLAN_Modus');break;
		case 'WPA': str = getDescription('WPA');break;
		case 'WPS': str = getDescription('WPS');break;
		/*X*/
		/*Y*/
		/*Z*/
		case 'Zugangsdaten': str = getDescription('Zugangsdaten');break;
		case 'Zurücksetzen': str = getDescription('Zurücksetzen');break;
		
		default : str = getDescription('default');break;
	}
	if(element)
	{
		element.innerHTML = str;
	}
}

function getDescription(word)
{
	var str = "";
	
	switch(word)
	{
		/*A*/
		case 'ADSL':
			str = "ADSL (Asymmetric Digital Subscriber Line) bezeichnet eine asymmetrische DSL Variante. Das bedeutet, dass die Downloadrate und die Uploadrate unterschiedlich hoch sind, was für die private Nutzung des Internets von Vorteil ist, da in der Regel wesentlich mehr Daten aus dem Internet geladen werden, als umgekehrt. ADSL2+ bietet in Deutschland bis zu 16 Mbit/s Downloadrate.";
			break;
		case 'Analoge_Endgeräte':
			str = "Dazu zählen Geräte wie Telefone, Faxe (der Gruppe 3) oder Anrufbeantworter zum Anschluss an die analogen Anschlüsse (TAE) Ihrer HomeBox. ISDN-Endgeräte können hier nicht angeschlossen werden.";
			break;	
		case 'Angeschlossene_USB_Drucker':
			str = "Zeitgleich können bis zu vier USB-Drucker angeschlossen werden. Die Einrichtung des Druckertreibers erfolgt über Windows. Bitte folgen Sie der Anleitung im Handbuch.";
			break;	
		case 'Angeschlossene_USB_Massenspeicher':
			str = "Um einen am USB-Port Ihrer HomeBox angeschlossenen Massenspeicher am Computer anzuzeigen, können Sie, je nach Betriebssystem Ihres Computers, verschiedene Möglichkeiten nutzen: Unter Windows geben Sie im Dateiexplorer folgende Adresszeile ein: \\\\o2.box . Unter MAC OS geben Sie bitte im Finder unter \"Gehe Zu -> Mit Server verbinden\" folgende Adresse ein: smb:\\\\o2.box. Die Beschreibung von alternativen Methoden finden Sie im Benutzerhandbuch auf der beigefügten CD. ";
			break;
		case 'Art_der_Internetverbindunge':
			str = "Für die Internetverbindung kann eine Flatrate oder bei bestimmten o2- Produkten ein zeitbasierender Tarif genutzt werden. Bei einem zeitbasierenden Tarif sollte die Verbindung manuell oder automatisch bei Inaktivität nach einer vorgegebenen Zeit getrennt werden, um unnötige Verbindungskosten zu vermeiden.";
			break;	
		
		/*B*/
		case 'Browser':
			str = "Der Begriff Browser ist durch den Erfolg des Internets zum Synonym für ein Programm geworden, mit dem Webseiten dargestellt werden. Diese Seiten können auf einem lokalen Datenträger oder irgendwo im Internet abgelegt sein. Web-Browser unterstützen in der Regel nicht nur die Anzeige von HTML-Seiten, sondern auch andere Dienste im Internet wie die Dateiübertragung per FTP, Chat, E-Mail oder Newsgroups.";
			break;
			
		/*C*/
		case 'CAT_iq':
			str = "CAT-iq (Cordless Advanced Technology - internet and quality) ist eine Weiterentwicklung des DECT-Standards zur kabellosen Datenübertragung. CAT-iq vereint Sprachübertragung, Breitbandanwendungen und Multimedia. So ermöglicht CAT-iq Telefonie in HiFi-Qualität HD-voice.";
			break;
			
		/*D*/
		case 'DECT':
			str = "DECT (Digital Enhanced Cordless Telecommunications) ist eine Kurzstrecken-Funktechnik zur schnurlosen Telefonie im Heimbereich. Der Frequenzbereich von DECT ist geschützt und liegt unterhalb von 2,45 GHz. Dadurch werden Störungen durch WLAN, Bluetooth oder Ähnlichem vermieden. Ihre o2 HomeBox beinhaltet eine DECT-Basisstation und ermöglich Ihnen damit die Anmeldung und Nutzung von DECT- Handsets.";
			break;
		case 'DHCP_Server':
			str = "Das Dynamic Host Configuration Protocol (DHCP) dient dazu, Computern im Netzwerk automatisch eine nicht benutzte IP-Adresse aus einem Adressbereich zuzuordnen. Auch andere Informationen, wie der Name der Domain und das Gateway können dem Client übergeben werden. Durch einen DHCP-Server ist die vollautomatische Einbindung eines neuen Computers in ein bestehendes Netzwerk ohne weitere Konfiguration möglich. Am Computer muss im Normalfall lediglich der automatische Bezug der IP-Adresse eingestellt sein. Die Vergabe der IP-Adressen erfolgt an alle Netzwerkgeräte, die per LAN oder WLAN angeschlossen sind.";
			break;	
		case 'DLNA':
			str = "Digital Living Network Alliance - ist eine Vereinigung von Herstellern u. a. der Unterhaltungselektronik und des Mobilfunks. Ziel ist es, das Zusammenspiel von Geräten verschiedener Hersteller sicherzustellen und beispielsweise die Nutzung von Medien im Heimnetz zu erleichern.";
			break;
		case 'DNS':
			str = "DNS steht für \"Domain Name System\" – Ein DNS-Server enthält eine Datenbank mit Zuordnungen von IP-Adressen und Domain-Namen. Jeder Computer und jede Webseite im Internet verfügt über eine eindeutige  IP-Adresse, die aus vier Zahlen nach dem Muster  \"82.113.101.129\" besteht. Damit Sie sich den Zahlencode einer Webseite nicht merken müssen, ist der IP-Adresse in der Regel eine Bezeichnung zugeordnet, die sogenannte Domain. Wenn Sie also in Verbindung mit dem Protokoll http und dem Internetdienst www die Adresse http://www.o2.de aufrufen, wird über das DNS eine IP-Adresse angesteuert.";
			break;
		case 'DSL_Modem':
			str = "Ein Gerät, mit dem sich der Computer per DSL in das Internet einwählt. Die o2 HomeBox hingegen verfügt neben der einfachen Einwahlmöglichkeit ins Internet zusätzlich über Telefoniefunktionen.";
			break;
		case 'DynDNS':
			str = "DynDNS (Dynamic Domain Name Server) ist ein Internetdienst, der es der o2 HomeBox ermöglicht, immer unter einem feststehenden Namen aus dem Internet erreichbar zu sein, obwohl sich die öffentliche IP-Adresse des IAD mit jeder Interneteinwahl ändert.";
			break;
			
		/*E*/
		case 'Einrichtungsassistent':
			str = "Der Einrichtungsassistent der o2 HomeBox erleichtern die Ersteinrichtung und führt durch die notwendigen Eingaben wie z. B. Zugangs-PIN. Nach einem Umzug bewirkt das Starten des Einrichtungsasststenten das Erkennen des neuen Zugangsnetzes, um die Nutzerdaten neu einzugeben.";
			break;
		case 'Ethernet':
			str = "Ihre o2 HomeBox stellt den Internet-Zugang über die Netze ADSL, VDSL sowie Ethernet bereit. Hierzu wird die Gerätebuchse DSL/ETH benutzt.";
			break;
			
		/*F*/
		case 'Firewall':
			str = "Firewall ist eine Sicherheitsfunktion zum Schutz eines Computers oder Netzwerkes. Die Firewall überwacht den Datenverkehr zwischen Computer/Netzwerk und Internet und schützt so vor unerlaubten Zugriffen.";
			break;
		case 'Firmware':
			str = "Unter Firmware versteht man die Betriebssoftware einer Hardwarekomponente (z. B. DSL-Modem).";
			break;
		case 'Funkkanal':
			str = "Der Funkkanal bestimmt, über welche Frequenz die Signale Ihrer o2 HomeBox gesendet werden. Befinden sich mehrere WLAN-Basisstationen in der Umgebung, die über die gleiche Frequenz senden, kann über das Menü \"Heimnetz/WLAN\" ein anderer Funkkanal ausgewählt werden.";
			break;
		
		/*G*/
		case 'Gateway':
			str = "Ein Gateway bezeichnet den Übergang zwischen zwei Netzen. Ein Gateway-Rechner ist ein spezieller Rechner, der an beide Netze angeschlossen ist. Dabei werden u. U. unterschiedliche Protokolle umgesetzt. So werden z. B. zwei LANs durch einen Gateway verbunden. Alle Nachrichten von einem in ein anderes Netzwerk müssen den Gateway-Rechner passieren.";
			break;
		case 'GUI':
			str = "GUI (Graphical User Interface) bezeichnet eine grafische Benutzeroberfläche, die eine einfache Interaktion zwischen Mensch und Maschine ermöglicht. Das GUI Ihrer o2 HomeBox ist über die Browser-Adresse o2.box bzw. 192.168.1.1 zu erreichen.";
			break;
		
		/*H*/
		case 'Halten_eines_Gesprächs':
			str = "Durch das Leistungsmerkmal Halten kann ein Teilnehmer eine bestehende Verbindung in einen Wartezustand bringen. Der Anrufer erhält in diesem Zustand ein Signal.";
			break;
		case 'Handbuch':
			str = "Weitere ausführliche Informationen finden Sie im Handbuch auf der mitgelieferten o2 HomeBox-CD. Im Internet unter www.o2.de, im Downloadbereich der Online-Hilfe, steht Ihnen immer eine aktuelle Ausgabe des Handbuches zur Verfügung.";
			break;
		case 'Heimnetz':
			str = "Heimnetze sind lokale Netze, die für die Vernetzung von Computern und deren Peripheriegeräte sorgen. Darüber hinaus können über Heimnetze auch die Geräte der Unterhaltungselektronik wie Fernseher, Mediaboxen, etc. gesteuert werden. Ein Heimnetz kann drahtgebunden und/oder drahtlos aufgebaut sein, wobei verschiedenste Technologien wie WLAN, USB oder kabelgebundenes Ethernet zusammenarbeiten.";
			break;
			
		/*I*/
		case 'IAD':
			str = "IAD (Integrated Access Device - integriertes Zugangsgerät) bezeichnet ein Modem für ADSL bzw VDSL mit zusätzlichen Funktionalitäten wie zum Beispiel Telefonie.";
			break;
		case 'IP_Adresse':
			str = "In einem Netzwerk, das mit dem TCP/IP-Protokoll arbeitet, ist jeder Computer über eine nummerische Adresse identifizierbar. Eine IP-Adresse besteht aus vier durch Punkte getrennten Zahlen, die jeweils einen Wert von 0  bis 255 annehmen können (normalerweise werden 0, 1 und 255 nicht vergeben, da sie für besondere Zwecke reserviert sind) . Eine typische IP-Adresse ist \"82.113.101.129\". Die Punkte haben die Aufgabe, über- und untergeordnete Netze  anzusprechen. So wie zu einer Telefonnummer im Telefonnetz eine Landes- und Ortsvorwahl und die Teilnehmerrufnummer gehören, gibt es auch im Internet eine Vorwahl (die Netzwerknummer) und eine Durchwahl (die Nummer des Computers). Der erste Teil einer IP-Adresse ist die Netzwerknummer, der zweite Teil die Nummer des Computers. Die Grenze zwischen Netzwerk- und Computernummer wird durch die Subnetzmaske bestimmt.";
			break;
		case 'IPv4':
			str = "IPv4 (Internet Protocol Version 4) bezeichnet die vierte Version des Internet Protocols (IP) und die erste, die weltweit eingesetzt wurde. IPv4 bildet eine der wichtigsten technischen Grundlagen für das Internet.";
			break;
		case 'IPv6':
			str = "IPv6 (Internet Protocol Version 6) ist die sechste Version des Internet Protocols und der Nachfolger von IPv4. IPv6 wurde gegenüber IPv4 stark verbessert. Die deutlichste Verbesserung ist die Anzahl der verfügbaren Adressen. Mit der starken Zunahme von internetfähigen Geräten, wie Smartphones oder Tablet-Computern, wurden die IPv4-Adressen immer knapper. Mit IPv6 sind nun statt 4,3 Milliarden Adressen, 340 Sextillionen Adressen verfügbar.";
			break;
		case 'ISDN':
			str = "ISDN (Integrated Services Digital Network) bezeichnet einen internationalen Standard für ein digitales Telekommunikationsnetz. Die Telekommunikationsdienste Sprache und Daten werden zusammengefasst und digital übertragen. Durch ISDN ist es möglich, mehrere Endgeräte über einen Bus parallel zu nutzen.";
			break;
		
		/*J*/
		/*K*/
		case 'Kennwort_Schutz':
			str = "Der Zugangsschutz verhindert unbefugten Zugriff auf die Konfigurationsoberfläche der o2 HomeBox. Ändern Sie das Kennwort möglichst sofort nach Erhalt der o2 HomeBox. Das Kennwort muss mindestens aus 5 alphanumerischen Zeichen (a-z 1-9) bestehen.";
			break;
		case 'Kennwort_Schutz_konfigurieren':
			str = "Im Menü \"System/Kennwort\" können Sie das Kennwort zur Bedienoberfläche der o2 HomeBox ändern. Tun Sie dies in jedem Fall nach dem ersten Anschließen der HomeBox, um unbefugten Zugriff auf die HomeBox zu vermeiden. Notieren Sie sich Ihr neues Kennwort. Sollten Sie Ihr Kennwort vergessen haben, können Sie nur durch ein Zurücksetzen der o2 HomeBox auf die Werkseinstellungen die Bedienoberfläche erreichen. Dabei gehen alle Einstellungen verloren. Drücken Sie dazu den RESET-Taster am Gerät ca. 10 Sekunden lang.";
			break;
			
		/*L*/
		case 'LAN':
			str = "LAN bedeutet Local Area Network (deutsch: lokales Netzwerk). Diese Netzwerke beschränken sich meist auf eine kleine Grundfläche (ein Gebäude oder gar ein Stockwerk). Oft wird dieses Netzwerk auch aus sehr unterschiedlichen Rechnersystemen gebildet.";
			break;
		case 'LAN_Anschlüsse':
			str = "Im Menü Heimnetz sehen Sie eine Liste aller an das lokale Netzwerk der o2 HomeBox angeschlossenen Netzwerkgeräte mit ihrem Namen (sofern dieser übermittelt werden konnte), der zugewiesenen IP-Adresse und der MAC-Adresse des Gerätes. Kann der Name nicht übermittelt werden, wird \"Name nicht verfügbar\" angezeigt.";
			break;
		case 'LED_Anzeige':
			str = "Leuchtdioden (Light Emitting Diodes), die als Signallampen Auskunft über den Status eines Gerätes geben. Die Kurzanleitung und das Handbuch beschreiben die Funktionen im Detail.";
			break;
		
		/*M*/
		case 'MAC_Adresse':
			str = "Die MAC-Adresse (Media Access Control) ist die physikalische Adresse einer Netzwerkkomponente. Die MAC-Adresse ist weltweit einmalig und der Netzwerkkomponente fest zugewiesen.";
			break;
		case 'Makeln':
			str = "Makeln ist ein Leistungsmerkmal, bei dem ein Teilnehmer abwechselnd zwischen zwei aktiven Verbindungen hin- und herschalten kann. Es können nur die Teilnehmer der gerade bestehenden aktiven Verbindung miteinander sprechen. Der wartende Teilnehmer wird in der Vermittlungsstelle oder Telefonanlage bis zur Wiederaufnahme des Gesprächs gehalten.";
			break;
		case 'MediaCenter':
			str = "Das MediaCenter stellt einen Streamingserver für Multimedia-Anwendungen bereit. Mit der Aktivierung des MediaCenters werden UPnP-fähigen (Universal Plug & Play) PCs und Netzwerkgeräten (z. B. Netzwerk-Musikplayer) die an der o2 HomeBox angeschlossenen Medien bereitgestellt. Schließen Sie dazu einen USB-Massenspeicher mit Musik, Bildern oder Videos an die o2 HomeBox an. Im Menü des sich im lokalen Netzwerk befindlichen Abspielgeräts (Streaming Client) wählen Sie das MediaCenter als Quelle aus und nutzen so die Inhalte des angeschlossenen USB-Massenspeichers.";
			break;
		case 'MSN':
			str = "Die MSN (Multiple Subscriber Number) ist, bei einem ISDN-Anschluss, die Rufnummer ohne Vorwahl. Sie ermöglicht, dass ein ISDN-Anschluss unter mehreren Rufnummern zu erreichen ist. Jedem ISDN-Endgerät kann somit eine individuelle Rufnummer oder mehrere Rufnummern zugeteilt werden. In Deutschland ist die Anzahl der MSNs auf zehn begrenzt.";
			break;
		
		/*N*/
		case 'Name_der_WLAN_Basisstation':
			str = "Im Menü \"Heimnetz/WLAN\" können Sie den Namen der WLAN-Basisstation vergeben. Dieser wird auch als SSID (Service Set Identifier) bezeichnet. Der Name darf aus bis zu 32 alphanumerischen Zeichen bestehen und sollte sich möglichst von den Namen der WLAN-Basisstationen in Ihrer Umgebung unterscheiden. Da die o2 HomeBox 2 unabhängige WLAN-Basisstationen enthält, haben Sie die Option 2 SSIDs zu vergeben. Wenn Sie den Namen verbergen, ist dieser in der Anzeige der Drahtlosnetzwerke nicht mehr zu sehen. Beachten Sie, dass nicht verhindert werden kann, dass bestimmte Software diesen Namen trotzdem auslesen kann. Eine Sicherung ist in jedem Fall notwendig. Die MAC-Adresse Ihrer WLAN-Basisstation wird benötigt, falls Sie die Reichweite Ihres WLAN-Netzes mit Hilfe von WLAN-Repeatern erhöhen möchten.";
			break;
		case 'NAT':
			str = "NAT (Network Address Translation) ist ein in Ihrer o2 HomeBox eingesetztes Verfahren, um das lokale Netzwerk mit dem Internet zu verbinden. Für das Internet steht in der Regel nur eine öffentliche IP-Adresse zur Verfügung, während im lokalen Netzwerk jede Station eine private IP-Adresse hat. Diese privaten Adressen besitzen in öffentlichen Netzen keine Gültigkeit. Damit trotzdem alle Computer mit privater IP-Adresse Zugang zum Internet haben, ersetzt Ihre o2 HomeBox in allen ausgehenden Datenpaketen die IP-Adressen der angeschlossenen Computer durch seine eigene, öffentliche IP-Adresse. Damit die eingehenden Datenpakete dem richtigen Ziel zugeordnet werden, speichert die o2 HomeBox die aktuellen Verbindungen in einer Tabelle.";
			break;
		case 'Netzteil':
			str = "Das Netzteil ist für die Spannungsversorgung einer Hardwarekomponente zuständig. Bitte benutzen Sie nur das der o2 HomeBox beiliegende Netzteil.";
			break;
		case 'Netzwerk':
			str = "Unter Netzwerk versteht man mehrere miteinander verbundene Computer. Hierbei können die Computer gemeinsam auf einzelne Hardwarekomponenten (z. B. Drucker) zugreifen.";
			break;
		case 'Neustart':
			str = "Im Menü \"System/Zurücksetzen\" können Sie die o2 HomeBox neu starten. Alle Einstellungen bleiben erhalten, alle laufenden Gespräche und Internetverbindungen werden unterbrochen.";
			break;
			
		/*O*/
		case 'Offline':
			str = "Offline bezeichnet eine inaktive Verbindung.";
			break;
		case 'Online':
			str = "Online bezeichnet eine aktive Verbindung.";
			break;
		
		/*P*/
		case 'PBX':
			str = "PBX-Mode (Private Branch Exchange) - Nutzung der internen Telefonanlage der o2 HomeBox für das interne Telefonnetz, auch TK-Anlage genannt. Sie haben hier die Möglichkeit verschiedene analoge- ISDN- bzw. DECT- Geräte an die o2 HomeBox anzuschliessen, zu administrieren sowie viele weitere Optionen.";
			break;
		case 'Port':
			str = "Port ist die Bezeichnung für einen Anschluss bzw. eine Schnittstelle. Man unterscheidet zwischen physikalischem Port (z. B. Anschluss für Drucker) und logischem Port (z. B. Port 80 für die Internetverbindung).";
			break;
		case 'Port_Forwarding':
			str = "Durch Port Forwarding (auch Portweiterleitung genannt) wird es Computern innerhalb eines lokalen Netzes möglich, auch außerhalb dieses Netzes zu arbeiten und damit auch für andere Computer erreichbar zu sein. Dies ist beispielsweise wichtig, um Daten über das Internet auszutauschen oder bei Spielen im Netz mit anderen Computern zu kommunizieren.";
			break;
		case 'PPPoE_Pass_Through':
			str = "PPPoE (Point-to-Point-Protocol over Ethernet) Pass Through ermöglicht den transparenten Betrieb Ihres Routers und den Aufbau einer zusätzlichen Internetverbindung aus dem Heimnetz. Diese Funktion steht nicht in allen Anschlussgebieten zur Verfügung.";
			break;
		
		/*Q*/
		/*R*/
		case 'Reset':
			str = "Reset ist ein Vorgang, der ein elektronisches Gerät in einen vordefinierten Zustand zurückversetzt.";
			break;
		case 'Router':
			str = "Router sind Geräte, die verschiedene Netzwerktechniken beherrschen und Daten über weite Strecken versenden können. Ein Router teilt ein Netzwerk in Segmente auf, um die Übertragungsmenge zu reduzieren. Gleichzeitig bietet er Sicherheitsfunktionen zwischen den verschiedenen Übertragungsorten. Er arbeitet mit definierten Protokollen wie TCP/IP und trifft Entscheidungen bei der Optimierung der Datenübertragung und der Wahl des Weges.";
			break;
		case 'Rückfrage':
			str = "Das Leistungsmerkmal Rückfrage ermöglicht es, zu einer bestehenden Verbindung vom selben Endgerät eine zweite Verbindung aufzubauen oder ein anklopfendes Gespräch entgegenzunehmen. Dazu muss die bestehende Verbindung vorher mit Halten in einen Wartezustand gebracht worden sein.";
			break;
			
		/*S*/
		case 'Sendeleistung':
			str = "Unter Sendeleistung versteht man die Stärke des Funksignals. Befindet sich die WLAN-Basisstation relativ nahe zum Computer, kann die Sendeleistung verringert werden. Die Übertragung verschlechtert sich hierbei nicht.";
			break;
		case 'SSID':
			str = "Die SSID (Service Set Identifier) bezeichnet den Namen der WLAN-Basisstation. Der Name darf aus bis zu 32 alphanumerischen Zeichen bestehen und sollte sich möglichst von den Namen anderer WLAN Basisstationen in der Umgebung unterscheiden. Die SSID kann aus Sicherheitsgründen verborgen werden, damit die Basisstation in Drahtlosnetzwerken nicht mehr angezeigt wird. Iher o2 HomeBox ist im Auslieferungszustand mit 1 SSID für die 2 vorhandenen WLAN-Basisstationen ausgestattet, Sie können für jede dieser Basisstationen eine eigene SSID vergeben.";
			break;
		case 'Status_Freischaltung':
			str = "Um die o2 HomeBox nutzen zu können, muss das Gerät freigeschaltet und konfiguriert werden. Sie haben eine Auftragsbestätigung erhalten, in der abhängig von Ihrem Produkt ggf. eine 10-stellige Zugangs-PIN enthalten ist. Diese können Sie entweder per Telefon oder über den Einrichtungsassistenten der Konfigurationsoberfläche eingeben. Bitte beachten Sie, dass nach korrekter Eingabe die o2 HomeBox automatisch für die Telefonie konfiguriert und, falls nötig, mit einer neuen Software versehen wird. Dies kann bis zu 10 Minuten dauern. Trennen Sie während dieser Zeit die o2 HomeBox nicht vom Stromnetz. Wenn keine LED mehr rot leuchtet, wurde der Prozess erfolgreich abgeschlossen.";
			break;
		case 'Subnetzmaske':
			str = "Eine Subnetzmaske bestimmt, zu welchem Teilnetz (Subnetz) eine IP-Adresse gehört. Eine IP-Adresse hat zwei Bestandteile: die Netzwerkadresse und die Computer-Adresse. Mit der Subnetzmaske kann der Computer intern die IP-Adresse in Netzwerknummer und Computernummer aufteilen.";
			break;
			
		/*T*/
		case 'TAE':
			str = "TAE steht für Telekommunikations-Anschluss-Einheit und bezeichnet eine Anschlussdose für analoge Telefonanschlüsse und ISDN-Anschlüsse. Umgangssprachlich wird sie als Telefondose bezeichnet. In der Regel hat die TAE drei Steckplätze mit der Kodierung NFN. Die Kodierung N für Steckplatz 1 und 3 steht für \"Nicht Fernsprechen\". Hier werden z. B. Anrufbeantworter, FAX oder Modem angeschlossen. Die Kodierung F für Steckplatz 2 bedeutet Fernsprechen. Hier wird das analoge Telefon oder der Splitter angeschlossen.";
			break;
		case 'TCP_IP':
			str = "(Transmission Control Protocol/Internet Protocol) Dieses Protokoll wird zur Datenübertragung in lokalen Netzwerken und im Internet verwendet.";
			break;
		case 'Telefonanlage':
			str = "Auch TK-Anlage genannt, ist eine Vermittlungseinrichtung zum Anschluss und zur Administration mehrerer Endgeräte (wie z. B. Telefone, Fax, Anrufbeantworter und DECT-Handsets).";
			break;
			
		/*U*/
		case 'USB':
			str = "Der Universal Serial Bus (USB) ist ein serielles Bussystem zur Verbindung Ihrer o2 HomeBox mit externen Geräten. Mit USB ausgestattete Speichermedien und Drucker können im laufenden Betrieb miteinander verbunden (Hot-Plugging) und deren Eigenschaften automatisch erkannt werden. Die o2 HomeBox hat 2 USB- Anschlüsse des Standards USB 2.0.";
			break;
			
		/*V*/
		case 'VDSL':
			str = "VDSL (Very High-Speed Digital Subscriber Line) bezeichnet eine DSL-Variante, mit der auf kurze Distanz bis zu 100 MBit/s Downlink möglich sind. Üblich sind, aufgrund von zu langen Distanzen und schlechter Leitungsqualität, Geschwindigkeiten von bis zu 25 MBit/s oder 50 MBit/s downlink.";
			break;
		case 'Verschlüsselung':
			str = "Kodierung von Daten (Umwandlung in eine unleserliche (verschlüsselte) Zeichenfolge) zum Schutz vor Unbefugten.";
			break;
		case 'VoIP':
			str = "Die Abkürzung VoIP steht für Voice over Internet Protocol und beschreibt die Sprachübertragung über das Internet. Hierzu wird die Sprache, im Gegensatz zum analogen Festnetzanschluss, digital in Form von Datenpaketen übertragen.";
			break;
			
		/*W*/
		case 'WEP':
			str = "WEP (Wired Equivalent Privacy) ist ein Standard-Verschlüsselungsalgorithmus für Drahtlosnetze (WLAN). Es gilt als unsicher und wird nicht zur Nutzung empfohlen.";
			break;
		case 'WLAN':
			str = "WLAN (Wireless Local Area Network) bezeichnet ein drahtloses lokales Netzwerk, das üblicherweise dem Standard IEEE 802.11 genügt. In einigen Ländern wird für WLAN das Synonym Wi-Fi benutzt. Ihre o2 HomeBox beinhaltet 2 unabhängige WLAN- Basisstationen im 2,4- und 5Hz- Bereich.";
			break;
		case 'WLAN_aktivieren':
			str = "Im Menü \"Heimnetz/WLAN\" kann die WLAN-Basisstation ein- und ausgeschaltet werden. Einfacher und schneller geht es jedoch mit WLAN-Taster an der o2 HomeBox. Einzelheiten zum WLAN-Taster enthält das Handbuch auf der beiliegenden CD.";
			break;
		case 'WLAN_Basisstation_verschlüsseln':
			str = "Zur Sicherung Ihres WLAN verwenden Sie bitte unbedingt eine Verschlüsselung. Stellen Sie dazu den Verschlüsselungsmodus ein und hinterlegen Sie einen Schlüssel. Beachten Sie, dass nur ein starker Schlüssel Ihr WLAN ausreichend sichert. Sichern Sie, wenn möglich, Ihr WLAN mit WPA und dem Modus WPA2/AES. Sehen Sie im Handbuch Ihres WLAN-Gerätes nach, ob dieser Verschlüsselungsmodus unterstützt wird. Ältere WLAN-Adapter unterstützen unter Umständen nur einen WEP-Schlüssel. Unterschiedliche Verschlüsselungsmodi können nicht konfiguriert werden.";
			break;
		case 'WLAN_Modus':
			str = "Der WLAN-Modus muss in Abstimmung mit Ihren WLAN-Geräten konfiguriert werden. Verwenden Sie ausschließlich Geräte, die einen der Standards 802.11b, 802.11g, 802.11n oder 802.11 ac erfüllen. Dann aktivieren Sie diesen Standard in der o2 HomeBox. Sind Sie nicht sicher, welcher Standard von Ihrem WLAN-Gerät erfüllt wird,  aktivieren Sie den Kombi-Modus der vorhandenen Standards. Zur Optimierung Ihres WLAN können Sie sowohl den Kanal als auch die Sendeleistung ändern.";
			break;
		case 'WPA':
			str = "WPA (Wi-Fi Protected Access) ist eine Verschlüsselungsmethode für ein Drahtlosnetzwerk (WLAN). Nachdem sich WEP als unsicher erwiesen hat, wurde WPA als neuer Standard etabliert. Der Nachfolger ist WPA2, der die grundlegenden Funktionen des neuen Sicherheitsstandards IEEE 802.11i implementiert.";
			break;
		case 'WPS':
			str = "WPS steht für \"Wi-Fi Protected Setup\" und ist ein Standard zum einfachen und sicheren Aufbau eines drahtlosen Heimnetzwerkes.  Weitere Details zum WPS-Verfahren finden Sie im Benutzerhandbuch der o2 HomeBox.";
			break;
		
		/*X*/
		/*Y*/
		/*Z*/
		case 'Zugangsdaten':
			str = "Um eine Verbindung zum Internet herzustellen, werden die Zugangsdaten benötigt, die Sie mit Ihrer Auftragsbestätigung erhalten haben. Bitte geben Sie den Benutzernamen und falls erforderlich das Passwort ein. Bitte beachten Sie Groß- und Kleinschreibung und die korrekte Schreibweise.";
			break;
		case 'Zurücksetzen':
			str = "Im Menü \"System/Zurücksetzen\" können Sie die o2 HomeBox in den Auslieferungszustand zurücksetzen. Wichtig: Alle Einstellungen gehen verloren, alle laufenden Gespräche und Internetverbindungen werden unterbrochen.";
			break;
		
		default:
			str = "Do not have this word!"
			break;
	}
	
	return str;
}