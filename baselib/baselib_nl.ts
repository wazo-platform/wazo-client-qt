<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="nl">
<context>
    <name>BaseEngine</name>
    <message>
        <location filename="src/baseengine.cpp" line="505"/>
        <source>Connection lost with XiVO CTI server</source>
        <translation>De verbinding met de Xivo CTI server is verbroken</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="804"/>
        <source>Could not modify the Services data.</source>
        <translation>De gegevens voor Services konden niet worden gewijzigd.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="804"/>
        <source>Maybe Asterisk is down.</source>
        <translation>Misschien werkt Asterisk momenteel niet.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1200"/>
        <source>Server has sent an Error.</source>
        <translation>De server heeft een foutmelding teruggestuurd.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1204"/>
        <source>Your registration name &lt;%1@%2&gt; is not known by the XiVO CTI server on %3:%4.</source>
        <translation>Uw registratienaam &lt;%1@%2&gt; wordt niet herkend door de XiVO CTI server op %3:%4.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1208"/>
        <source>You entered a wrong login / password.</source>
        <translation>U hebt een verkeerd(e) login/paswoord ingevoerd.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1210"/>
        <source>You have no profile defined.</source>
        <translation>U hebt geen profiel toegekend.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1214"/>
        <source>The server %1:%2 did not reply to the last keepalive packet.</source>
        <oldsource>The XiVO CTI server on %1:%2 did not reply to the last keepalive.</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1219"/>
        <source>You defined an IP address %1 that is probably an unresolved host name.</source>
        <translation>U hebt een IP-adres %1 gedefinieerd dat waarschijnlijk een onbekende hostnaam is.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1222"/>
        <source>Socket timeout (~ 60 s) : you probably attempted to reach, via a gateway, an IP address %1 that does not exist.</source>
        <translation>Socket time-out (~ 60 s): u hebt waarschijnlijk via een gateway, een onbestaand IP-adres %1 proberen te bereiken.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1226"/>
        <source>There seems to be a machine running on this IP address %1, and either no CTI server is running, or your port %2 is wrong.</source>
        <translation>Er lijkt een machine te werken op dit IP-adres %1, maar ofwel is er geen CTI-server actief, ofwel is uw poort %2 verkeerd.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1230"/>
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
        <location filename="src/baseengine.cpp" line="1236"/>
        <source>It seems that the server with IP address %1 does not accept encryption on its port %2. Please change either your port or your encryption setting.</source>
        <translation>De server met IP adres %1 accepteert geen geëncrypteerde  verbindingen op poort %2. Gelieve de poort of de encryptie instellingen aan te passen.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1240"/>
        <source>An unknown socket error has occured while attempting to join the IP address:port %1:%2.</source>
        <translation>Er heeft zich een onbekende socketfout voorgedaan tijdens de poging om het IP-adres te bereiken:poort %1:%2.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1244"/>
        <source>An unmanaged (number %1) socket error has occured while attempting to join the IP address:port %1:%2.</source>
        <translation>Er heeft zich een onbeheerde socketfout (nummer %1) voorgedaan tijdens de poging om het IP-adres te bereiken: poort %1:%2.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1249"/>
        <source>The XiVO CTI server on %1:%2 has just closed the connection.</source>
        <translation>De XiVO CTI-server op %1:%2 heeft net de verbinding verbroken.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1253"/>
        <source>The XiVO CTI server on %1:%2 has just been stopped.</source>
        <translation>De XiVO CTI-server op %1:%2 werd net stopgezet.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1256"/>
        <source>The XiVO CTI server on %1:%2 has just been reloaded.</source>
        <translation>De XiVO CTI-server op %1:%2 werd net terug opgeladen.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1260"/>
        <source>You are already connected to %1:%2.</source>
        <oldsource>You are already connected from %1:%2.</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1262"/>
        <source>No capability allowed.</source>
        <translation>Geen capaciteit toegestaan.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1265"/>
        <source>Max number (%1) of XiVO Clients already reached.</source>
        <translation>Max. aantal (%1) XiVO Clients bereikt.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1267"/>
        <source>Missing Argument(s)</source>
        <translation>Ontbrekend(e) argument(en)</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1271"/>
        <source>Your client version (%1) is too old for this server.
Please upgrade it to %2 at least.</source>
        <translation>Uw klantversie (%1) is te oud voor deze server.
Gelieve op z&apos;n minst naar versie %2 te upgraden. </translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1276"/>
        <source>Your client version (%1) is too old for this server.
Please upgrade it.</source>
        <translation>Uw klantversie (%1) is te oud voor deze server.
Gelieve uw versie te upgraden.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1283"/>
        <source>Your client&apos;s major version (%1)
is not the same as the server&apos;s (%2).</source>
        <translation>De hoofdversie  (%1) van uw klant
is niet dezelfde als deze van de server (%2).</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1290"/>
        <source>Your server version (%1) is too old for this client.
Please upgrade it to %2 at least.</source>
        <translation>Uw serverversie (%1) is te oud voor deze klant.
Gelieve op z&apos;n minst naar versie %2 te upgraden.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1295"/>
        <source>Your server version (%1) is too old for this client.
Please upgrade it.</source>
        <translation>Uw serverversie (%1) is te oud voor deze klant.
Gelieve uw versie te upgraden.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1299"/>
        <source>You were disconnected by the server.</source>
        <translation>Uw verbinding werd verbroken door de server.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1301"/>
        <source>You were forced to disconnect by the server.</source>
        <translation>Uw verbinding werd gedwongen afgebroken door de server.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1303"/>
        <source>Could not log agent: invalid extension.</source>
        <oldsource>Invalid extension number</oldsource>
        <translation>Ongeldig extensie nummer</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1305"/>
        <source>Could not log agent: extension already in use.</source>
        <translation>Kon de agent niet aanmelden: extensie is al in gebruik</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1308"/>
        <source>Unreachable number: %1</source>
        <translation>Nummer onbereikbaar: %1</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1312"/>
        <source>ERROR</source>
        <translation>FOUT</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1527"/>
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
</TS>
