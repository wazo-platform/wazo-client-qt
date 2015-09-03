<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="pt_BR">
<context>
    <name>BaseEngine</name>
    <message>
        <location filename="src/baseengine.cpp" line="505"/>
        <source>Connection lost with XiVO CTI server</source>
        <translation>Conexão perdida com o servidor CTI XiVO</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="804"/>
        <source>Could not modify the Services data.</source>
        <translation>Não foi possível modificar os dados do Serviço</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="804"/>
        <source>Maybe Asterisk is down.</source>
        <translation>Talvez o servidor Asterisk esteja indisponível</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1200"/>
        <source>Server has sent an Error.</source>
        <translation>O servidor respondeu com um erro.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1204"/>
        <source>Your registration name &lt;%1@%2&gt; is not known by the XiVO CTI server on %3:%4.</source>
        <translation>Seu nome de registro &lt;%1@%2&gt; não foi reconhecido pelo servidor CTI XiVO em %3:%4.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1208"/>
        <source>You entered a wrong login / password.</source>
        <translation>Seu login / senha estão incorretos.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1210"/>
        <source>You have no profile defined.</source>
        <translation>Você não possui uma conta definida.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1214"/>
        <source>The server %1:%2 did not reply to the last keepalive packet.</source>
        <oldsource>The XiVO CTI server on %1:%2 did not reply to the last keepalive.</oldsource>
        <translation>O Servidor CTI XiVO em %1:%2 não respondeu ao último sinal.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1219"/>
        <source>You defined an IP address %1 that is probably an unresolved host name.</source>
        <translation>Você definiu um IP %1 que não se resolve para um hostname.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1222"/>
        <source>Socket timeout (~ 60 s) : you probably attempted to reach, via a gateway, an IP address %1 that does not exist.</source>
        <translation>Tempo de conexão excedido (~60s): provavelmente o endereço IP %1 não existe.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1226"/>
        <source>There seems to be a machine running on this IP address %1, and either no CTI server is running, or your port %2 is wrong.</source>
        <translation>Existe um conflito com o IP %1, o servidor CTI não está sendo executado ou a porta %2 não está correta.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1230"/>
        <source>An error occurred on the network while attempting to join the IP address %1 :
- no external route defined to access this IP address (~ no timeout)
- this IP address is routed but there is no machine (~ 5 s timeout)
- a cable has been unplugged on your LAN on the way to this IP address (~ 30 s timeout).</source>
        <translation>Um erro ocorreu na rede ao tentar utilizar o endereço %1:
- Não existe rota definida parar acessar este endereço IP(~ no timeout)
- O endereço IP está sendo roteado mas não existe máquina (˜5s timeout)
- O cabo está desconectado na LAN (~30s timetout)</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1236"/>
        <source>It seems that the server with IP address %1 does not accept encryption on its port %2. Please change either your port or your encryption setting.</source>
        <translation>O servidor com o endereço %1 não aceita criptografia na porta %2. Modifique a porta ou a configuração de criptografia.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1240"/>
        <source>An unknown socket error has occured while attempting to join the IP address:port %1:%2.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1244"/>
        <source>An unmanaged (number %1) socket error has occured while attempting to join the IP address:port %1:%2.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1249"/>
        <source>The XiVO CTI server on %1:%2 has just closed the connection.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1253"/>
        <source>The XiVO CTI server on %1:%2 has just been stopped.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1256"/>
        <source>The XiVO CTI server on %1:%2 has just been reloaded.</source>
        <translation type="unfinished"></translation>
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
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1265"/>
        <source>Max number (%1) of XiVO Clients already reached.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1267"/>
        <source>Missing Argument(s)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1271"/>
        <source>Your client version (%1) is too old for this server.
Please upgrade it to %2 at least.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1276"/>
        <source>Your client version (%1) is too old for this server.
Please upgrade it.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1283"/>
        <source>Your client&apos;s major version (%1)
is not the same as the server&apos;s (%2).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1290"/>
        <source>Your server version (%1) is too old for this client.
Please upgrade it to %2 at least.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1295"/>
        <source>Your server version (%1) is too old for this client.
Please upgrade it.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1299"/>
        <source>You were disconnected by the server.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1301"/>
        <source>You were forced to disconnect by the server.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1303"/>
        <source>Could not log agent: invalid extension.</source>
        <oldsource>Invalid extension number</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1305"/>
        <source>Could not log agent: extension already in use.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1308"/>
        <source>Unreachable number: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1312"/>
        <source>ERROR</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1527"/>
        <source>Attempting to reconnect to server</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="src/storage/queuememberinfo.cpp" line="86"/>
        <source>Agent</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/storage/queuememberinfo.cpp" line="86"/>
        <source>Phone</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QueueAgentStatus</name>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="51"/>
        <source>Dynamic membership</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="55"/>
        <source>Static/RT membership</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="67"/>
        <source>Agent not in Queue</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="71"/>
        <location filename="src/storage/queue_agent_status.cpp" line="83"/>
        <source>Agent in Queue</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="72"/>
        <location filename="src/storage/queue_agent_status.cpp" line="76"/>
        <source>Logged</source>
        <oldsource>Logged in</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="75"/>
        <source>Agent Called or Busy</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="79"/>
        <source>Agent in Queue but Invalid</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="84"/>
        <source>Unlogged</source>
        <oldsource>Logged out</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="92"/>
        <source>Unpaused</source>
        <oldsource>Not paused</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="96"/>
        <source>Paused</source>
        <translation type="unfinished">Pausado</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="100"/>
        <source>Not relevant</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
