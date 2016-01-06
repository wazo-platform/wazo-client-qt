<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="es_CL">
<context>
    <name>BaseEngine</name>
    <message>
        <location filename="src/baseengine.cpp" line="493"/>
        <source>Connection lost with XiVO CTI server</source>
        <translation>Pérdida de conexión con el servidor CTI de XiVO</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="754"/>
        <source>Could not modify the Services data.</source>
        <translation>No se puede modificar los datos del servicio.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="754"/>
        <source>Maybe Asterisk is down.</source>
        <translation>Probablemente Asterisk está caído.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1117"/>
        <source>Server has sent an Error.</source>
        <translation>El servidor ha enviado un error.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1121"/>
        <source>Your registration name &lt;%1@%2&gt; is not known by the XiVO CTI server on %3:%4.</source>
        <translation>Su nombre de registro  &lt;%1@%2&gt;  no es conocido por el servidor CTI de XiVO en %3:%4.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1125"/>
        <source>You entered a wrong login / password.</source>
        <translation>Ha ingresado un usuario o contraseña incorrectos.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1127"/>
        <source>You have no profile defined.</source>
        <translation>No tiene un perfil definido.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1131"/>
        <source>The server %1:%2 did not reply to the last keepalive packet.</source>
        <oldsource>The XiVO CTI server on %1:%2 did not reply to the last keepalive.</oldsource>
        <translation>El servidor CTI de XiVO en %1:%2 no ha respondido al último &apos;keepalive&apos;.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1136"/>
        <source>You defined an IP address %1 that is probably an unresolved host name.</source>
        <translation>Ha definido una dirección IP %1 que probablemente no tiene una resolución para el nombre de equipo.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1139"/>
        <source>Socket timeout (~ 60 s) : you probably attempted to reach, via a gateway, an IP address %1 that does not exist.</source>
        <translation>Tiempo exedido del socket (~ 60 s):  probablemente intentaste alcanzar, a través de una pasarela, una dirección IP %1 que no existe.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1143"/>
        <source>There seems to be a machine running on this IP address %1, and either no CTI server is running, or your port %2 is wrong.</source>
        <translation>Aparentemente existe una máquina corriendo en la dirección IP %1, sin embargo no existe un servidor CTI corriendo, o la puerta %2 es incorrecta.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1147"/>
        <source>An error occurred on the network while attempting to join the IP address %1 :
- no external route defined to access this IP address (~ no timeout)
- this IP address is routed but there is no machine (~ 5 s timeout)
- a cable has been unplugged on your LAN on the way to this IP address (~ 30 s timeout).</source>
        <translation>Un error ha ocurrido en la red mientras se intentaba unir la dirección IP %1:
- no se definió una ruta externa para acceder ésta dirección IP (~ sin tiempo exedido)
- ésta dirección IP se encuentra enrutada pero no existe una máquina (~5 s tiempo exedido)
- un cable de encuentra desconectado en la red camino a ésta dirección IP (~ 30 s tiempo exedido)

</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1153"/>
        <source>It seems that the server with IP address %1 does not accept encryption on its port %2. Please change either your port or your encryption setting.</source>
        <translation>Aparentemente el servidor en la dirección IP %1 no está aceptando encriptación en el puerto %2.  Por favor corrige el número de puerto o los parámetros de encriptación.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1157"/>
        <source>An unknown socket error has occured while attempting to join the IP address:port %1:%2.</source>
        <translation>Ha ocurrido un error desconocido en el socket mientras se intentaba unir a la dirección IP:puerto %1:%2.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1161"/>
        <source>An unmanaged (number %1) socket error has occured while attempting to join the IP address:port %1:%2.</source>
        <translation>Ha ocurrido un error de socket (número %1) no administrado mientras se intentaba unir la dirección IP.puerto %1:%2.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1166"/>
        <source>The XiVO CTI server on %1:%2 has just closed the connection.</source>
        <translation>El servidor CTI de XiVO en %1:%2 justo ha cerrado la conexión.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1170"/>
        <source>The XiVO CTI server on %1:%2 has just been stopped.</source>
        <translation>El servidor CTI de XiVO en %1:%2 justo ha sido detenido.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1173"/>
        <source>The XiVO CTI server on %1:%2 has just been reloaded.</source>
        <translation>El servidor CTI de XiVO en %1:%2 justo ha sido reiniciado.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1177"/>
        <source>You are already connected to %1:%2.</source>
        <oldsource>You are already connected from %1:%2.</oldsource>
        <translation>Ya se encuentra conectado desde %1:%2.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1179"/>
        <source>No capability allowed.</source>
        <translation>Sin la capacidad permitida.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1182"/>
        <source>Max number (%1) of XiVO Clients already reached.</source>
        <translation>Número máximo (%1) de clientes XiVO ya se ha alcanzado.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1184"/>
        <source>Missing Argument(s)</source>
        <translation>Argumento(s) perdido(s).</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1188"/>
        <source>Your client version (%1) is too old for this server.
Please upgrade it to %2 at least.</source>
        <translation>Su versión del cliente (%1) es demasiado antigua para éste servidor.
Por favor actualice a la versión %2 como mínimo.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1193"/>
        <source>Your client version (%1) is too old for this server.
Please upgrade it.</source>
        <translation>Su versión del cliente (%1) es demasiado antigua para éste servidor.
Por favor actualízece.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1200"/>
        <source>Your client&apos;s major version (%1)
is not the same as the server&apos;s (%2).</source>
        <translation>Su versión mayor (%1) del cliente
no es la misma que la del servidor (%2).</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1207"/>
        <source>Your server version (%1) is too old for this client.
Please upgrade it to %2 at least.</source>
        <translation>Su versión del servidor (%1) es demasiado antigua para éste cliente.
Por favor actualice a la versión %2 como mínimo.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1212"/>
        <source>Your server version (%1) is too old for this client.
Please upgrade it.</source>
        <translation>Su versión del servidor (%1) es demasiado antigua para éste cliente.
Por favor actualice.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1216"/>
        <source>You were disconnected by the server.</source>
        <translation>Ha sido desconectado por el servidor.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1218"/>
        <source>You were forced to disconnect by the server.</source>
        <translation>Ha sido desconectado forzosamente por el servidor.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1220"/>
        <source>Could not log agent: invalid extension.</source>
        <oldsource>Invalid extension number</oldsource>
        <translation>Número de extensión no válido.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1222"/>
        <source>Could not log agent: extension already in use.</source>
        <translation>No se puede logear el agente: La extensión ya se encuentra en uso.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1225"/>
        <source>Unreachable number: %1</source>
        <translation>Número inalcanzable: %1</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1229"/>
        <source>ERROR</source>
        <translation>ERROR</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1385"/>
        <source>Attempting to reconnect to server</source>
        <translation>Intentando reconexión con el servidor.</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="src/storage/queuememberinfo.cpp" line="86"/>
        <source>Agent</source>
        <translation>Agente</translation>
    </message>
    <message>
        <location filename="src/storage/queuememberinfo.cpp" line="86"/>
        <source>Phone</source>
        <translation>Teléfono</translation>
    </message>
</context>
<context>
    <name>QueueAgentStatus</name>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="51"/>
        <source>Dynamic membership</source>
        <translation>Membresía dinámica</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="55"/>
        <source>Static/RT membership</source>
        <translation>Membresía estática</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="67"/>
        <source>Agent not in Queue</source>
        <translation>Agente no en la cola</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="71"/>
        <location filename="src/storage/queue_agent_status.cpp" line="83"/>
        <source>Agent in Queue</source>
        <translation>Agente en la cola</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="72"/>
        <location filename="src/storage/queue_agent_status.cpp" line="76"/>
        <source>Logged</source>
        <oldsource>Logged in</oldsource>
        <translation>Logueado</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="75"/>
        <source>Agent Called or Busy</source>
        <translation>Agente en una llamada u ocupado</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="79"/>
        <source>Agent in Queue but Invalid</source>
        <translation>Agente en la cola pero inválido</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="84"/>
        <source>Unlogged</source>
        <oldsource>Logged out</oldsource>
        <translation>Deslogueado</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="92"/>
        <source>Unpaused</source>
        <oldsource>Not paused</oldsource>
        <translation>No pausado</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="96"/>
        <source>Paused</source>
        <translation>Pausado</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="100"/>
        <source>Not relevant</source>
        <translation>No relevante</translation>
    </message>
</context>
</TS>
