<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.0" language="de_DE">
<context>
    <name>BaseEngine</name>
    <message>
        <location filename="src/baseengine.cpp" line="1249"/>
        <source>Received Services Data</source>
        <translation>Empfangene Dienstleistungsdaten</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1257"/>
        <source>Could not modify the Services data.</source>
        <translation>Die Dienstleistungsdaten konnten nicht verändert werden.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1257"/>
        <source>Maybe Asterisk is down.</source>
        <translation>Asterisk ist möglicherweise ausgefallen.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1501"/>
        <source>Server has sent an Error.</source>
        <translation>Der Server hat einen Fehler gesendet.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1505"/>
        <source>Your registration name &lt;%1@%2&gt; is not known by the XiVO CTI server on %3:%4.</source>
        <translation>Der XIVO CTI-Server auf %3:%4 kennt Ihren Registrierungsnamen &lt;%1@%2&gt; nicht.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1510"/>
        <source>You entered a wrong login / password.</source>
        <translation>Benutzername bzw. Kennwort wurden falsch eingegeben.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1513"/>
        <source>Your profile identifier &lt;%1&gt; is not defined.</source>
        <translation>Ihre Profilkennung &lt;%1&gt; ist nicht definiert.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1517"/>
        <source>The XiVO CTI server on %1:%2 did not reply to the last keepalive.</source>
        <translation>Der XIVO CTI-Server %1:%2 hat auf das letzte „Keep-alive“ nicht geantwortet.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1522"/>
        <source>You defined an IP address %1 that is probably an unresolved host name.</source>
        <translation>Sie haben eine IP-Adresse %1 definiert, die möglicherweise ein ungelöster Hostname ist.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1525"/>
        <source>Socket timeout (~ 60 s) : you probably attempted to reach, via a gateway, an IP address %1 that does not exist.</source>
        <translation>Sockettimeout (~ 60 s): Sie haben möglicherweise versucht, über ein Gateway eine IP-Adresse %1 zu erreichen, die nicht vorhanden ist.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1529"/>
        <source>There seems to be a machine running on this IP address %1, and either no CTI server is running, or your port %2 is wrong.</source>
        <translation>An dieser IP-Adresse %1 scheint ein Computer zu arbeiten, aber entweder arbeitet kein Server oder Ihr Port %2 ist falsch.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1533"/>
        <source>An error occurred on the network while attempting to join the IP address %1 :
- no external route defined to access this IP address (~ no timeout)
- this IP address is routed but there is no machine (~ 5 s timeout)
- a cable has been unplugged on your LAN on the way to this IP address (~ 30 s timeout).</source>
        <translation>



Beim Versuch, sich mit der IP-Adresse %1 zu verbinden, ist ein Fehler aufgetreten:
- keine externe Weiterleitung für diese IP-Adresse definiert (~ keine Zeitüberschreitung)
- diese IP-Adresse wird weitergeleitet, aber es ist kein Computer vorhanden (~ 5 s Zeitüberschreitung)
- an Ihrem LAN wurde auf dem Weg zu dieser IP-Adresse ein Kabel entfernt (~ 30 s Zeitüberschreitung).</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1539"/>
        <source>An unknown socket error has occured while attempting to join the IP address:port %1:%2.</source>
        <translation>Beim Versuch, sich mit der IP-Adresse: Port %1:%2 zu verbinden, ist ein unbekannter Socketfehler aufgetreten.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1543"/>
        <source>An unmanaged (number %1) socket error has occured while attempting to join the IP address:port %1:%2.</source>
        <translation>Beim Versuch, sich mit der IP-Adresse: Port %1:%2 zu verbinden, ist ein nicht verwalteter (Nummer %1) Socketfehler aufgetreten.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1548"/>
        <source>The XiVO CTI server on %1:%2 has just closed the connection.</source>
        <translation>Der XIVO CTI-Server auf %1:%2 hat die Verbindung gerade geschlossen.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1552"/>
        <source>The XiVO CTI server on %1:%2 has just been stopped.</source>
        <translation>Der XIVO CTI-Server auf %1:%2 wurde gerade angehalten.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1555"/>
        <source>The XiVO CTI server on %1:%2 has just been reloaded.</source>
        <translation>Der XIVO CTI-Server auf %1:%2 wurde gerade erneut geladen.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1559"/>
        <source>You are already connected from %1:%2.</source>
        <translation>Sie sind bereits von %1:%2 verbunden.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1561"/>
        <source>No capability allowed.</source>
        <translation>Keine Funktion zugelassen.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1564"/>
        <source>Max number (%1) of XiVO Clients already reached.</source>
        <translation>Maximale Anzahl (%1) an XIVO-Clients bereits erreicht.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1566"/>
        <source>Missing Argument(s)</source>
        <translation>Fehlende(s) Argument(e)</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1570"/>
        <source>Your client version (%1) is too old for this server.
Please upgrade it to %2 at least.</source>
        <translation>
Ihre Client-Version (%1) ist zu alt für diesen Server.
Aktualisieren Sie sie bitte mindestens auf Version %2.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1575"/>
        <source>Your client version (%1) is too old for this server.
Please upgrade it.</source>
        <translation>
Ihre Client-Version (%1) ist zu alt für diesen Server.
Aktualisieren Sie sie bitte.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1582"/>
        <source>Your client's major version (%1)
is not the same as the server's (%2).</source>
        <translation>
Die Hauptversion Ihres Clients (%1)
ist nicht dieselbe wie jene Ihres Servers (%2).</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1589"/>
        <source>Your server version (%1) is too old for this client.
Please upgrade it to %2 at least.</source>
        <translation>
Ihre Server-Version (%1) ist zu alt für diesen Client.
Aktualisieren Sie sie bitte mindestens auf Version %2.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1594"/>
        <source>Your server version (%1) is too old for this client.
Please upgrade it.</source>
        <translation>
Ihre Server-Version (%1) ist zu alt für diesen Client.
Aktualisieren Sie sie bitte.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1598"/>
        <source>You were disconnected by the server.</source>
        <translation>Der Server hat Ihre Verbindung getrennt.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1600"/>
        <source>You were forced to disconnect by the server.</source>
        <translation>Sie wurden vom Server zur Trennung der Verbindung gezwungen.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1604"/>
        <source>ERROR</source>
        <translation>FEHLER</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1959"/>
        <source>Attempting to reconnect to server</source>
        <translation>Versuchen, den Server erneut zu verbinden</translation>
    </message>
</context>
<context>
    <name>BasePeerWidget</name>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="55"/>
        <source>&amp;Open a chat window</source>
        <translation>&amp;Ein Chat-Fenster öffnen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="56"/>
        <source>Open a chat window with this user</source>
        <translation>Ein Chat-Fenster mit diesem Benutzer öffnen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="63"/>
        <source>&amp;Remove</source>
        <translation>&amp;Entfernen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="64"/>
        <source>Remove this peer from the panel</source>
        <translation>Diesen Kollegen aus dem Bereich entfernen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="68"/>
        <source>Re&amp;name</source>
        <translation>Um&amp;benennen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="69"/>
        <source>Rename this peer</source>
        <translation>Diesen Kollegen umbenennen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="73"/>
        <location filename="src/gui/basepeerwidget.cpp" line="483"/>
        <location filename="src/gui/basepeerwidget.cpp" line="487"/>
        <source>&amp;Intercept</source>
        <translation>&amp;Unterbrechen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="74"/>
        <source>Intercept call</source>
        <translation>Anruf unterbrechen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="372"/>
        <source>Call this peer</source>
        <translation>Diesen Kollegen anrufen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="376"/>
        <source>&amp;Call mobile</source>
        <translation>&amp;Mobiltelefon anrufen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="382"/>
        <source>&amp;Call</source>
        <translation>&amp;Anrufen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="389"/>
        <source>&amp;Edit</source>
        <translation>&amp;Bearbeiten</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="462"/>
        <location filename="src/gui/basepeerwidget.cpp" line="466"/>
        <source>&amp;Hangup</source>
        <translation>&amp;Auflegen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="467"/>
        <source>Hangup this communication</source>
        <translation>Dieses Gespräch beenden</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="488"/>
        <source>Intercept this communication</source>
        <translation>Dieses Gespräch unterbrechen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="500"/>
        <location filename="src/gui/basepeerwidget.cpp" line="504"/>
        <source>&amp;Park</source>
        <translation>&amp;Parken</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="505"/>
        <source>Park this call</source>
        <translation>Diesen Anruf parken</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="541"/>
        <source>Invite in meetme room %1</source>
        <translation>In MeetMe-Raum %1 einladen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="549"/>
        <location filename="src/gui/basepeerwidget.cpp" line="555"/>
        <source>Direct &amp;Transfer</source>
        <translation>Direktes &amp;Durchstellen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="553"/>
        <location filename="src/gui/basepeerwidget.cpp" line="577"/>
        <source>Transfer this communication</source>
        <translation>Dieses Gespräch durchstellen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="556"/>
        <location filename="src/gui/basepeerwidget.cpp" line="583"/>
        <source>Transfer to this person</source>
        <translation>Zu dieser Person durchstellen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="571"/>
        <location filename="src/gui/basepeerwidget.cpp" line="582"/>
        <source>&amp;Indirect Transfer</source>
        <translation>&amp;Indirektes Durchstellen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="578"/>
        <source>(Cancel)</source>
        <translation>(Abbrechen)</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="580"/>
        <location filename="src/gui/basepeerwidget.cpp" line="585"/>
        <source>Cancel the Transfer</source>
        <translation>Das Durchstellen abbrechen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="584"/>
        <source>&amp;Cancel the Transfer</source>
        <translation>&amp;Das Durchstellen abbrechen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="607"/>
        <location filename="src/gui/basepeerwidget.cpp" line="613"/>
        <source>Transfer to &amp;voice mail</source>
        <translation>Zu &amp;Voicemail weiterleiten</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="611"/>
        <location filename="src/gui/basepeerwidget.cpp" line="614"/>
        <source>Transfer to voice mail</source>
        <translation>Zu Voicemail weiterleiten</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="755"/>
        <source>XiVO Client - Removing %1 %2</source>
        <translation>XIVO-Client - %1 %2 entfernen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="756"/>
        <source>Removing %1 %2.
Are you sure ?</source>
        <translation>
%1 %2 entfernen.
Sind Sie sicher?</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="770"/>
        <source>Rename Item</source>
        <translation>Element umbenennen</translation>
    </message>
    <message>
        <location filename="src/gui/basepeerwidget.cpp" line="770"/>
        <source>Rename %1 :</source>
        <translation>%1 umbenennen:</translation>
    </message>
</context>
<context>
    <name>BasicPeerWidget</name>
    <message>
        <location filename="src/gui/basicpeerwidget.cpp" line="54"/>
        <source>(No callerid yet)</source>
        <translation>(Noch keine Anrufer-ID)</translation>
    </message>
</context>
<context>
    <name>ChitChatWindow</name>
    <message>
        <location filename="src/gui/chitchat.cpp" line="82"/>
        <source>&amp;Clear history</source>
        <translation>&amp;Verlauf löschen</translation>
    </message>
    <message>
        <location filename="src/gui/chitchat.cpp" line="83"/>
        <source>&amp;Send</source>
        <translation>&amp;Senden</translation>
    </message>
    <message>
        <location filename="src/gui/chitchat.cpp" line="101"/>
        <source>chitchat - %0</source>
        <translation>Plaudern - %0</translation>
    </message>
    <message>
        <location filename="src/gui/chitchat.cpp" line="131"/>
        <location filename="src/gui/chitchat.cpp" line="176"/>
        <source>chat window opened with user - </source>
        <translation>Chat-Fenster mit Benutzer geöffnet - </translation>
    </message>
    <message>
        <location filename="src/gui/chitchat.cpp" line="147"/>
        <source>you said: </source>
        <translation>Sie sagten: </translation>
    </message>
    <message>
        <location filename="src/gui/chitchat.cpp" line="176"/>
        <source>system: </source>
        <translation>System: </translation>
    </message>
</context>
<context>
    <name>CtiConn</name>
    <message>
        <location filename="src/cticonn.cpp" line="87"/>
        <source>Connection lost with XiVO CTI server</source>
        <translation>Verbindung mit dem XIVO CTI-Server verloren</translation>
    </message>
</context>
<context>
    <name>ExtendedTableWidget</name>
    <message>
        <location filename="src/gui/extendedtablewidget.cpp" line="76"/>
        <source>&amp;Remove</source>
        <translation>&amp;Entfernen</translation>
    </message>
    <message>
        <location filename="src/gui/extendedtablewidget.cpp" line="80"/>
        <source>&amp;Dial</source>
        <translation>&amp;Wählen</translation>
    </message>
    <message>
        <location filename="src/gui/extendedtablewidget.cpp" line="83"/>
        <source>Direct &amp;Transfer</source>
        <translation>Direktes &amp;Durchstellen</translation>
    </message>
    <message>
        <location filename="src/gui/extendedtablewidget.cpp" line="84"/>
        <source>&amp;Indirect Transfer</source>
        <translation>&amp;Indirektes Durchstellen</translation>
    </message>
    <message>
        <location filename="src/gui/extendedtablewidget.cpp" line="120"/>
        <source>Send an E-mail</source>
        <translation>Eine E-Mail senden</translation>
    </message>
    <message>
        <location filename="src/gui/extendedtablewidget.cpp" line="229"/>
        <source>Removing this contact</source>
        <translation>Diesen Kontakt entfernen</translation>
    </message>
    <message>
        <location filename="src/gui/extendedtablewidget.cpp" line="230"/>
        <source>Removing this contact.
Are you sure ?</source>
        <translation>
Diesen Kontakt entfernen.
Sind Sie sicher?</translation>
    </message>
</context>
<context>
    <name>ExternalPhoneDialog</name>
    <message>
        <location filename="src/gui/externalphonedialog.cpp" line="50"/>
        <source>External phone number</source>
        <translation>Externe Telefonnummer</translation>
    </message>
    <message>
        <location filename="src/gui/externalphonedialog.cpp" line="53"/>
        <source>Label</source>
        <translation>Bezeichnung</translation>
    </message>
    <message>
        <location filename="src/gui/externalphonedialog.cpp" line="55"/>
        <source>Phone number</source>
        <translation>Telefonnummer</translation>
    </message>
</context>
<context>
    <name>PeerWidget</name>
    <message>
        <location filename="src/gui/peerwidget.cpp" line="125"/>
        <source>Agent %1</source>
        <translation>Agent %1</translation>
    </message>
    <message>
        <location filename="src/gui/peerwidget.cpp" line="152"/>
        <source>Mobile number : %1</source>
        <translation>Mobiltelefonnummer: %1</translation>
    </message>
    <message>
        <location filename="src/gui/peerwidget.cpp" line="162"/>
        <source>User : %1</source>
        <translation>Benutzer: %1</translation>
    </message>
    <message>
        <location filename="src/gui/peerwidget.cpp" line="176"/>
        <source>Phone %1 : %2</source>
        <translation>Telefon %1: %2</translation>
    </message>
    <message numerus="yes">
        <location filename="src/gui/peerwidget.cpp" line="186"/>
        <source>Agent %1
%n Queue(s) %2</source>
        <translation><numerusform>
Agent %1
%n Warteschlange(n) %2</numerusform></translation>
    </message>
    <message>
        <location filename="src/gui/peerwidget.cpp" line="195"/>
        <source>(No callerid yet)</source>
        <translation>(Noch keine Anrufer-ID)</translation>
    </message>
    <message>
        <location filename="src/gui/peerwidget.cpp" line="198"/>
        <source>%1 &lt;%2&gt;</source>
        <translation>%1 &lt;%2&gt;</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../3rdparty/json_jsonqt/lib/JsonToProperties.cpp" line="49"/>
        <source>Found property %1 with incompatible data type.</source>
        <translation>Eigenschaft %1 mit nicht kompatiblem Datentyp gefunden.</translation>
    </message>
    <message>
        <location filename="../3rdparty/json_jsonqt/lib/JsonToVariant.cpp" line="192"/>
        <source>string, number, object, array, bool, or null</source>
        <translation>String, Zahl, Objekt, Datenfeld, Boolsche Variable oder Null</translation>
    </message>
    <message>
        <location filename="../3rdparty/json_jsonqt/lib/JsonToVariant.cpp" line="258"/>
        <source>Any unicode character except for " or JSON escape sequences</source>
        <translation>Alle Unicode-Zeichen außer " oder JSON-Escapesequenzen</translation>
    </message>
    <message>
        <location filename="../3rdparty/json_jsonqt/lib/ParseException.cpp" line="35"/>
        <source>A parsing error occurred:
	Got: '%1'
	Expected: '%2'
	At: '%3'</source>
        <translation>


Ein Analysefehler ist aufgetreten:
	Erhalten: '%1'
	Erwartet: '%2'
	Bei: '%3'</translation>
    </message>
</context>
<context>
    <name>QueueAgentStatus</name>
    <message>
        <location filename="src/queue_agent_status.cpp" line="52"/>
        <source>Dynamic membership</source>
        <translation>Dynamische Mitgliedschaft</translation>
    </message>
    <message>
        <location filename="src/queue_agent_status.cpp" line="57"/>
        <source>Static/RT membership</source>
        <translation>Statische/RT-Mitgliedschaft</translation>
    </message>
    <message>
        <location filename="src/queue_agent_status.cpp" line="69"/>
        <source>Agent not in Queue</source>
        <translation>Agent nicht in Warteschlange</translation>
    </message>
    <message>
        <location filename="src/queue_agent_status.cpp" line="73"/>
        <location filename="src/queue_agent_status.cpp" line="85"/>
        <source>Agent in Queue</source>
        <translation>Agent in Warteschlange</translation>
    </message>
    <message>
        <location filename="src/queue_agent_status.cpp" line="74"/>
        <location filename="src/queue_agent_status.cpp" line="78"/>
        <source>Logged in</source>
        <translation>Angemeldet</translation>
    </message>
    <message>
        <location filename="src/queue_agent_status.cpp" line="77"/>
        <source>Agent Called or Busy</source>
        <translation>Agent angerufen oder besetzt</translation>
    </message>
    <message>
        <location filename="src/queue_agent_status.cpp" line="81"/>
        <source>Agent in Queue but Invalid</source>
        <translation>Agent in Warteschlange, aber ungültig</translation>
    </message>
    <message>
        <location filename="src/queue_agent_status.cpp" line="86"/>
        <source>Logged out</source>
        <translation>Abgemeldet</translation>
    </message>
    <message>
        <location filename="src/queue_agent_status.cpp" line="94"/>
        <source>Not paused</source>
        <translation>Nicht angehalten</translation>
    </message>
    <message>
        <location filename="src/queue_agent_status.cpp" line="98"/>
        <source>Paused</source>
        <translation>Angehalten</translation>
    </message>
    <message>
        <location filename="src/queue_agent_status.cpp" line="102"/>
        <source>Not relevant</source>
        <translation>Nicht relevant</translation>
    </message>
</context>
<context>
    <name>SearchDialog</name>
    <message>
        <location filename="src/gui/searchdialog.cpp" line="43"/>
        <source>Search :</source>
        <translation>Suchen:</translation>
    </message>
</context>
</TS>
