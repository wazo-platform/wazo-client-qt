<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="nl">
<context>
    <name>BaseEngine</name>
    <message>
        <location filename="src/baseengine.cpp" line="493"/>
        <source>Connection lost with XiVO CTI server</source>
        <translation>De verbinding met de Xivo CTI server is verbroken</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="754"/>
        <source>Could not modify the Services data.</source>
        <translation>De gegevens voor Services konden niet worden gewijzigd.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="754"/>
        <source>Maybe Asterisk is down.</source>
        <translation>Misschien werkt Asterisk momenteel niet.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1117"/>
        <source>Server has sent an Error.</source>
        <translation>De server heeft een foutmelding teruggestuurd.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1121"/>
        <source>Your registration name &lt;%1@%2&gt; is not known by the XiVO CTI server on %3:%4.</source>
        <translation>Uw registratienaam &lt;%1@%2&gt; wordt niet herkend door de XiVO CTI server op %3:%4.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1125"/>
        <source>You entered a wrong login / password.</source>
        <translation>U hebt een verkeerd(e) login/paswoord ingevoerd.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1127"/>
        <source>You have no profile defined.</source>
        <translation>U hebt geen profiel toegekend.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1131"/>
        <source>The server %1:%2 did not reply to the last keepalive packet.</source>
        <oldsource>The XiVO CTI server on %1:%2 did not reply to the last keepalive.</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1136"/>
        <source>You defined an IP address %1 that is probably an unresolved host name.</source>
        <translation>U hebt een IP-adres %1 gedefinieerd dat waarschijnlijk een onbekende hostnaam is.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1139"/>
        <source>Socket timeout (~ 60 s) : you probably attempted to reach, via a gateway, an IP address %1 that does not exist.</source>
        <translation>Socket time-out (~ 60 s): u hebt waarschijnlijk via een gateway, een onbestaand IP-adres %1 proberen te bereiken.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1143"/>
        <source>There seems to be a machine running on this IP address %1, and either no CTI server is running, or your port %2 is wrong.</source>
        <translation>Er lijkt een machine te werken op dit IP-adres %1, maar ofwel is er geen CTI-server actief, ofwel is uw poort %2 verkeerd.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1147"/>
        <source>An error occurred on the network while attempting to join the IP address %1 :
- no external route defined to access this IP address (~ no timeout)
- this IP address is routed but there is no machine (~ 5 s timeout)
- a cable has been unplugged on your LAN on the way to this IP address (~ 30 s timeout).</source>
        <translation>Er heeft zich een fout voorgedaan op het netwerk tijdens de poging om het IP-adres %1 te bereiken:
- er is geen externe route gedefinieerd voor toegang tot dit IP-adres (~ geen time-out)
- dit IP-adres heeft een route, maar er is geen machine (time-out ~ 5 s)
- er is een kabel losgeraakt op uw LAN op weg naar dit IP-adres (time-out ~ 30 s).</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1153"/>
        <source>It seems that the server with IP address %1 does not accept encryption on its port %2. Please change either your port or your encryption setting.</source>
        <translation>De server met IP adres %1 accepteert geen geëncrypteerde  verbindingen op poort %2. Gelieve de poort of de encryptie instellingen aan te passen.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1157"/>
        <source>An unknown socket error has occured while attempting to join the IP address:port %1:%2.</source>
        <translation>Er heeft zich een onbekende socketfout voorgedaan tijdens de poging om het IP-adres te bereiken:poort %1:%2.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1161"/>
        <source>An unmanaged (number %1) socket error has occured while attempting to join the IP address:port %1:%2.</source>
        <translation>Er heeft zich een onbeheerde socketfout (nummer %1) voorgedaan tijdens de poging om het IP-adres te bereiken: poort %1:%2.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1166"/>
        <source>The XiVO CTI server on %1:%2 has just closed the connection.</source>
        <translation>De XiVO CTI-server op %1:%2 heeft net de verbinding verbroken.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1170"/>
        <source>The XiVO CTI server on %1:%2 has just been stopped.</source>
        <translation>De XiVO CTI-server op %1:%2 werd net stopgezet.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1173"/>
        <source>The XiVO CTI server on %1:%2 has just been reloaded.</source>
        <translation>De XiVO CTI-server op %1:%2 werd net terug opgeladen.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1177"/>
        <source>You are already connected to %1:%2.</source>
        <oldsource>You are already connected from %1:%2.</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1179"/>
        <source>No capability allowed.</source>
        <translation>Geen capaciteit toegestaan.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1182"/>
        <source>Max number (%1) of XiVO Clients already reached.</source>
        <translation>Max. aantal (%1) XiVO Clients bereikt.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1184"/>
        <source>Missing Argument(s)</source>
        <translation>Ontbrekend(e) argument(en)</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1188"/>
        <source>Your client version (%1) is too old for this server.
Please upgrade it to %2 at least.</source>
        <translation>Uw klantversie (%1) is te oud voor deze server.
Gelieve op z&apos;n minst naar versie %2 te upgraden. </translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1193"/>
        <source>Your client version (%1) is too old for this server.
Please upgrade it.</source>
        <translation>Uw klantversie (%1) is te oud voor deze server.
Gelieve uw versie te upgraden.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1200"/>
        <source>Your client&apos;s major version (%1)
is not the same as the server&apos;s (%2).</source>
        <translation>De hoofdversie  (%1) van uw klant
is niet dezelfde als deze van de server (%2).</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1207"/>
        <source>Your server version (%1) is too old for this client.
Please upgrade it to %2 at least.</source>
        <translation>Uw serverversie (%1) is te oud voor deze klant.
Gelieve op z&apos;n minst naar versie %2 te upgraden.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1212"/>
        <source>Your server version (%1) is too old for this client.
Please upgrade it.</source>
        <translation>Uw serverversie (%1) is te oud voor deze klant.
Gelieve uw versie te upgraden.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1216"/>
        <source>You were disconnected by the server.</source>
        <translation>Uw verbinding werd verbroken door de server.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1218"/>
        <source>You were forced to disconnect by the server.</source>
        <translation>Uw verbinding werd gedwongen afgebroken door de server.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1220"/>
        <source>Could not log agent: invalid extension.</source>
        <oldsource>Invalid extension number</oldsource>
        <translation>Ongeldig extensie nummer</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1222"/>
        <source>Could not log agent: extension already in use.</source>
        <translation>Kon de agent niet aanmelden: extensie is al in gebruik</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1225"/>
        <source>Unreachable number: %1</source>
        <translation>Nummer onbereikbaar: %1</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1229"/>
        <source>ERROR</source>
        <translation>FOUT</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1385"/>
        <source>Attempting to reconnect to server</source>
        <translation>Poging tot herverbinden met de server</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="src/storage/queuememberinfo.cpp" line="86"/>
        <source>Agent</source>
        <translation>Agent</translation>
    </message>
    <message>
        <location filename="src/storage/queuememberinfo.cpp" line="86"/>
        <source>Phone</source>
        <translation>Telefoon</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="43"/>
        <source>N/A</source>
        <translation>NVT</translation>
    </message>
    <message>
        <location filename="../src/xletlib/lookup_directory_entry.cpp" line="98"/>
        <source>Remote search result</source>
        <translation>Resultaat externe zoekopdracht</translation>
    </message>
    <message>
        <location filename="../src/xletlib/mobile_directory_entry.cpp" line="84"/>
        <source>Mobile phone</source>
        <translation>GSM</translation>
    </message>
    <message>
        <location filename="../src/xletlib/current_filter_directory_entry.cpp" line="65"/>
        <source>Current Search</source>
        <translation>Huidige Zoekopdracht</translation>
    </message>
</context>
<context>
    <name>QueueAgentStatus</name>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="51"/>
        <source>Dynamic membership</source>
        <translation>Dynamisch lidmaatschap</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="55"/>
        <source>Static/RT membership</source>
        <translation>Statisch/RT-lidmaatchap</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="67"/>
        <source>Agent not in Queue</source>
        <translation>Agent niet in wachtrij</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="71"/>
        <location filename="src/storage/queue_agent_status.cpp" line="83"/>
        <source>Agent in Queue</source>
        <translation>Agent in wachtrij</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="72"/>
        <location filename="src/storage/queue_agent_status.cpp" line="76"/>
        <source>Logged</source>
        <oldsource>Logged in</oldsource>
        <translation>Ingelogd</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="75"/>
        <source>Agent Called or Busy</source>
        <translation>Agent opgeroepen of bezet</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="79"/>
        <source>Agent in Queue but Invalid</source>
        <translation>Agent in wachtrij maar ongeldig</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="84"/>
        <source>Unlogged</source>
        <oldsource>Logged out</oldsource>
        <translation>Uitgelogd</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="92"/>
        <source>Unpaused</source>
        <oldsource>Not paused</oldsource>
        <translation>Niet gepauzeerd</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="96"/>
        <source>Paused</source>
        <translation>Gepauzeerd</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="100"/>
        <source>Not relevant</source>
        <translation>Niet relevant</translation>
    </message>
</context>
<context>
    <name>Conference</name>
    <message>
        <location filename="../src/xlets/conference/conference.cpp" line="42"/>
        <source>Conference</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference.cpp" line="50"/>
        <source>Room list</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference.cpp" line="145"/>
        <source>%1 (%2)</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>ConferenceListModel</name>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="37"/>
        <source>Name</source>
        <translation>Naam</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="38"/>
        <source>Number</source>
        <translation>Nummer</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="39"/>
        <source>PIN code</source>
        <translation>Pincode</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="40"/>
        <source>Member count</source>
        <translation>Aantal deelnemers</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="41"/>
        <source>Started since</source>
        <translation>Gestart sinds</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="107"/>
        <source>Yes</source>
        <translation>Ja</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="109"/>
        <source>No</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="137"/>
        <source>Not started</source>
        <translation>Niet gestart</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="139"/>
        <source>Unknown</source>
        <translation>Onbekend</translation>
    </message>
</context>
<context>
    <name>ConferenceRoomModel</name>
    <message>
        <location filename="../src/xlets/conference/conference_room_model.cpp" line="42"/>
        <source>Name</source>
        <translation>Naam</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_room_model.cpp" line="43"/>
        <source>Number</source>
        <translation>Nummer</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_room_model.cpp" line="44"/>
        <source>Since</source>
        <translation>Sinds</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_room_model.cpp" line="158"/>
        <source>Mute/UnMute</source>
        <translation>Geluid uit/Geluid aan</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_room_model.cpp" line="174"/>
        <source>Unknown</source>
        <translation>Onbekend</translation>
    </message>
</context>
<context>
    <name>ConferenceWidget</name>
    <message>
        <location filename="../src/xlets/conference/conference_widget.ui" line="54"/>
        <source>Conference</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>XletDatetime</name>
    <message>
        <location filename="../src/xlets/datetime/datetime.cpp" line="42"/>
        <source>Date and Time</source>
        <translation>Datum en uur</translation>
    </message>
</context>
<context>
    <name>History</name>
    <message>
        <location filename="../src/xlets/history/history.cpp" line="39"/>
        <source>History</source>
        <translation>Historiek</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history.cpp" line="52"/>
        <source>All calls</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history.cpp" line="53"/>
        <source>Sent calls</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history.cpp" line="54"/>
        <source>Received calls</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history.cpp" line="55"/>
        <source>Missed calls</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>HistoryModel</name>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="41"/>
        <source>Name</source>
        <translation>Naam</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="42"/>
        <source>Number</source>
        <translation>Nummer</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="43"/>
        <source>Date</source>
        <translation>Datum</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="44"/>
        <source>Duration</source>
        <translation>Duur</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="160"/>
        <source>%1 hr %2 min %3 s</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="162"/>
        <source>%1 min %2 s</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="164"/>
        <source>%1 s</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>HistoryWidget</name>
    <message>
        <location filename="../src/xlets/history/history_widget.ui" line="54"/>
        <source>History</source>
        <translation>Historiek</translation>
    </message>
</context>
<context>
    <name>AgentsModel</name>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="43"/>
        <source>Number</source>
        <translation>Nummer</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="44"/>
        <source>First name</source>
        <translation>Voornaam</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="45"/>
        <source>Last name</source>
        <translation>Achternaam</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="46"/>
        <location filename="../src/xletlib/agents_model.cpp" line="200"/>
        <source>Listen</source>
        <translation>Luisteren</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="47"/>
        <source>Status since</source>
        <translation>Status sinds</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="48"/>
        <location filename="../src/xletlib/agents_model.cpp" line="390"/>
        <source>Logged in</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="49"/>
        <source>Joined
queues</source>
        <translation>Ingelogde
wachtrijen</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="50"/>
        <location filename="../src/xletlib/agents_model.cpp" line="411"/>
        <source>Paused</source>
        <translation>Gepauzeerd</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="51"/>
        <source>Paused
queues</source>
        <translation>Gepauzeerde
wachtrijen</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="268"/>
        <source>Agent ready to receive a call</source>
        <translation>Agent is klaar om oproepen te ontvangen</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="270"/>
        <source>Agent processing a call or paused</source>
        <translation>Agent gepauzeerd of in gesprek</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="272"/>
        <source>Agent receiving an internal call out of queue</source>
        <translation>Agent ontvangt een interne oproep buiten de wachtrij</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="274"/>
        <source>Agent receiving an external call out of queue</source>
        <translation>Agent ontvangt een externe oproep buiten de wachtrij</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="276"/>
        <source>Agent emitting an internal call</source>
        <translation>Agent maakt een interne oproep</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="278"/>
        <source>Agent emitting an external call</source>
        <translation>Agent maakt een externe oproep</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="334"/>
        <source>Not in use</source>
        <translation>Niet in gebruik</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="336"/>
        <source>In use</source>
        <translation>In gebruik</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="338"/>
        <source>Int. Incoming</source>
        <translation>Int. inkomend </translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="340"/>
        <source>Ext. Incoming</source>
        <translation>Ext. inkomend </translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="342"/>
        <source>Int. Outgoing</source>
        <translation>Int. Uitgaand</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="344"/>
        <source>Ext. Outgoing</source>
        <translation>Ext. Uitgaand</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="392"/>
        <source>Logged out</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="409"/>
        <source>Unpaused</source>
        <translation>Niet gepauzeerd</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="413"/>
        <source>Partially Paused</source>
        <translation>Gedeeltelijk gepauzeerd</translation>
    </message>
</context>
<context>
    <name>ChitChatWindow</name>
    <message>
        <location filename="../src/xletlib/chitchat.cpp" line="116"/>
        <source>&amp;Clear history</source>
        <translation>&amp;Historiek wissen/schrappen</translation>
    </message>
    <message>
        <location filename="../src/xletlib/chitchat.cpp" line="117"/>
        <source>&amp;Send</source>
        <translation>&amp;Verzenden</translation>
    </message>
    <message>
        <location filename="../src/xletlib/chitchat.cpp" line="138"/>
        <source>chat window opened with &quot;%1&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xletlib/chitchat.cpp" line="182"/>
        <source>you said: </source>
        <translation>U hebt gezegd: </translation>
    </message>
    <message>
        <location filename="../src/xletlib/chitchat.cpp" line="208"/>
        <source>chitchat - %1</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>DirectoryPanel</name>
    <message>
        <location filename="../src/xletlib/directorypanel.cpp" line="37"/>
        <source>Remote Directory Panel</source>
        <translation>Externe telefoonlijst venster</translation>
    </message>
    <message>
        <location filename="../src/xletlib/directorypanel.cpp" line="40"/>
        <source>Remote Di&amp;rectory</source>
        <translation>Externe telefoonlijst</translation>
    </message>
    <message>
        <location filename="../src/xletlib/directorypanel.cpp" line="48"/>
        <source>Search</source>
        <translation>Zoeken</translation>
    </message>
    <message>
        <location filename="../src/xletlib/directorypanel.cpp" line="131"/>
        <source>Double-click to send an E-mail to</source>
        <translation>Dubbel klik om een e-mail te sturen</translation>
    </message>
    <message>
        <location filename="../src/xletlib/directorypanel.cpp" line="133"/>
        <source>Double-click to call</source>
        <translation>Dubbel klik om te bellen</translation>
    </message>
</context>
<context>
    <name>ExtendedTableWidget</name>
    <message>
        <location filename="../src/xletlib/extendedtablewidget.cpp" line="63"/>
        <source>&amp;Dial</source>
        <translation>&amp;Bellen</translation>
    </message>
    <message>
        <location filename="../src/xletlib/extendedtablewidget.cpp" line="66"/>
        <source>Send an E-mail</source>
        <translation>Een e-mail verzenden</translation>
    </message>
</context>
<context>
    <name>ExternalPhoneDialog</name>
    <message>
        <location filename="../src/xletlib/externalphonedialog.cpp" line="46"/>
        <source>External phone number</source>
        <translation>Extern telefoonnummer</translation>
    </message>
    <message>
        <location filename="../src/xletlib/externalphonedialog.cpp" line="49"/>
        <source>Label</source>
        <translation>Label</translation>
    </message>
    <message>
        <location filename="../src/xletlib/externalphonedialog.cpp" line="51"/>
        <source>Phone number</source>
        <translation>Telefoonnummer</translation>
    </message>
</context>
<context>
    <name>NumberButtonDelegate</name>
    <message>
        <location filename="../src/xletlib/number_button_delegate.cpp" line="68"/>
        <source>CALL</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QueueEntriesModel</name>
    <message>
        <location filename="../src/xletlib/queue_entries/queue_entries_model.cpp" line="54"/>
        <source>ID</source>
        <translation>ID</translation>
    </message>
    <message>
        <location filename="../src/xletlib/queue_entries/queue_entries_model.cpp" line="55"/>
        <source>Position</source>
        <translation>Positie</translation>
    </message>
    <message>
        <location filename="../src/xletlib/queue_entries/queue_entries_model.cpp" line="56"/>
        <source>Name</source>
        <translation>Naam</translation>
    </message>
    <message>
        <location filename="../src/xletlib/queue_entries/queue_entries_model.cpp" line="57"/>
        <source>Number</source>
        <translation>Nummer</translation>
    </message>
    <message>
        <location filename="../src/xletlib/queue_entries/queue_entries_model.cpp" line="58"/>
        <source>Time</source>
        <translation>Tijd</translation>
    </message>
</context>
<context>
    <name>SearchDialog</name>
    <message>
        <location filename="../src/xletlib/searchdialog.cpp" line="40"/>
        <source>Search :</source>
        <translation>Opzoeking:</translation>
    </message>
</context>
<context>
    <name>QueueMembersHeader</name>
    <message numerus="yes">
        <location filename="../src/xlets/queue_members/queue_members_header.cpp" line="109"/>
        <source>%n agent(s)</source>
        <translation>
            <numerusform>%n agent(en)</numerusform>
            <numerusform>%n agent(en)</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../src/xlets/queue_members/queue_members_header.cpp" line="110"/>
        <source>%n phone(s)</source>
        <translation>
            <numerusform>%n telefoon(s)</numerusform>
            <numerusform>%n telefoon(s)</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_header.cpp" line="111"/>
        <source>&lt;b&gt;%1&lt;/b&gt; (%2@%3) : %4 and %5</source>
        <translation>&lt;b&gt;%1&lt;/b&gt; (%2@%3) : %4 en %5</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_header.cpp" line="123"/>
        <source>No selected queue</source>
        <translation>Geen queue geselecteerd</translation>
    </message>
</context>
<context>
    <name>QueueMembersModel</name>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="59"/>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="60"/>
        <source>ID</source>
        <translation>ID</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="61"/>
        <source>Number</source>
        <translation>Nummer</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="62"/>
        <source>Phone number of the queue</source>
        <translation>Telefoon nummer van de queue</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="63"/>
        <source>Firstname</source>
        <translation>Voornaam</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="64"/>
        <source>Queue member&apos;s firstname</source>
        <translation>Queue lid&apos;s voornaam </translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="65"/>
        <source>Lastname</source>
        <translation>Familienaam</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="66"/>
        <source>Queue member&apos;s lastname</source>
        <translation>Queue lid&apos;s familienaam </translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="67"/>
        <source>Logged in</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="68"/>
        <source>Queue member&apos;s login status</source>
        <translation>Queue lid&apos;s login status</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="69"/>
        <source>Paused</source>
        <translation>Gepauseerd</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="70"/>
        <source>Queue member&apos;s pause status</source>
        <translation>Queue lid&apos;s pauze status</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="71"/>
        <source>Answered calls</source>
        <translation>Beantwoorde oproepen</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="72"/>
        <source>Number of answered calls</source>
        <translation>Aantal beantwoorde oproepen</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="73"/>
        <source>Last call</source>
        <translation>Laatste oproep</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="74"/>
        <source>Hangup date of the last answered call</source>
        <translation>Datum waarop de laatst beantwoorde oproep opgehangen werd.</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="75"/>
        <source>Penalty</source>
        <translation>Penalty</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="76"/>
        <source>Queue member&apos;s penalty</source>
        <translation>Queue lid&apos;s penalty</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="196"/>
        <source>Always</source>
        <translation>Altijd</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="198"/>
        <source>Never</source>
        <translation>Nooit</translation>
    </message>
</context>
<context>
    <name>XletQueueMembers</name>
    <message>
        <location filename="../src/xlets/queue_members/queue_members.cpp" line="47"/>
        <source>Queue Members</source>
        <translation>Queue leden</translation>
    </message>
</context>
<context>
    <name>ServicesPanel</name>
    <message>
        <location filename="../src/xlets/services/servicespanel.cpp" line="41"/>
        <source>Services</source>
        <translation>Diensten</translation>
    </message>
</context>
<context>
    <name>ServicesWidget</name>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="46"/>
        <source>Services</source>
        <translation type="unfinished">Diensten</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="72"/>
        <source>Call &amp;filtering</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="84"/>
        <source>Do not &amp;disturb</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="114"/>
        <source>Call forwards</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="147"/>
        <source>Simple call forwards</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="214"/>
        <source>&amp;Unconditional forward to</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="235"/>
        <source>No call forward</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="393"/>
        <source>Forward on &amp;busy to</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="424"/>
        <source>Forward on &amp;no answer to</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>DirDialog</name>
    <message>
        <location filename="../src/xlets/fax/dirdialog.cpp" line="36"/>
        <source>Directory</source>
        <translation>Telefoonlijst</translation>
    </message>
</context>
<context>
    <name>Fax</name>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="42"/>
        <source>Fax</source>
        <translation>Fax</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/xlets/fax/fax.cpp" line="82"/>
        <source>%n page(s) sent</source>
        <translation type="unfinished">
            <numerusform></numerusform>
            <numerusform></numerusform>
        </translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="86"/>
        <source>Failed to send</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="98"/>
        <source>Open Fax File</source>
        <translation>Open Fax Bestand</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="100"/>
        <source>PDF Files (*.pdf);;All Files (*)</source>
        <translation>PDF Bestanden (*.pdf);;Alle Bestanden (*)</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="123"/>
        <source>Missing file and fax number</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="126"/>
        <source>Missing file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="129"/>
        <source>Missing fax number</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="145"/>
        <source>File empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="148"/>
        <source>File not found</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="157"/>
        <source>Unreachable number</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="166"/>
        <source>Sending...</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>FaxWidget</name>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="43"/>
        <source>Send fax</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="69"/>
        <source>1.</source>
        <translation type="unfinished">1.</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="84"/>
        <source>Choose file to send</source>
        <translation>Kies het bestand om te versturen</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="122"/>
        <source>file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="134"/>
        <source>BROWSE...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="163"/>
        <source>2.</source>
        <translation type="unfinished">2.</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="178"/>
        <source>Choose destination number</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="216"/>
        <source>fax number</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="286"/>
        <source>SEND FAX</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>IdentityDisplay</name>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="53"/>
        <source>Identity</source>
        <translation>Identiteit</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="60"/>
        <source>Current User Panel</source>
        <translation>Panel van huidige gebruikers</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="98"/>
        <source>Login</source>
        <translation type="unfinished">Inloggen</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="102"/>
        <source>Logout</source>
        <translation type="unfinished">Uitloggen</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="110"/>
        <source>Pause</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="114"/>
        <source>Unpause</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="377"/>
        <source>DND</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="378"/>
        <source>Do Not Disturb</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="380"/>
        <source>UNC %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="381"/>
        <source>Unconditional Forward towards %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="383"/>
        <source>BUSY %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="384"/>
        <source>Busy Forward towards %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="386"/>
        <source>FNA %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="387"/>
        <source>Non-Answer Forward towards %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="389"/>
        <source>CALL FILTER</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="392"/>
        <source>AVAILABLE</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>IdentityWidget</name>
    <message>
        <location filename="../src/xlets/identity/identity.ui" line="167"/>
        <source>VOICEMAIL</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.ui" line="205"/>
        <source>Call your voicemail</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.ui" line="364"/>
        <source>call</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>XletAgentDetails</name>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="49"/>
        <source>Agent Details</source>
        <translation>Gegevens van de agenten</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="56"/>
        <source>Queues</source>
        <translation>Wachtrijen</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="57"/>
        <source>Joined</source>
        <translation>Gegroepeerd</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="58"/>
        <source>Paused</source>
        <translation>Gepauzeerd</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="62"/>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="207"/>
        <source>Login</source>
        <translation>Inloggen</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="197"/>
        <source>on &lt;b&gt;%1&lt;/b&gt;</source>
        <translation>op &lt;b&gt;%1&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="203"/>
        <source>Logout</source>
        <translation>Uitloggen</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="227"/>
        <source>Not in use</source>
        <translation>Niet in gebruik</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="231"/>
        <source>In use</source>
        <translation>In gebruik</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="235"/>
        <source>Int. Incoming </source>
        <translation>Int. inkomend</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="239"/>
        <source>Ext. Incoming</source>
        <translation>Ext. inkomend </translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="243"/>
        <source>Int. Outgoing</source>
        <translation>Int. Uitgaand</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="247"/>
        <source>Ext. Outgoing</source>
        <translation>Ext. Uitgaand</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="251"/>
        <source>Unlogged</source>
        <translation>Niet ingelogged</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="255"/>
        <source>%1 since %2</source>
        <translation>%1 sedert %2</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="346"/>
        <source>Server: %1</source>
        <translation>Server: %1</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="347"/>
        <source>Context: %1</source>
        <translation>Context: %1</translation>
    </message>
</context>
<context>
    <name>AgentStatus</name>
    <message>
        <location filename="../src/xlets/agent_status_dashboard/agent_status.ui" line="21"/>
        <source>Form</source>
        <translation>Formulier</translation>
    </message>
    <message>
        <location filename="../src/xlets/agent_status_dashboard/agent_status.ui" line="49"/>
        <source>66:66</source>
        <translation>66:66</translation>
    </message>
</context>
<context>
    <name>XletAgentStatusDashboard</name>
    <message>
        <location filename="../src/xlets/agent_status_dashboard/agent_status_dashboard.cpp" line="48"/>
        <source>Agent status dashboard</source>
        <translation>Overzicht scherm agenten status</translation>
    </message>
</context>
<context>
    <name>QueueEntries</name>
    <message>
        <location filename="../src/xlets/queue_entries/queue_entries.cpp" line="50"/>
        <source>Calls of a Queue</source>
        <translation>Oproepen van een wachtrij</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_entries/queue_entries.cpp" line="107"/>
        <source>&lt;b&gt;%1&lt;/b&gt; (%2): %3 call(s)</source>
        <translation>&lt;b&gt;%1&lt;/b&gt; (%2): %3 oproep(en)</translation>
    </message>
</context>
<context>
    <name>ConfigWidget</name>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="92"/>
        <source>System Language</source>
        <translation>Systeem taal</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="93"/>
        <source>Dutch</source>
        <translation>Nederlands</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="94"/>
        <source>English</source>
        <translation>Engels</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="95"/>
        <source>French</source>
        <translation>Frans</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="96"/>
        <source>Italian</source>
        <translation>Italiaans</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="97"/>
        <source>Spanish (Chile)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="98"/>
        <location filename="../src/config_widget/config_widget.cpp" line="99"/>
        <location filename="../src/config_widget/config_widget.cpp" line="100"/>
        <location filename="../src/config_widget/config_widget.cpp" line="101"/>
        <location filename="../src/config_widget/config_widget.cpp" line="102"/>
        <source>incomplete</source>
        <translation>Onvolledig</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="98"/>
        <source>German</source>
        <translation>Duits</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="99"/>
        <source>Hungarian</source>
        <translation>Hongaars</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="100"/>
        <source>Japanese</source>
        <translation>Japans</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="101"/>
        <source>Portuguese</source>
        <translation>Portugees</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="102"/>
        <source>Spanish</source>
        <translation>Spaans</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="20"/>
        <source>XiVO Client Configuration</source>
        <translation>XiVO Client configuratie</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="37"/>
        <source>User</source>
        <translation>Gebruiker</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="45"/>
        <source>Language</source>
        <translation>Taal</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="80"/>
        <location filename="../src/config_widget/config_widget.ui" line="152"/>
        <location filename="../src/config_widget/config_widget.ui" line="722"/>
        <source>You must restart the program for this setting to apply.</source>
        <translation>U moet de toepassing herstarten op de instellingen toe te passen.</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="113"/>
        <source>Auto-reconnect </source>
        <translation>Verbinding automatisch herstellen</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="120"/>
        <source>Connect at startup</source>
        <translation>Verbinding maken bij het starten</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="127"/>
        <source>Start systrayed</source>
        <translation>Start in taakbalk</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="134"/>
        <source>Show agent options</source>
        <translation>Toon agent opties</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="155"/>
        <source>Show window when calling from external application</source>
        <translation>Toon scherm wanneer vanaf een externe applicatie gebeld wordt</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="190"/>
        <source>Reset dock positions</source>
        <translation>Herstel de locatie van de docks</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="226"/>
        <source>Features</source>
        <translation>Functies</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="250"/>
        <source>Presence</source>
        <translation>Aanwezigheid</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="256"/>
        <source>Presence reporting</source>
        <translation>Aanwezigheidsrapport</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="275"/>
        <source>Screen popup</source>
        <translation>Popup scherm</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="284"/>
        <source>Max screen popups</source>
        <translation>Maximum popup schermen</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="304"/>
        <source>Screen Popup</source>
        <translation>Popup scherm</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="314"/>
        <source>Allow automatic URL opening</source>
        <translation>Laat het automatisch openen van URL&apos;s toe</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="333"/>
        <source>History</source>
        <translation>Historiek</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="342"/>
        <source>History size</source>
        <translation>Grootte historiek</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="368"/>
        <source>Queue list</source>
        <translation>Wachtrij lijst</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="376"/>
        <source>Show longest wait time</source>
        <translation>Toon langste wachttijd</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="390"/>
        <source>Orange</source>
        <translation>Oranje</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="400"/>
        <source>Red</source>
        <translation>Rood</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="410"/>
        <source>Longest wait time thresholds</source>
        <translation>Langste wachttijd limiet</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="437"/>
        <source>Waiting calls thresholds</source>
        <translation>Wachtende oproepen limiet</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="464"/>
        <source>calls</source>
        <translation>oproepen</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="471"/>
        <source>seconds</source>
        <translation>seconden</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="502"/>
        <source>Queue Members</source>
        <translation>Queue leden</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="508"/>
        <source>Hide unlogged agents</source>
        <translation>Verberg queue leden die niet ingelogs zijn</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="524"/>
        <source>Switchboard</source>
        <translation>Schakelbord</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="533"/>
        <source>Switchboard call on hold queue name</source>
        <translation>schakelbord wachtrij naam voor wachtende oproepen </translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="543"/>
        <source>Switchboard queue name</source>
        <translation>Schakelbord wachtrij naam</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="565"/>
        <source>Administration</source>
        <translation>Administratie</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="573"/>
        <source>Server</source>
        <translation>Server</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="587"/>
        <location filename="../src/config_widget/config_widget.ui" line="634"/>
        <source>Port</source>
        <translation>Poort</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="620"/>
        <source>Backup server</source>
        <translation>Backup server</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="709"/>
        <source>Advanced</source>
        <translation>Geavanceerd</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="715"/>
        <source>Display the configuration profile</source>
        <translation>Toon het configuratie profiel</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="725"/>
        <source>Allow multiple instances</source>
        <translation>Laat meerdere instantie toe</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="736"/>
        <source>Auto-reconnect interval</source>
        <translation>Automatisch herverbinden interval</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="775"/>
        <source>Keepalive interval</source>
        <translation>Keepalive interval</translation>
    </message>
</context>
<context>
    <name>CustomerInfoPanel</name>
    <message>
        <location filename="../src/xlets/customerinfo-builtin/customerinfopanel.cpp" line="35"/>
        <source>Sheets</source>
        <translation>Tabbladen</translation>
    </message>
</context>
<context>
    <name>LoginWidget</name>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="20"/>
        <source>Login Screen</source>
        <translation>Login Scherm</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="144"/>
        <source>LOGIN</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="271"/>
        <source>XiVO is a unified communication system that connects phones inside an organization with public and mobile telephone networks.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="306"/>
        <source>login</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="340"/>
        <source>password</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="376"/>
        <source>Remember me</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="405"/>
        <source>No Agent</source>
        <translation>Geen agent</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="410"/>
        <source>Agent (logged off)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="415"/>
        <source>Agent (logged in)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="423"/>
        <source>phone</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="471"/>
        <source>CONNECT</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../src/main_window/main_window.ui" line="17"/>
        <source>XiVO Client</source>
        <translation>XiVO Client</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="53"/>
        <source>&amp;XiVO Client</source>
        <translation>&amp;XiVO Client</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="66"/>
        <source>&amp;Help</source>
        <translation>&amp;Help</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="88"/>
        <source>Confi&amp;gure</source>
        <translation>Confi&amp;gureren</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="91"/>
        <source>Configure account and connection options</source>
        <translation>Account en verbindingsopties configureren</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="99"/>
        <source>To S&amp;ystray</source>
        <translation>Naar S&amp;ystray</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="102"/>
        <source>Enter the system tray</source>
        <translation> De toepassing minimaliseren in de systeemnotificatiezone</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="107"/>
        <source>&amp;Connect</source>
        <translation>&amp;Verbinden</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="110"/>
        <source>Connect to the server</source>
        <translation>Verbinden met de server</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="118"/>
        <source>&amp;Disconnect</source>
        <translation>&amp;Verbinding verbreken</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="121"/>
        <source>Disconnect from the server</source>
        <translation>Verbinding met de server verbreken</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="129"/>
        <source>&amp;Quit</source>
        <translation>&amp;Verlaten</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="132"/>
        <source>Close the application</source>
        <translation>De toepassing sluiten</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="135"/>
        <source>Ctrl+Q</source>
        <translation>Ctrl+Q</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="143"/>
        <source>&amp;Show window</source>
        <translation>&amp;Venster weergeven</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="146"/>
        <source>Leave the system tray</source>
        <translation> De toepassing uit de systeemnotificatiezone halen</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="154"/>
        <source>&amp;About XiVO Client</source>
        <translation>&amp;Over XiVO Client</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="159"/>
        <source>About &amp;Qt</source>
        <translation>Over &amp;Qt</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="164"/>
        <source>&amp;Credits</source>
        <translation>&amp;Credits</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="110"/>
        <location filename="../src/main_window/main_window.cpp" line="212"/>
        <location filename="../src/main_window/main_window.cpp" line="397"/>
        <source>Client %1</source>
        <translation>Client %1</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="176"/>
        <source>XiVO CTI Error</source>
        <translation>XiVO CTI-fout</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="194"/>
        <source>Client %1 (%2 profile)</source>
        <translation>Client %1 (%2 profiel)</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="317"/>
        <source>&amp;Availability</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="389"/>
        <source>Fetch the last one</source>
        <translation>De laatste ophalen</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="396"/>
        <source>About XiVO Client</source>
        <translation>Over XiVO Client</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="398"/>
        <source>The XiVO CTI applications, once connected to the proper server, complete the XiVO telephony system, by providing computer tools, such as customer information display, directory searches, or various supervision &apos;X-lets&apos; for call centers.</source>
        <translation>De XiVO CTI-toepassingen, eens verbonden met de juiste server, vervolledigen het XiVO-telefoonsysteem, door computertools ter beschikking te stellen zoals de weergave van klanteninformatie, opzoekingen in de directory&apos;s, of verschillende supervisie-xlets voor callcenters.</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="403"/>
        <source>&lt;u&gt;Version Information&lt;/u&gt;</source>
        <translation>&lt;u&gt;Versie Informatie&lt;/u&gt;</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="404"/>
        <source>Major : &lt;b&gt;%1&lt;/b&gt; (XiVO branch)</source>
        <translation>Major: &lt;b&gt;%1&lt;/b&gt; (XiVO branch)</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="405"/>
        <source>Minor : git hash &lt;b&gt;%1&lt;/b&gt; from : %2</source>
        <translation>Minor : git hash &lt;b&gt;%1&lt;/b&gt; van : %2</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="408"/>
        <source>(Application Built on : %1)</source>
        <translation>(Applicatie Gecompileerd op : %1)</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="409"/>
        <source>(Application Launched on : %1)</source>
        <translation>(Applicatie gestart op: %1)</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="410"/>
        <source>(Config File Location : %1)</source>
        <translation>(Locatie Configuratie Bestand : %1)</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="413"/>
        <source>E-mail : </source>
        <translation>E-mail: </translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="414"/>
        <source>Phone : </source>
        <translation>Tel. : </translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="415"/>
        <source>Authors : </source>
        <translation>Auteurs : </translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="415"/>
        <source>Avencall Development Team</source>
        <translation>Avencall Development Team</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="417"/>
        <source>License : </source>
        <translation>Licentie: </translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="419"/>
        <source>with a special exception allowing you to link it with OpenSSL, under some conditions.</source>
        <translation>met een speciale uitzondering om het te koppelen met OpenSSL, onder bepaalde voorwaarden.</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="425"/>
        <source>Credits</source>
        <translation>Credits</translation>
    </message>
</context>
<context>
    <name>Statusbar</name>
    <message>
        <location filename="../src/main_window/statusbar.cpp" line="78"/>
        <source>Connected</source>
        <translation>Verbonden</translation>
    </message>
    <message>
        <location filename="../src/main_window/statusbar.cpp" line="84"/>
        <source>Disconnected</source>
        <translation>Verbinding verbroken</translation>
    </message>
</context>
<context>
    <name>loading_dialog</name>
    <message>
        <location filename="../src/main_window/loading_dialog.ui" line="17"/>
        <source>Loading Screen</source>
        <translation>Scherm laden</translation>
    </message>
    <message>
        <location filename="../src/main_window/loading_dialog.ui" line="34"/>
        <source>Loading...</source>
        <translation>Laden...</translation>
    </message>
</context>
<context>
    <name>Directory</name>
    <message>
        <location filename="../src/xlets/directory/directory.cpp" line="44"/>
        <source>Directory</source>
        <translation>Telefoonlijst</translation>
    </message>
</context>
<context>
    <name>DirectoryWidget</name>
    <message>
        <location filename="../src/xlets/directory/directory_widget.ui" line="14"/>
        <source>Form</source>
        <translation>Van</translation>
    </message>
</context>
<context>
    <name>XletAgents</name>
    <message>
        <location filename="../src/xlets/agents/agents.cpp" line="46"/>
        <source>Agents&apos; List (plain)</source>
        <translation>Agentenlijst (standaard)</translation>
    </message>
</context>
<context>
    <name>QueuesModel</name>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="42"/>
        <source>Number</source>
        <translation>Nummer</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="43"/>
        <source>Phone number of the queue</source>
        <translation>Telefoonnummer van de wachtrij</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="44"/>
        <source>Queues</source>
        <translation>Wachtrijen</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="45"/>
        <source>Queue names</source>
        <translation>Wachtrij namen</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="46"/>
        <source>Waiting calls</source>
        <translation>Oproepen in wacht</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="47"/>
        <source>Number of waiting calls</source>
        <translation>Aantal oproepen in wacht</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="48"/>
        <source>EWT</source>
        <translation>GWT</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="49"/>
        <source>Estimated Waiting Time</source>
        <translation>Geschatte Wachttijd</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="50"/>
        <source>Longest wait</source>
        <translation>Langste wachttijd</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="51"/>
        <source>Longest waiting call</source>
        <translation>Oproep met langste wachttijd</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="52"/>
        <source>Logged in</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="53"/>
        <source>Number of agents logged in</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="54"/>
        <source>Available</source>
        <translation>Beschikbaar</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="55"/>
        <source>Number of agents ready to take a call</source>
        <translation>Aantal agenten beschikbaar om oproepen te beantwoorden</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="56"/>
        <source>Talking</source>
        <translation>In gesprek</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="57"/>
        <source>Number of agents talking</source>
        <translation>Aantal agenten in gesprek</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="58"/>
        <source>Received</source>
        <translation>ontvangen</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="59"/>
        <source>Number of received calls</source>
        <translation>Aantal ontvangen oproepen</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="60"/>
        <source>Answered</source>
        <translation>Beantwoord</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="61"/>
        <source>Number of answered calls</source>
        <translation>Aantal beantwoorde oproepen</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="62"/>
        <source>Abandoned</source>
        <translation>Verlaten</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="63"/>
        <source>Number of abandoned calls</source>
        <translation>Aantal verlaten oproepen</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="64"/>
        <source>Mean Waiting Time</source>
        <translation>Gemiddelde Wachttijd</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="65"/>
        <source>Mean waiting time before getting an agent</source>
        <translation>Gemiddelde wachttijd op een agent te bereiken</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="66"/>
        <source>Max Waiting Time</source>
        <translation>Maximum wachttijd</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="67"/>
        <source>Maximum waiting time before getting an agent</source>
        <translation>Maximum wachttijd om een agent te bereiken</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="68"/>
        <source>Efficiency</source>
        <translation>Doeltreffendheid</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="69"/>
        <source>Ratio (Answered) / (Received)</source>
        <translation>Verhouding (Beantwoord) / (Ontvangen)</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="70"/>
        <source>QOS</source>
        <translation>QOS</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="71"/>
        <source>Ratio (Calls answered in less than X sec / Number of calls answered)</source>
        <translation>Verhouding (Oproepen beantwoord in minder dan X seconden / Aantal beantwoorde oproepen)</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="257"/>
        <source>N/A</source>
        <translation>N.v.t.</translation>
    </message>
</context>
<context>
    <name>XletQueues</name>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="54"/>
        <source>Queues</source>
        <translation>Queues</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="120"/>
        <source>Configure</source>
        <translation>Configureren</translation>
    </message>
</context>
<context>
    <name>XletQueuesConfigure</name>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="156"/>
        <source>Queues configuration</source>
        <translation>Configuratie van de wachtrijen</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="158"/>
        <source>Choose which queue should be displayed, and the
queues parameters for the Stats on slice:</source>
        <translation>Kies welke wachtrij wordt weergegeven, evenals de parameters van de wachtrij voor de statistieken van de periode:</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="169"/>
        <source>&amp;Close</source>
        <translation>&amp;Sluiten</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="182"/>
        <source>Queue</source>
        <translation>Wachtrij</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="183"/>
        <source>Qos - X (s)</source>
        <translation>Qos - X (s)</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="184"/>
        <source>This is the threshold in seconds to consider that the answer to a call was too late to be accounted as an answer of quality.</source>
        <translation>Dit is de drempel, in seconden, waarna een oproep te laat is beantwoord om nog aanzien te worden als een kwalitatieve oproep.</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="188"/>
        <source>Window (s)</source>
        <translation>Venster(s)</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="189"/>
        <source>The window is the period of time used to compute the statistics</source>
        <translation>De tijdspanne is de periode die gebruikt wordt om statistieken te berekenen</translation>
    </message>
</context>
<context>
    <name>CallAction</name>
    <message>
        <location filename="../src/xlets/people/people_action_generator.cpp" line="224"/>
        <source>Call</source>
        <translation type="unfinished">Bel</translation>
    </message>
</context>
<context>
    <name>ChatAction</name>
    <message>
        <location filename="../src/xlets/people/people_action_generator.cpp" line="291"/>
        <source>Send a message</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>ContactDialogWidget</name>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="17"/>
        <source>Personal Contact</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="37"/>
        <source>Last Name</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="44"/>
        <source>First Name</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="78"/>
        <source>Phone Number</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="85"/>
        <source>Mobile Number</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="105"/>
        <source>Company</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="121"/>
        <source>Fax</source>
        <translation type="unfinished">Fax</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="128"/>
        <source>Email</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>People</name>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="50"/>
        <source>People</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="60"/>
        <source>all</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="61"/>
        <source>favorites</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="62"/>
        <source>my contacts</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="220"/>
        <source>Import Results</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="221"/>
        <source>%1 contacts created
%2 contacts failed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="228"/>
        <source>Following lines are errors</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="231"/>
        <source>
line %1: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="291"/>
        <source>Removing this contact</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="292"/>
        <source>Removing this contact.
Are you sure ?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="307"/>
        <source>Deleting all personal contacts</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="308"/>
        <source>Deleting all personal contacts.
Are you sure ?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="399"/>
        <source>Save Personal Contacts</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="401"/>
        <source>CSV Files (*.csv);;All Files (*)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="427"/>
        <source>Import Personal Contacts</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="429"/>
        <source>CSV Files (UTF-8)(*.csv);;All Files (*)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="479"/>
        <source>Failed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="501"/>
        <source>Waiting</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>PeopleEntryNumberDelegate</name>
    <message>
        <location filename="../src/xlets/people/people_entry_delegate.cpp" line="129"/>
        <source>CALL</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_entry_delegate.cpp" line="238"/>
        <source>BLIND TRANSFER</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_entry_delegate.cpp" line="240"/>
        <source>ATTENDED TRANSFER</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>PeoplePersonalMigration</name>
    <message>
        <location filename="../src/xlets/people/people_personal_migration.cpp" line="89"/>
        <source>Contacts Migration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_personal_migration.cpp" line="90"/>
        <source>Your local contacts will be migrated to the People Xlet. %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_personal_migration.cpp" line="94"/>
        <source>Your local contacts are currently stored on your machine in:
%1
They will be migrated to the XiVO server in your personal database.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>PeopleWidget</name>
    <message>
        <location filename="../src/xlets/people/people_widget.ui" line="54"/>
        <source>Contacts</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_widget.ui" line="105"/>
        <source>search</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_widget.ui" line="133"/>
        <source>NEW CONTACT</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_widget.ui" line="149"/>
        <source>IMPORT</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_widget.ui" line="165"/>
        <source>EXPORT</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_widget.ui" line="181"/>
        <source>DELETE ALL CONTACTS</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>CurrentCall</name>
    <message>
        <location filename="../src/xlets/switchboard/current_call.cpp" line="50"/>
        <source>Attended T</source>
        <translation>Aangekondigd D</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.cpp" line="51"/>
        <source>Call</source>
        <translation>Bel</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.cpp" line="52"/>
        <source>Complete T</source>
        <translation>Vervolledig D</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.cpp" line="53"/>
        <source>Hangup</source>
        <translation>Ophangen</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.cpp" line="54"/>
        <source>Cancel</source>
        <translation>Annuleren</translation>
    </message>
</context>
<context>
    <name>CurrentCallWidget</name>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="14"/>
        <source>Form</source>
        <translation>Formulier</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="66"/>
        <source>Current Call</source>
        <translation>Huidige Oproep</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="82"/>
        <source>Enter</source>
        <translation>Enter</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="98"/>
        <source>Answer</source>
        <translation>Antwoorden</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="147"/>
        <source>Call</source>
        <translation>Bel</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="183"/>
        <source>Blind Transfer</source>
        <translation>Niet aangekondigde doorschakeling</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="186"/>
        <source>Blind T</source>
        <translation>Niet aangekondigd D</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="189"/>
        <source>F4</source>
        <translation>F4</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="222"/>
        <source>Attended Transfer</source>
        <translation>Aangekondigde doorschakeling</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="225"/>
        <source>Attended T</source>
        <translation>Aangekondigd D</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="228"/>
        <source>F5</source>
        <translation>F5</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="261"/>
        <source>Hold</source>
        <translation>Op wacht</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="264"/>
        <source>F7</source>
        <translation>F7</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="301"/>
        <source>Hangup</source>
        <translation>Ophangen</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="304"/>
        <source>F8</source>
        <translation>F8</translation>
    </message>
</context>
<context>
    <name>Switchboard</name>
    <message>
        <location filename="../src/xlets/switchboard/switchboard.cpp" line="51"/>
        <source>Switchboard</source>
        <translation>Schakelbord</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/xlets/switchboard/switchboard.cpp" line="323"/>
        <location filename="../src/xlets/switchboard/switchboard.cpp" line="333"/>
        <source>%n call(s)</source>
        <translation>
            <numerusform>%n oproep</numerusform>
            <numerusform>%n oproepen</numerusform>
        </translation>
    </message>
</context>
<context>
    <name>SwitchboardPanel</name>
    <message>
        <location filename="../src/xlets/switchboard/switchboard.ui" line="35"/>
        <source>Incoming Calls (F6)</source>
        <translation>Inkomende Oproepen (F6)</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/switchboard.ui" line="62"/>
        <location filename="../src/xlets/switchboard/switchboard.ui" line="132"/>
        <source>0 call</source>
        <translation>0 oproepen</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/switchboard.ui" line="105"/>
        <source>Waiting Calls (F9)</source>
        <translation>Oproepen in wacht (F9)</translation>
    </message>
</context>
</TS>
