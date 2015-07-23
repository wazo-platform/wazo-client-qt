<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="es_CL">
<context>
    <name>BaseEngine</name>
    <message>
        <location filename="src/baseengine.cpp" line="515"/>
        <source>Connection lost with XiVO CTI server</source>
        <translation>Pérdida de conexión con el servidor CTI de XiVO</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="814"/>
        <source>Could not modify the Services data.</source>
        <translation>No se puede modificar los datos del servicio.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="814"/>
        <source>Maybe Asterisk is down.</source>
        <translation>Probablemente Asterisk está caído.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1210"/>
        <source>Server has sent an Error.</source>
        <translation>El servidor ha enviado un error.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1214"/>
        <source>Your registration name &lt;%1@%2&gt; is not known by the XiVO CTI server on %3:%4.</source>
        <translation>Su nombre de registro  &lt;%1@%2&gt;  no es conocido por el servidor CTI de XiVO en %3:%4.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1218"/>
        <source>You entered a wrong login / password.</source>
        <translation>Ha ingresado un usuario o contraseña incorrectos.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1220"/>
        <source>You have no profile defined.</source>
        <translation>No tiene un perfil definido.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1224"/>
        <source>The server %1:%2 did not reply to the last keepalive packet.</source>
        <oldsource>The XiVO CTI server on %1:%2 did not reply to the last keepalive.</oldsource>
        <translation>El servidor CTI de XiVO en %1:%2 no ha respondido al último &apos;keepalive&apos;.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1229"/>
        <source>You defined an IP address %1 that is probably an unresolved host name.</source>
        <translation>Ha definido una dirección IP %1 que probablemente no tiene una resolución para el nombre de equipo.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1232"/>
        <source>Socket timeout (~ 60 s) : you probably attempted to reach, via a gateway, an IP address %1 that does not exist.</source>
        <translation>Tiempo exedido del socket (~ 60 s):  probablemente intentaste alcanzar, a través de una pasarela, una dirección IP %1 que no existe.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1236"/>
        <source>There seems to be a machine running on this IP address %1, and either no CTI server is running, or your port %2 is wrong.</source>
        <translation>Aparentemente existe una máquina corriendo en la dirección IP %1, sin embargo no existe un servidor CTI corriendo, o la puerta %2 es incorrecta.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1240"/>
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
        <location filename="src/baseengine.cpp" line="1246"/>
        <source>It seems that the server with IP address %1 does not accept encryption on its port %2. Please change either your port or your encryption setting.</source>
        <translation>Aparentemente el servidor en la dirección IP %1 no está aceptando encriptación en el puerto %2.  Por favor corrige el número de puerto o los parámetros de encriptación.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1250"/>
        <source>An unknown socket error has occured while attempting to join the IP address:port %1:%2.</source>
        <translation>Ha ocurrido un error desconocido en el socket mientras se intentaba unir a la dirección IP:puerto %1:%2.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1254"/>
        <source>An unmanaged (number %1) socket error has occured while attempting to join the IP address:port %1:%2.</source>
        <translation>Ha ocurrido un error de socket (número %1) no administrado mientras se intentaba unir la dirección IP.puerto %1:%2.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1259"/>
        <source>The XiVO CTI server on %1:%2 has just closed the connection.</source>
        <translation>El servidor CTI de XiVO en %1:%2 justo ha cerrado la conexión.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1263"/>
        <source>The XiVO CTI server on %1:%2 has just been stopped.</source>
        <translation>El servidor CTI de XiVO en %1:%2 justo ha sido detenido.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1266"/>
        <source>The XiVO CTI server on %1:%2 has just been reloaded.</source>
        <translation>El servidor CTI de XiVO en %1:%2 justo ha sido reiniciado.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1270"/>
        <source>You are already connected to %1:%2.</source>
        <oldsource>You are already connected from %1:%2.</oldsource>
        <translation>Ya se encuentra conectado desde %1:%2.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1272"/>
        <source>No capability allowed.</source>
        <translation>Sin la capacidad permitida.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1275"/>
        <source>Max number (%1) of XiVO Clients already reached.</source>
        <translation>Número máximo (%1) de clientes XiVO ya se ha alcanzado.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1277"/>
        <source>Missing Argument(s)</source>
        <translation>Argumento(s) perdido(s).</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1281"/>
        <source>Your client version (%1) is too old for this server.
Please upgrade it to %2 at least.</source>
        <translation>Su versión del cliente (%1) es demasiado antigua para éste servidor.
Por favor actualice a la versión %2 como mínimo.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1286"/>
        <source>Your client version (%1) is too old for this server.
Please upgrade it.</source>
        <translation>Su versión del cliente (%1) es demasiado antigua para éste servidor.
Por favor actualízece.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1293"/>
        <source>Your client&apos;s major version (%1)
is not the same as the server&apos;s (%2).</source>
        <translation>Su versión mayor (%1) del cliente
no es la misma que la del servidor (%2).</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1300"/>
        <source>Your server version (%1) is too old for this client.
Please upgrade it to %2 at least.</source>
        <translation>Su versión del servidor (%1) es demasiado antigua para éste cliente.
Por favor actualice a la versión %2 como mínimo.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1305"/>
        <source>Your server version (%1) is too old for this client.
Please upgrade it.</source>
        <translation>Su versión del servidor (%1) es demasiado antigua para éste cliente.
Por favor actualice.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1309"/>
        <source>You were disconnected by the server.</source>
        <translation>Ha sido desconectado por el servidor.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1311"/>
        <source>You were forced to disconnect by the server.</source>
        <translation>Ha sido desconectado forzosamente por el servidor.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1313"/>
        <source>Could not log agent: invalid extension.</source>
        <oldsource>Invalid extension number</oldsource>
        <translation>Número de extensión no válido.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1315"/>
        <source>Could not log agent: extension already in use.</source>
        <translation>No se puede logear el agente: La extensión ya se encuentra en uso.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1318"/>
        <source>Unreachable number: %1</source>
        <translation>Número inalcanzable: %1</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1322"/>
        <source>ERROR</source>
        <translation>ERROR</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1537"/>
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
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="37"/>
        <source>N/A</source>
        <translation>N/D</translation>
    </message>
    <message>
        <location filename="../src/xletlib/lookup_directory_entry.cpp" line="98"/>
        <source>Remote search result</source>
        <translation>Resultado de búsqueda remota</translation>
    </message>
    <message>
        <location filename="../src/xletlib/mobile_directory_entry.cpp" line="84"/>
        <source>Mobile phone</source>
        <translation>Teléfono móbil</translation>
    </message>
    <message>
        <location filename="../src/xletlib/current_filter_directory_entry.cpp" line="65"/>
        <source>Current Search</source>
        <translation>Búsqueda actual</translation>
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
<context>
    <name>XletAgents</name>
    <message>
        <location filename="../src/xlets/agents/agents.cpp" line="46"/>
        <source>Agents&apos; List (plain)</source>
        <translation>Lista de agentes (plana)</translation>
    </message>
</context>
<context>
    <name>QueueEntries</name>
    <message>
        <location filename="../src/xlets/queue_entries/queue_entries.cpp" line="50"/>
        <source>Calls of a Queue</source>
        <translation>Llamadas de una cola</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_entries/queue_entries.cpp" line="107"/>
        <source>&lt;b&gt;%1&lt;/b&gt; (%2): %3 call(s)</source>
        <translation>&lt;b&gt;%1&lt;/b&gt; (%2): %3 llamadas(s)</translation>
    </message>
</context>
<context>
    <name>History</name>
    <message>
        <location filename="../src/xlets/history/history.cpp" line="39"/>
        <source>History</source>
        <translation>Historial</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history.cpp" line="53"/>
        <source>All calls</source>
        <translation>Todas las llamadas</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history.cpp" line="54"/>
        <source>Sent calls</source>
        <translation>Llamadas realizadas</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history.cpp" line="55"/>
        <source>Received calls</source>
        <translation>Llamadas recibidas</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history.cpp" line="56"/>
        <source>Missed calls</source>
        <translation>Llamadas perdidas</translation>
    </message>
</context>
<context>
    <name>HistoryModel</name>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="41"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="42"/>
        <source>Number</source>
        <translation>Número</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="43"/>
        <source>Date</source>
        <translation>Fecha</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="44"/>
        <source>Duration</source>
        <translation>Duración</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="160"/>
        <source>%1 hr %2 min %3 s</source>
        <translation>%1 hr %2 min %3 s</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="162"/>
        <source>%1 min %2 s</source>
        <translation>%1 min %2 s</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="164"/>
        <source>%1 s</source>
        <translation>%1 s</translation>
    </message>
</context>
<context>
    <name>HistoryWidget</name>
    <message>
        <location filename="../src/xlets/history/history_widget.ui" line="54"/>
        <source>History</source>
        <translation>Historial</translation>
    </message>
</context>
<context>
    <name>ServicesPanel</name>
    <message>
        <location filename="../src/xlets/services/servicespanel.cpp" line="41"/>
        <source>Services</source>
        <translation>Servicios</translation>
    </message>
</context>
<context>
    <name>ServicesWidget</name>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="46"/>
        <source>Services</source>
        <translation>Servicios</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="72"/>
        <source>Call &amp;filtering</source>
        <translation>&amp;Filtro de llamadas</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="84"/>
        <source>Do not &amp;disturb</source>
        <translation>No &amp;molestar</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="114"/>
        <source>Call forwards</source>
        <translation>Llamadas transferidas</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="147"/>
        <source>Simple call forwards</source>
        <translation>Transferencias de llamadas simple</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="214"/>
        <source>&amp;Unconditional forward to</source>
        <translation>Transferir &amp;Incondicionalmente a</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="235"/>
        <source>No call forward</source>
        <translation>No transferir llamada</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="393"/>
        <source>Forward on &amp;busy to</source>
        <translation>Transferir cuando &amp;ocupado a</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="424"/>
        <source>Forward on &amp;no answer to</source>
        <translation>Transferir cuando %no contestado a</translation>
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
        <translation type="unfinished">Apellido</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="44"/>
        <source>First Name</source>
        <translation type="unfinished">Nombre</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="78"/>
        <source>Phone Number</source>
        <translation type="unfinished">Número de teĺefono</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="85"/>
        <source>Mobile Number</source>
        <translation type="unfinished">Número celular</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="105"/>
        <source>Company</source>
        <translation type="unfinished">Compañía</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="121"/>
        <source>Fax</source>
        <translation type="unfinished">Fax</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="128"/>
        <source>Email</source>
        <translation type="unfinished">Correo</translation>
    </message>
</context>
<context>
    <name>People</name>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="45"/>
        <source>People</source>
        <translation>Gente</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="56"/>
        <source>all</source>
        <translation>todo</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="57"/>
        <source>favorites</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="58"/>
        <source>my contacts</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="207"/>
        <source>Removing this contact</source>
        <translation type="unfinished">Removiendo éste contacto</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="208"/>
        <source>Removing this contact.
Are you sure ?</source>
        <translation type="unfinished">Removiendo éste contacto.
está seguro ?</translation>
    </message>
</context>
<context>
    <name>PeopleActions</name>
    <message>
        <location filename="../src/xlets/people/people_actions.cpp" line="50"/>
        <source>Call</source>
        <translation>Llamada</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_actions.cpp" line="56"/>
        <source>%1 - %2</source>
        <translation>%1  - %2</translation>
    </message>
</context>
<context>
    <name>PeopleEntryNumberDelegate</name>
    <message>
        <location filename="../src/xlets/people/people_entry_delegate.cpp" line="128"/>
        <source>CALL</source>
        <translation>LLAMADA</translation>
    </message>
</context>
<context>
    <name>PeopleWidget</name>
    <message>
        <location filename="../src/xlets/people/people_widget.ui" line="54"/>
        <source>Contacts</source>
        <translation>Contactos</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_widget.ui" line="105"/>
        <source>search</source>
        <translation>Buscar</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_widget.ui" line="130"/>
        <source>NEW CONTACT</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QueuesModel</name>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="42"/>
        <source>Number</source>
        <translation>Número</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="43"/>
        <source>Phone number of the queue</source>
        <translation>Número teléfonico de la cola</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="44"/>
        <source>Queues</source>
        <translation>Colas</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="45"/>
        <source>Queue names</source>
        <translation>Nombres de colas</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="46"/>
        <source>Waiting calls</source>
        <translation>Llamadas en espera</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="47"/>
        <source>Number of waiting calls</source>
        <translation>Número de llamadas en espera</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="48"/>
        <source>EWT</source>
        <translation>EWT</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="49"/>
        <source>Estimated Waiting Time</source>
        <translation>Tiempo de espera estimado</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="50"/>
        <source>Longest wait</source>
        <translation>Espera más larga</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="51"/>
        <source>Longest waiting call</source>
        <translation>Llamada en espera más larga </translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="52"/>
        <source>Logged in</source>
        <translation>Logueado</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="53"/>
        <source>Number of agents logged in</source>
        <translation>Número de agentes logueados</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="54"/>
        <source>Available</source>
        <translation>Disponible</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="55"/>
        <source>Number of agents ready to take a call</source>
        <translation>Número de agentes listos para tomar llamada</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="56"/>
        <source>Talking</source>
        <translation>Hablando</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="57"/>
        <source>Number of agents talking</source>
        <translation>Número de agentes hablando</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="58"/>
        <source>Received</source>
        <translation>Recibida</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="59"/>
        <source>Number of received calls</source>
        <translation>Número de llamadas recibidas</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="60"/>
        <source>Answered</source>
        <translation>Contestada</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="61"/>
        <source>Number of answered calls</source>
        <translation>Número de llamadas contestadas</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="62"/>
        <source>Abandoned</source>
        <translation>Abandonada</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="63"/>
        <source>Number of abandoned calls</source>
        <translation>Número de llamadas abandonadas</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="64"/>
        <source>Mean Waiting Time</source>
        <translation>tiempo promedio de espera</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="65"/>
        <source>Mean waiting time before getting an agent</source>
        <translation>Tiempo promedio de espera antes de contestar un agente</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="66"/>
        <source>Max Waiting Time</source>
        <translation>Tiempo de espera máximo</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="67"/>
        <source>Maximum waiting time before getting an agent</source>
        <translation>Tiempo máximo de espera antes de contestar un agente</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="68"/>
        <source>Efficiency</source>
        <translation>Eficeincia</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="69"/>
        <source>Ratio (Answered) / (Received)</source>
        <translation>Ratio (Contestadas) / (Recibidas)</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="70"/>
        <source>QOS</source>
        <translation>QOS</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="71"/>
        <source>Ratio (Calls answered in less than X sec / Number of calls answered)</source>
        <translation>Ratio (Llamadas contestadas en menos de X segs. / Número de llamadas contestadas)</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="257"/>
        <source>N/A</source>
        <translation>N/D</translation>
    </message>
</context>
<context>
    <name>XletQueues</name>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="54"/>
        <source>Queues</source>
        <translation>Colas</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="120"/>
        <source>Configure</source>
        <translation>Configurar</translation>
    </message>
</context>
<context>
    <name>XletQueuesConfigure</name>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="156"/>
        <source>Queues configuration</source>
        <translation>Configuración de colas</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="158"/>
        <source>Choose which queue should be displayed, and the
queues parameters for the Stats on slice:</source>
        <translation>Seleccione que cola debe ser mostrada y los
parámetros de cola para los estados en el panel deslizante</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="169"/>
        <source>&amp;Close</source>
        <translation>&amp;Cerrar</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="182"/>
        <source>Queue</source>
        <translation>Cola</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="183"/>
        <source>Qos - X (s)</source>
        <translation>Qos -X (s)</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="184"/>
        <source>This is the threshold in seconds to consider that the answer to a call was too late to be accounted as an answer of quality.</source>
        <translation>Este es el umbral en segundos para considerar que la respuesta a una llamada era demasiado tarde para ser contabilizado como una respuesta de calidad.</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="188"/>
        <source>Window (s)</source>
        <translation>Ventana(s)</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="189"/>
        <source>The window is the period of time used to compute the statistics</source>
        <translation>La ventana es el período de tiempo utilizado para calcular las estadísticas</translation>
    </message>
</context>
<context>
    <name>CurrentCall</name>
    <message>
        <location filename="../src/xlets/switchboard/current_call.cpp" line="50"/>
        <source>Attended T</source>
        <translation>Atendido T</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.cpp" line="51"/>
        <source>Call</source>
        <translation>Llamada</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.cpp" line="52"/>
        <source>Complete T</source>
        <translation>Completado T</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.cpp" line="53"/>
        <source>Hangup</source>
        <translation>Colgar</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.cpp" line="54"/>
        <source>Cancel</source>
        <translation>Cancelar</translation>
    </message>
</context>
<context>
    <name>CurrentCallWidget</name>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="14"/>
        <source>Form</source>
        <translation>Fromulario</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="66"/>
        <source>Current Call</source>
        <translation>Llamada actual</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="82"/>
        <source>Enter</source>
        <translation>Ingresar</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="98"/>
        <source>Answer</source>
        <translation>Contestar</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="147"/>
        <source>Call</source>
        <translation>Llamada</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="183"/>
        <source>Blind Transfer</source>
        <translation>Transferencia ciega</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="186"/>
        <source>Blind T</source>
        <translation>Ciego T</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="189"/>
        <source>F4</source>
        <translation>F4</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="222"/>
        <source>Attended Transfer</source>
        <translation>Transferencia atendida</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="225"/>
        <source>Attended T</source>
        <translation>Atendida T</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="228"/>
        <source>F5</source>
        <translation>F5</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="261"/>
        <source>Hold</source>
        <translation>Espera</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="264"/>
        <source>F7</source>
        <translation>F7</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="301"/>
        <source>Hangup</source>
        <translation>Colgar</translation>
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
        <translation>Centralita</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/xlets/switchboard/switchboard.cpp" line="323"/>
        <location filename="../src/xlets/switchboard/switchboard.cpp" line="333"/>
        <source>%n call(s)</source>
        <translation>
            <numerusform>%n llamada</numerusform>
            <numerusform>%n llamadas</numerusform>
        </translation>
    </message>
</context>
<context>
    <name>SwitchboardPanel</name>
    <message>
        <location filename="../src/xlets/switchboard/switchboard.ui" line="35"/>
        <source>Incoming Calls (F6)</source>
        <translation>Llamadas entrantes (F6)</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/switchboard.ui" line="62"/>
        <location filename="../src/xlets/switchboard/switchboard.ui" line="132"/>
        <source>0 call</source>
        <translation>0 llamadas</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/switchboard.ui" line="105"/>
        <source>Waiting Calls (F9)</source>
        <translation>Llamadas en espera (F9)</translation>
    </message>
</context>
<context>
    <name>DirDialog</name>
    <message>
        <location filename="../src/xlets/fax/dirdialog.cpp" line="36"/>
        <source>Directory</source>
        <translation>Directorio</translation>
    </message>
</context>
<context>
    <name>Fax</name>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="39"/>
        <source>Fax</source>
        <translation>Fax</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/xlets/fax/fax.cpp" line="78"/>
        <source>%n page(s) sent</source>
        <translation>
            <numerusform>%n página enviada</numerusform>
            <numerusform>%n páginas enviadas</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="82"/>
        <source>Failed to send</source>
        <translation>Error al enviar</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="98"/>
        <source>Open Fax File</source>
        <translation>Abrir archivo de ax</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="100"/>
        <source>PDF Files (*.pdf);;All Files (*)</source>
        <translation>Archivo PDF (*.pdf);; Todos los archivos (*)</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="119"/>
        <source>Missing file and fax number</source>
        <translation>Archivo y número de fax perdidos</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="122"/>
        <source>Missing file</source>
        <translation>Archivo perdido</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="125"/>
        <source>Missing fax number</source>
        <translation>Número de fax perdido</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="141"/>
        <source>File empty</source>
        <translation>Archivo vacío</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="144"/>
        <source>File not found</source>
        <translation>Archivo no encontrado</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="153"/>
        <source>Unreachable number</source>
        <translation>Número no alcanzable</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="162"/>
        <source>Sending...</source>
        <translation>Enviando...</translation>
    </message>
</context>
<context>
    <name>FaxWidget</name>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="43"/>
        <source>Send fax</source>
        <translation>Enviar fax</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="69"/>
        <source>1.</source>
        <translation>1.</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="84"/>
        <source>Choose file to send</source>
        <translation>Seleccione archivo a enviar</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="122"/>
        <source>file</source>
        <translation>archivo</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="134"/>
        <source>BROWSE...</source>
        <translation>EXPLORAR...</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="163"/>
        <source>2.</source>
        <translation>2.</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="178"/>
        <source>Choose destination number</source>
        <translation>Seleccione número de destino</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="216"/>
        <source>fax number</source>
        <translation>número de fax</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="286"/>
        <source>SEND FAX</source>
        <translation>ENVIAR FAX</translation>
    </message>
</context>
<context>
    <name>XletAgentDetails</name>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="49"/>
        <source>Agent Details</source>
        <translation>Detalles del agente</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="56"/>
        <source>Queues</source>
        <translation>Colas</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="57"/>
        <source>Joined</source>
        <translation>Unido</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="58"/>
        <source>Paused</source>
        <translation>Pausado</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="62"/>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="207"/>
        <source>Login</source>
        <translation>Ingreso</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="197"/>
        <source>on &lt;b&gt;%1&lt;/b&gt;</source>
        <translation>en &lt;b&gt;%1&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="203"/>
        <source>Logout</source>
        <translation>Salir</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="227"/>
        <source>Not in use</source>
        <translation>Sin uso</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="231"/>
        <source>In use</source>
        <translation>En uso</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="235"/>
        <source>Int. Incoming </source>
        <translation>Interna entrando</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="239"/>
        <source>Ext. Incoming</source>
        <translation>Externa entrando</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="243"/>
        <source>Int. Outgoing</source>
        <translation>Interna saliendo</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="247"/>
        <source>Ext. Outgoing</source>
        <translation>Externa saliendo</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="251"/>
        <source>Unlogged</source>
        <translation>Deslogueado</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="255"/>
        <source>%1 since %2</source>
        <translation>%1  desde %2</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="346"/>
        <source>Server: %1</source>
        <translation>Servidor: %1</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="347"/>
        <source>Context: %1</source>
        <translation>Contexto: %1</translation>
    </message>
</context>
<context>
    <name>AgentStatus</name>
    <message>
        <location filename="../src/xlets/agent_status_dashboard/agent_status.ui" line="21"/>
        <source>Form</source>
        <translation>Form</translation>
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
        <translation>Tablero de estado de agentes</translation>
    </message>
</context>
<context>
    <name>QueueMembersHeader</name>
    <message numerus="yes">
        <location filename="../src/xlets/queue_members/queue_members_header.cpp" line="109"/>
        <source>%n agent(s)</source>
        <translation>
            <numerusform>%n agente</numerusform>
            <numerusform>%n agentes</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../src/xlets/queue_members/queue_members_header.cpp" line="110"/>
        <source>%n phone(s)</source>
        <translation>
            <numerusform>%n teléfono</numerusform>
            <numerusform>%n teléfonos</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_header.cpp" line="111"/>
        <source>&lt;b&gt;%1&lt;/b&gt; (%2@%3) : %4 and %5</source>
        <translation>&lt;b&gt;%1&lt;/b&gt; (%2@%3) : %4 y %5</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_header.cpp" line="123"/>
        <source>No selected queue</source>
        <translation>Sin se ha seleccionado una cola</translation>
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
        <translation>Número</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="62"/>
        <source>Phone number of the queue</source>
        <translation>Número teléfonico de la cola</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="63"/>
        <source>Firstname</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="64"/>
        <source>Queue member&apos;s firstname</source>
        <translation>Nombre de los miembros de la cola</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="65"/>
        <source>Lastname</source>
        <translation>Apellido</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="66"/>
        <source>Queue member&apos;s lastname</source>
        <translation>Apellido de los miembros de la cola</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="67"/>
        <source>Logged in</source>
        <translation>Logueado</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="68"/>
        <source>Queue member&apos;s login status</source>
        <translation>Estado de logueo de los miembros de la cola</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="69"/>
        <source>Paused</source>
        <translation>Pausado</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="70"/>
        <source>Queue member&apos;s pause status</source>
        <translation>Estado de pausa de los miembros de la cola</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="71"/>
        <source>Answered calls</source>
        <translation>Llamadas contestadas</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="72"/>
        <source>Number of answered calls</source>
        <translation>Número de llamadas contestadas</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="73"/>
        <source>Last call</source>
        <translation>Última llamada</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="74"/>
        <source>Hangup date of the last answered call</source>
        <translation>Fecha de cuelgue de la última llamada contestada</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="75"/>
        <source>Penalty</source>
        <translation>Penalización</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="76"/>
        <source>Queue member&apos;s penalty</source>
        <translation>Penalización de los miembros de la cola</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="196"/>
        <source>Always</source>
        <translation>Siempre</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="198"/>
        <source>Never</source>
        <translation>Nunca</translation>
    </message>
</context>
<context>
    <name>XletQueueMembers</name>
    <message>
        <location filename="../src/xlets/queue_members/queue_members.cpp" line="47"/>
        <source>Queue Members</source>
        <translation>Miembros de la cola</translation>
    </message>
</context>
<context>
    <name>ConfigWidget</name>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="92"/>
        <source>System Language</source>
        <translation>Lenguaje del sistema</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="93"/>
        <source>Dutch</source>
        <translation>Holandés</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="94"/>
        <source>English</source>
        <translation>Inglés</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="95"/>
        <source>French</source>
        <translation>Francés</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="96"/>
        <source>Italian</source>
        <translation>Italiano</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="97"/>
        <source>Spanish (Chile)</source>
        <translation>Español (Chile)</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="98"/>
        <location filename="../src/config_widget/config_widget.cpp" line="99"/>
        <location filename="../src/config_widget/config_widget.cpp" line="100"/>
        <location filename="../src/config_widget/config_widget.cpp" line="101"/>
        <location filename="../src/config_widget/config_widget.cpp" line="102"/>
        <source>incomplete</source>
        <translation>incompleto</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="98"/>
        <source>German</source>
        <translation>Alemán</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="99"/>
        <source>Hungarian</source>
        <translation>Húngaro</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="100"/>
        <source>Japanese</source>
        <translation>Japonés</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="101"/>
        <source>Portuguese</source>
        <translation>Portugués</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="102"/>
        <source>Spanish</source>
        <translation>Español</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="20"/>
        <source>XiVO Client Configuration</source>
        <translation>Configuración del cliente XiVO</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="37"/>
        <source>User</source>
        <translation>Usuario</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="45"/>
        <source>Language</source>
        <translation>Lenguaje</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="80"/>
        <location filename="../src/config_widget/config_widget.ui" line="152"/>
        <location filename="../src/config_widget/config_widget.ui" line="166"/>
        <location filename="../src/config_widget/config_widget.ui" line="774"/>
        <source>You must restart the program for this setting to apply.</source>
        <translation>Debe reiniciar el programa para aplicar éstas configuraciones.</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="113"/>
        <source>Auto-reconnect </source>
        <translation>Auto-reconexión</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="120"/>
        <source>Connect at startup</source>
        <translation>Conectar al inicio</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="127"/>
        <source>Start systrayed</source>
        <translation>Iniciar en la barra de tareas</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="134"/>
        <source>Show agent options</source>
        <translation>Mostrar opciones del agente</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="155"/>
        <source>Show window when calling from external application</source>
        <translation>Mostrar ventana cuando se llama desde una aplicación externa</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="169"/>
        <source>Enable clipboard (GNU/Linux only)</source>
        <translation>Habilitar portapapeles (GNU/Linux solamente)</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="207"/>
        <source>Reset dock positions</source>
        <translation>Resetear posiciones de acoples</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="243"/>
        <source>Features</source>
        <translation>Características</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="267"/>
        <source>Presence</source>
        <translation>Presencia</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="273"/>
        <source>Presence reporting</source>
        <translation>Reporte de presencia</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="292"/>
        <source>Screen popup</source>
        <translation>Pantalla emergente</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="301"/>
        <source>Max screen popups</source>
        <translation>Número máximo de pantallas emergentes</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="321"/>
        <source>Screen Popup</source>
        <translation>Pantalla emergente</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="331"/>
        <source>Allow automatic URL opening</source>
        <translation>Permitir apertura automática de URL&apos;s</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="350"/>
        <source>History</source>
        <translation>Historial</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="359"/>
        <source>History size</source>
        <translation>Tamaño del historial</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="385"/>
        <source>Contacts</source>
        <translation>Contactos</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="391"/>
        <source>Maximum of contacts displayed</source>
        <translation>Máximo de contactos mostrados</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="420"/>
        <source>Queue list</source>
        <translation>Lista de la cola</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="428"/>
        <source>Show longest wait time</source>
        <translation>Mostrar la espera mas larga</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="442"/>
        <source>Orange</source>
        <translation>Naranja</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="452"/>
        <source>Red</source>
        <translation>Rojo</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="462"/>
        <source>Longest wait time thresholds</source>
        <translation>Umbrales de espera más largos</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="489"/>
        <source>Waiting calls thresholds</source>
        <translation>Umbral de llamadas de espera</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="516"/>
        <source>calls</source>
        <translation>llamadas</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="523"/>
        <source>seconds</source>
        <translation>segundos</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="554"/>
        <source>Queue Members</source>
        <translation>Miembros de la cola</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="560"/>
        <source>Hide unlogged agents</source>
        <translation>Ocultar agentes no logueados</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="576"/>
        <source>Switchboard</source>
        <translation>Centralita</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="585"/>
        <source>Switchboard call on hold queue name</source>
        <translation>Nombre de la cola de llamadas en espera en la centralita</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="595"/>
        <source>Switchboard queue name</source>
        <translation>Nombre de cola de la centralita</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="617"/>
        <source>Administration</source>
        <translation>Administracion</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="625"/>
        <source>Server</source>
        <translation>Servidor</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="639"/>
        <location filename="../src/config_widget/config_widget.ui" line="686"/>
        <source>Port</source>
        <translation>Puerto</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="672"/>
        <source>Backup server</source>
        <translation>Servidor de respaldo</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="761"/>
        <source>Advanced</source>
        <translation>Avanzado</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="767"/>
        <source>Display the configuration profile</source>
        <translation>Mostrar el perfil de configuracion</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="777"/>
        <source>Allow multiple instances</source>
        <translation>Permitir multiples instancias</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="788"/>
        <source>Auto-reconnect interval</source>
        <translation>Intervalo de auto-reconexion</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="827"/>
        <source>Keepalive interval</source>
        <translation>Intervalo de sobrevida</translation>
    </message>
</context>
<context>
    <name>ContactDialogOld</name>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/contactdialog.cpp" line="37"/>
        <source>Contact</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/contactdialog.cpp" line="40"/>
        <source>First Name</source>
        <translation type="unfinished">Nombre</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/contactdialog.cpp" line="42"/>
        <source>Last Name</source>
        <translation type="unfinished">Apellido</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/contactdialog.cpp" line="44"/>
        <source>Phone Number</source>
        <translation type="unfinished">Número de teĺefono</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/contactdialog.cpp" line="46"/>
        <source>Email Address</source>
        <translation type="unfinished">Dirección de correo</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/contactdialog.cpp" line="48"/>
        <source>Company</source>
        <translation type="unfinished">Compañía</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/contactdialog.cpp" line="50"/>
        <source>Fax Number</source>
        <translation type="unfinished">Número de Fax</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/contactdialog.cpp" line="52"/>
        <source>Mobile Number</source>
        <translation type="unfinished">Número celular</translation>
    </message>
</context>
<context>
    <name>CustomerInfoPanel</name>
    <message>
        <location filename="../src/xlets/customerinfo-builtin/customerinfopanel.cpp" line="35"/>
        <source>Sheets</source>
        <translation>Hojas</translation>
    </message>
</context>
<context>
    <name>LoginWidget</name>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="20"/>
        <source>Login Screen</source>
        <translation>Pantalla de ingreso</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="144"/>
        <source>LOGIN</source>
        <translation>Ingreso</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="271"/>
        <source>XiVO is a unified communication system that connects phones inside an organization with public and mobile telephone networks.</source>
        <translation>XiVO es un sistema de comunicaciones unificado que conecta teléfonos dentro de la organización con redes de teléfonos púbicos y móviles.</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="306"/>
        <source>login</source>
        <translation>ingreso</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="340"/>
        <source>password</source>
        <translation>contraseña</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="376"/>
        <source>Remember me</source>
        <translation>Recordarme</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="405"/>
        <source>No Agent</source>
        <translation>No hay agente</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="410"/>
        <source>Agent (logged off)</source>
        <translation>Agente (logueado)</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="415"/>
        <source>Agent (logged in)</source>
        <translation>Agente (deslogueado)</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="423"/>
        <source>phone</source>
        <translation>telefono</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="471"/>
        <source>CONNECT</source>
        <translation>CONECTAR</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../src/main_window/main_window.ui" line="17"/>
        <source>XiVO Client</source>
        <translation>Cliente XiVO</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="53"/>
        <source>&amp;XiVO Client</source>
        <translation>Cliente &amp;XiVO</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="66"/>
        <source>&amp;Help</source>
        <translation>&amp;Ayuda</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="88"/>
        <source>Confi&amp;gure</source>
        <translation>Confi&amp;gurar</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="91"/>
        <source>Configure account and connection options</source>
        <translation>Configurar las opciones de cuenta y conexión</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="99"/>
        <source>To S&amp;ystray</source>
        <translation>A la barra de tareas</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="102"/>
        <source>Enter the system tray</source>
        <translation>Ingresar la barra de tareas</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="107"/>
        <source>&amp;Connect</source>
        <translation>&amp;Conectar</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="110"/>
        <source>Connect to the server</source>
        <translation>Conectar al servidor</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="118"/>
        <source>&amp;Disconnect</source>
        <translation>&amp;Desconectar</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="121"/>
        <source>Disconnect from the server</source>
        <translation>Desconectar del servidor</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="129"/>
        <source>&amp;Quit</source>
        <translation>&amp;Salir</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="132"/>
        <source>Close the application</source>
        <translation>Cerrar la aplicación</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="135"/>
        <source>Ctrl+Q</source>
        <translation>Ctrl+Q</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="143"/>
        <source>&amp;Show window</source>
        <translation>&amp;Mostrar ventana</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="146"/>
        <source>Leave the system tray</source>
        <translation>Abandonar la barra de tareas</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="154"/>
        <source>&amp;About XiVO Client</source>
        <translation>&amp;Acerca del cliente XiVO</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="159"/>
        <source>About &amp;Qt</source>
        <translation>Acerca de &amp;Qt</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="164"/>
        <source>&amp;Credits</source>
        <translation>&amp;Creditos</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="106"/>
        <location filename="../src/main_window/main_window.cpp" line="229"/>
        <location filename="../src/main_window/main_window.cpp" line="376"/>
        <source>Client %1</source>
        <translation>Cliente %1</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="181"/>
        <source>XiVO CTI Error</source>
        <translation>Error CTI XiVO</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="211"/>
        <source>Client %1 (%2 profile)</source>
        <translation>Cliente %1 (perfil %2)</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="368"/>
        <source>Fetch the last one</source>
        <translation>Obtener la última</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="375"/>
        <source>About XiVO Client</source>
        <translation>Acerca del cliente XiVO</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="377"/>
        <source>The XiVO CTI applications, once connected to the proper server, complete the XiVO telephony system, by providing computer tools, such as customer information display, directory searches, or various supervision &apos;X-lets&apos; for call centers.</source>
        <translation>Las aplicaciones XiVO, una vez conectadas al servidor apropiado, completan el sistema telefónico, proveyendo herramientas de computador, como lo es el despliegue de la información de clientes, búsquedas en el directorio,  o varios &apos;X-lets&apos; de supervisión para callcenters.</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="382"/>
        <source>&lt;u&gt;Version Information&lt;/u&gt;</source>
        <translation>&lt;u&gt;Información de Versión&lt;/u&gt;</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="383"/>
        <source>Major : &lt;b&gt;%1&lt;/b&gt; (XiVO branch)</source>
        <translation>Mayor : &lt;b&gt;%1&lt;/b&gt; (XiVO derivado)</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="384"/>
        <source>Minor : git hash &lt;b&gt;%1&lt;/b&gt; from : %2</source>
        <translation>Menor : git hash &lt;b&gt;%1&lt;/b&gt; desde : %2</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="387"/>
        <source>(Application Built on : %1)</source>
        <translation>(Aplicación compilada el : %1)</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="388"/>
        <source>(Application Launched on : %1)</source>
        <translation>(Aplicación lanzada el : %1)</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="389"/>
        <source>(Config File Location : %1)</source>
        <translation>(Ubicación del archivo de configuración : %1)</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="392"/>
        <source>E-mail : </source>
        <translation>Correo:</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="393"/>
        <source>Phone : </source>
        <translation>Teléfono:</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="394"/>
        <source>Authors : </source>
        <translation>Autores:</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="394"/>
        <source>Avencall Development Team</source>
        <translation>Equipo de desarrollo de Avencall</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="396"/>
        <source>License : </source>
        <translation>Licencia:</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="398"/>
        <source>with a special exception allowing you to link it with OpenSSL, under some conditions.</source>
        <translation>con una excepción especial se permite enlazar con OpenSSL, bajo algunas condiciones.</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="404"/>
        <source>Credits</source>
        <translation>Créditos</translation>
    </message>
</context>
<context>
    <name>MyLocalDirPanel</name>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="54"/>
        <source>Personal Directory</source>
        <translation>Directorio personal</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="70"/>
        <source>&amp;New Contact</source>
        <translation>&amp;Nuevo contacto</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="74"/>
        <source>&amp;Export Contacts</source>
        <translation>&amp;Exportar contactos</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="78"/>
        <source>&amp;Import Contacts</source>
        <translation>&amp;Importar contactos</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="82"/>
        <source>&amp;Search</source>
        <translation>&amp;Buscar</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="86"/>
        <source>&amp;Remove all Contacts</source>
        <translation>&amp;Remover todos los contactos</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="96"/>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="261"/>
        <source>First Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="97"/>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="265"/>
        <source>Last Name</source>
        <translation>Apellido</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="98"/>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="270"/>
        <source>Phone Number</source>
        <translation>Número de teĺefono</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="99"/>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="277"/>
        <source>Email Address</source>
        <translation>Dirección de correo</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="100"/>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="287"/>
        <source>Company</source>
        <translation>Compañía</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="101"/>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="292"/>
        <source>Fax Number</source>
        <translation>Número de Fax</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="102"/>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="299"/>
        <source>Mobile Number</source>
        <translation>Número celular</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="188"/>
        <source>Open Contacts File</source>
        <translation>Abrir archivo de contactos</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="190"/>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="206"/>
        <source>CSV file (*.csv)</source>
        <translation>Archivo CSV (*.csv)</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="204"/>
        <source>Save Contacts File</source>
        <translation>Grabar archivo de contactos</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="272"/>
        <source>Number</source>
        <translation>Número</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="278"/>
        <source>E-mail Address</source>
        <translation>Dirección de correo</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="279"/>
        <source>Email</source>
        <translation>Correo</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="293"/>
        <source>Fax</source>
        <translation>Fax</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="300"/>
        <source>Mobile</source>
        <translation>Celular</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="388"/>
        <source>Removing all contacts</source>
        <translation>Remover todos los contactos</translation>
    </message>
    <message>
        <location filename="../src/xlets/mylocaldir-builtin/mylocaldirpanel.cpp" line="389"/>
        <source>Removing all contacts.
Are you sure ?</source>
        <translation>Removiendo todos los contactos.
está seguro ?</translation>
    </message>
</context>
<context>
    <name>RemoteDirectory</name>
    <message>
        <location filename="../src/xlets/remotedirectory-builtin/remote_directory.cpp" line="35"/>
        <source>Remote Directory</source>
        <translation>Directorio remoto</translation>
    </message>
</context>
<context>
    <name>SearchPanel</name>
    <message>
        <location filename="../src/xlets/search-builtin/searchpanel.cpp" line="45"/>
        <source>Contacts</source>
        <translation>Contactos</translation>
    </message>
    <message>
        <location filename="../src/xlets/search-builtin/searchpanel.cpp" line="52"/>
        <source>N&amp;ame or number to search :</source>
        <translation>N&amp;ombre o número a buscar:</translation>
    </message>
</context>
<context>
    <name>Statusbar</name>
    <message>
        <location filename="../src/main_window/statusbar.cpp" line="78"/>
        <source>Connected</source>
        <translation>Conectado</translation>
    </message>
    <message>
        <location filename="../src/main_window/statusbar.cpp" line="84"/>
        <source>Disconnected</source>
        <translation>Desconectado</translation>
    </message>
</context>
<context>
    <name>loading_dialog</name>
    <message>
        <location filename="../src/main_window/loading_dialog.ui" line="17"/>
        <source>Loading Screen</source>
        <translation>Cargando pantalla</translation>
    </message>
    <message>
        <location filename="../src/main_window/loading_dialog.ui" line="34"/>
        <source>Loading...</source>
        <translation>Cargando...</translation>
    </message>
</context>
<context>
    <name>Conference</name>
    <message>
        <location filename="../src/xlets/conference/conference.cpp" line="42"/>
        <source>Conference</source>
        <translation>Conferencia</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference.cpp" line="50"/>
        <source>Room list</source>
        <translation>Lista de cuartos</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference.cpp" line="145"/>
        <source>%1 (%2)</source>
        <translation>%1 (%2)</translation>
    </message>
</context>
<context>
    <name>ConferenceListModel</name>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="37"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="38"/>
        <source>Number</source>
        <translation>Número</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="39"/>
        <source>PIN code</source>
        <translation>Código PIN</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="40"/>
        <source>Member count</source>
        <translation>Contador del miembro</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="41"/>
        <source>Started since</source>
        <translation>Iniciado desde</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="107"/>
        <source>Yes</source>
        <translation>Si</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="109"/>
        <source>No</source>
        <translation>No</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="137"/>
        <source>Not started</source>
        <translation>No iniciado</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="139"/>
        <source>Unknown</source>
        <translation>Desconocido</translation>
    </message>
</context>
<context>
    <name>ConferenceRoomModel</name>
    <message>
        <location filename="../src/xlets/conference/conference_room_model.cpp" line="42"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_room_model.cpp" line="43"/>
        <source>Number</source>
        <translation>Número</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_room_model.cpp" line="44"/>
        <source>Since</source>
        <translation>Desde</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_room_model.cpp" line="158"/>
        <source>Mute/UnMute</source>
        <translation>Mudo/NoMudo</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_room_model.cpp" line="174"/>
        <source>Unknown</source>
        <translation>Desconocido</translation>
    </message>
</context>
<context>
    <name>ConferenceWidget</name>
    <message>
        <location filename="../src/xlets/conference/conference_widget.ui" line="54"/>
        <source>Conference</source>
        <translation>Conferencia</translation>
    </message>
</context>
<context>
    <name>AgentsModel</name>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="43"/>
        <source>Number</source>
        <translation>Número</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="44"/>
        <source>First name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="45"/>
        <source>Last name</source>
        <translation>Apellido</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="46"/>
        <location filename="../src/xletlib/agents_model.cpp" line="200"/>
        <source>Listen</source>
        <translation>Escuchar</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="47"/>
        <source>Status since</source>
        <translation>Estado desde</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="48"/>
        <source>Logged in</source>
        <translation>Logueado</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="49"/>
        <source>Joined
queues</source>
        <translation>Unido
colas</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="50"/>
        <location filename="../src/xletlib/agents_model.cpp" line="411"/>
        <source>Paused</source>
        <translation>Pausado</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="51"/>
        <source>Paused
queues</source>
        <translation>Pausada
colas</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="268"/>
        <source>Agent ready to receive a call</source>
        <translation>Agente listo para recibir llamada</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="270"/>
        <source>Agent processing a call or paused</source>
        <translation>Agente procesando llamada o pausado</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="272"/>
        <source>Agent receiving an internal call out of queue</source>
        <translation>Agente recibiendo una llamada interna fuera de la cola</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="274"/>
        <source>Agent receiving an external call out of queue</source>
        <translation>Agente recibiendo una llamada externa fuera de la cola</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="276"/>
        <source>Agent emitting an internal call</source>
        <translation>Agente realizando una llamada interna</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="278"/>
        <source>Agent emitting an external call</source>
        <translation>Agente realizando una llamada externa</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="334"/>
        <source>Not in use</source>
        <translation>Sin uso</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="336"/>
        <source>In use</source>
        <translation>En uso</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="338"/>
        <source>Int. Incoming</source>
        <translation>Interna entrando</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="340"/>
        <source>Ext. Incoming</source>
        <translation>Externa entrando</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="342"/>
        <source>Int. Outgoing</source>
        <translation>Interna saliendo</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="344"/>
        <source>Ext. Outgoing</source>
        <translation>Externa saliendo</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="390"/>
        <source>Logged</source>
        <translation>Logueado</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="392"/>
        <source>Unlogged</source>
        <translation>Deslogueado</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="409"/>
        <source>Unpaused</source>
        <translation>Despausado</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="413"/>
        <source>Partially Paused</source>
        <translation>Parcialmente pausado</translation>
    </message>
</context>
<context>
    <name>BasePeerWidget</name>
    <message>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="68"/>
        <source>&amp;Open a chat window</source>
        <translation>&amp;Abrir una ventana de chat</translation>
    </message>
    <message>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="69"/>
        <source>Open a chat window with this user</source>
        <translation>Abrir una ventana de chat con éste usuario</translation>
    </message>
    <message>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="75"/>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="399"/>
        <source>&amp;Intercept</source>
        <translation>&amp;Interceptar</translation>
    </message>
    <message>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="76"/>
        <source>Intercept call</source>
        <translation>Interceptar llamada</translation>
    </message>
    <message>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="319"/>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="341"/>
        <source>&amp;Call</source>
        <translation>&amp;Llamada</translation>
    </message>
    <message>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="351"/>
        <source>Call &amp;mobile</source>
        <translation>Llamar a celular</translation>
    </message>
    <message>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="369"/>
        <source>&amp;Edit</source>
        <translation>Editar</translation>
    </message>
    <message>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="384"/>
        <source>&amp;Hangup call</source>
        <translation>Colgar llamada</translation>
    </message>
    <message>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="399"/>
        <source>Cancel transfer</source>
        <translation>Cancelar transferencia</translation>
    </message>
    <message>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="415"/>
        <source>Invite in conference room</source>
        <translation>Invitar en sala de conferencia</translation>
    </message>
    <message>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="443"/>
        <source>Blind &amp;Transfer</source>
        <translation>&amp;Transferencia ciega</translation>
    </message>
    <message>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="443"/>
        <source>&amp;Attended Transfer</source>
        <translation>Transferencia &amp;atendida</translation>
    </message>
    <message>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="519"/>
        <source>Blind transfer &lt;%0&gt;</source>
        <translation>Transferencia ciega &lt;%0&gt;</translation>
    </message>
    <message>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="536"/>
        <source>Attended transfer &lt;%0&gt;</source>
        <translation>Transferencia atendida &lt;%0&gt;</translation>
    </message>
    <message>
        <location filename="../src/xletlib/basepeerwidget.cpp" line="562"/>
        <source>Transfer to &amp;voice mail</source>
        <translation>Transferir a &amp;correo de voz</translation>
    </message>
</context>
<context>
    <name>ChitChatWindow</name>
    <message>
        <location filename="../src/xletlib/chitchat.cpp" line="83"/>
        <source>&amp;Clear history</source>
        <translation>&amp;Limpiar historial</translation>
    </message>
    <message>
        <location filename="../src/xletlib/chitchat.cpp" line="84"/>
        <source>&amp;Send</source>
        <translation>&amp;Enviar</translation>
    </message>
    <message>
        <location filename="../src/xletlib/chitchat.cpp" line="101"/>
        <source>chitchat - %1 (%2)</source>
        <translation>chitchat - %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/xletlib/chitchat.cpp" line="141"/>
        <location filename="../src/xletlib/chitchat.cpp" line="184"/>
        <source>chat window opened with &quot;%1&quot; (%2)</source>
        <translation>ventana de chat abierta con &quot;%1&quot; (%2)</translation>
    </message>
    <message>
        <location filename="../src/xletlib/chitchat.cpp" line="158"/>
        <source>you said: </source>
        <translation>usted dijo:</translation>
    </message>
    <message>
        <location filename="../src/xletlib/chitchat.cpp" line="187"/>
        <source>system: </source>
        <translation>sistema:</translation>
    </message>
</context>
<context>
    <name>DirectoryPanel</name>
    <message>
        <location filename="../src/xletlib/directorypanel.cpp" line="37"/>
        <source>Remote Directory Panel</source>
        <translation>Panel de directorio remoto</translation>
    </message>
    <message>
        <location filename="../src/xletlib/directorypanel.cpp" line="40"/>
        <source>Remote Di&amp;rectory</source>
        <translation>Directo&amp;rio Remoto </translation>
    </message>
    <message>
        <location filename="../src/xletlib/directorypanel.cpp" line="48"/>
        <source>Search</source>
        <translation>Buscar</translation>
    </message>
    <message>
        <location filename="../src/xletlib/directorypanel.cpp" line="131"/>
        <source>Double-click to send an E-mail to</source>
        <translation>Doble click para enviar un correo a</translation>
    </message>
    <message>
        <location filename="../src/xletlib/directorypanel.cpp" line="133"/>
        <source>Double-click to call</source>
        <translation>Doble click para llamar</translation>
    </message>
</context>
<context>
    <name>ExtendedTableWidget</name>
    <message>
        <location filename="../src/xletlib/extendedtablewidget.cpp" line="73"/>
        <source>&amp;Remove</source>
        <translation>&amp;Remover</translation>
    </message>
    <message>
        <location filename="../src/xletlib/extendedtablewidget.cpp" line="77"/>
        <source>&amp;Dial</source>
        <translation>&amp;Marcar</translation>
    </message>
    <message>
        <location filename="../src/xletlib/extendedtablewidget.cpp" line="80"/>
        <source>Send an E-mail</source>
        <translation>Enviar un Correo</translation>
    </message>
    <message>
        <location filename="../src/xletlib/extendedtablewidget.cpp" line="174"/>
        <source>Removing this contact</source>
        <translation>Removiendo éste contacto</translation>
    </message>
    <message>
        <location filename="../src/xletlib/extendedtablewidget.cpp" line="175"/>
        <source>Removing this contact.
Are you sure ?</source>
        <translation>Removiendo éste contacto.
está seguro ?</translation>
    </message>
</context>
<context>
    <name>ExternalPhoneDialog</name>
    <message>
        <location filename="../src/xletlib/externalphonedialog.cpp" line="46"/>
        <source>External phone number</source>
        <translation>Número de teléfono externo</translation>
    </message>
    <message>
        <location filename="../src/xletlib/externalphonedialog.cpp" line="49"/>
        <source>Label</source>
        <translation>Etiqueta</translation>
    </message>
    <message>
        <location filename="../src/xletlib/externalphonedialog.cpp" line="51"/>
        <source>Phone number</source>
        <translation>Número de teléfono</translation>
    </message>
</context>
<context>
    <name>ExternalPhonePeerWidget</name>
    <message>
        <location filename="../src/xletlib/externalphonepeerwidget.cpp" line="45"/>
        <source>Label : %1
Phone Number : %2</source>
        <translation>Etiqueta: %!
Número de teléfono: %2</translation>
    </message>
</context>
<context>
    <name>NumberButtonDelegate</name>
    <message>
        <location filename="../src/xletlib/number_button_delegate.cpp" line="68"/>
        <source>CALL</source>
        <translation>LLAMADA</translation>
    </message>
</context>
<context>
    <name>PeerWidget</name>
    <message>
        <location filename="../src/xletlib/peerwidget.cpp" line="237"/>
        <source>Agent Number : %1
In Queues : %2</source>
        <translation>Agente número: %1
En colas: %2</translation>
    </message>
    <message>
        <location filename="../src/xletlib/peerwidget.cpp" line="248"/>
        <source>Mobile number : %1</source>
        <translation>Número de celular: %1</translation>
    </message>
    <message>
        <location filename="../src/xletlib/peerwidget.cpp" line="262"/>
        <source>User Name: %1
IPBXid: %2
Status: %3</source>
        <translation>Nombre de usuario: %1
id IPBX: %2
Estado: %3</translation>
    </message>
    <message>
        <location filename="../src/xletlib/peerwidget.cpp" line="317"/>
        <source>Status:%1</source>
        <translation>Estado: %1</translation>
    </message>
    <message>
        <location filename="../src/xletlib/peerwidget.cpp" line="320"/>
        <source>No status (no phone number)</source>
        <translation>Sin estado (no hay numero de teléfono)</translation>
    </message>
    <message>
        <location filename="../src/xletlib/peerwidget.cpp" line="330"/>
        <source>&lt;EMPTY&gt;</source>
        <translation>&lt;VACÍO&gt;</translation>
    </message>
    <message>
        <location filename="../src/xletlib/peerwidget.cpp" line="331"/>
        <source>Phone Number: %1
Status: %2
Busy lines: %3</source>
        <translation>Número de teléfono: %1
Estado: %2
Líneas ocupadas: %3</translation>
    </message>
    <message>
        <location filename="../src/xletlib/peerwidget.cpp" line="352"/>
        <source>(No callerid yet)</source>
        <translation>(Sin identificador de llamante aún)</translation>
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
        <translation>Posición</translation>
    </message>
    <message>
        <location filename="../src/xletlib/queue_entries/queue_entries_model.cpp" line="56"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location filename="../src/xletlib/queue_entries/queue_entries_model.cpp" line="57"/>
        <source>Number</source>
        <translation>Número</translation>
    </message>
    <message>
        <location filename="../src/xletlib/queue_entries/queue_entries_model.cpp" line="58"/>
        <source>Time</source>
        <translation>Hora</translation>
    </message>
</context>
<context>
    <name>SearchDialog</name>
    <message>
        <location filename="../src/xletlib/searchdialog.cpp" line="40"/>
        <source>Search :</source>
        <translation>Buscar</translation>
    </message>
</context>
<context>
    <name>XletDatetime</name>
    <message>
        <location filename="../src/xlets/datetime/datetime.cpp" line="42"/>
        <source>Date and Time</source>
        <translation>Fecha y Hora</translation>
    </message>
</context>
<context>
    <name>Directory</name>
    <message>
        <location filename="../src/xlets/directory/directory.cpp" line="44"/>
        <source>Directory</source>
        <translation>Directorio</translation>
    </message>
</context>
<context>
    <name>DirectoryWidget</name>
    <message>
        <location filename="../src/xlets/directory/directory_widget.ui" line="14"/>
        <source>Form</source>
        <translation>Formulario</translation>
    </message>
</context>
<context>
    <name>IdentityDisplay</name>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="53"/>
        <source>Identity</source>
        <translation>Identidad</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="60"/>
        <source>Current User Panel</source>
        <translation>Panel de usuario actual</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="98"/>
        <source>Login</source>
        <translation>Ingreso</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="102"/>
        <source>Logout</source>
        <translation>Salir</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="110"/>
        <source>Pause</source>
        <translation>Pausar</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="114"/>
        <source>Unpause</source>
        <translation>Despausar</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="378"/>
        <source>DND</source>
        <translation>DND</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="379"/>
        <source>Do Not Disturb</source>
        <translation>No molestar</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="381"/>
        <source>UNC %1</source>
        <translation>UNC %1</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="382"/>
        <source>Unconditional Forward towards %1</source>
        <translation>Transferir Incondicionalmente hacia %1</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="384"/>
        <source>BUSY %1</source>
        <translation>OCUPADO %1</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="385"/>
        <source>Busy Forward towards %1</source>
        <translation>&apos;Sin contestar&apos; trasnferir hacia %1</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="387"/>
        <source>FNA %1</source>
        <translation>FNA %1</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="388"/>
        <source>Non-Answer Forward towards %1</source>
        <translation>&apos;Sin contestar&apos; trasnferir hacia %1</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="390"/>
        <source>CALL FILTER</source>
        <translation>FILTRO DE LLAMADAS</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="393"/>
        <source>AVAILABLE</source>
        <translation>DISPONIBLE</translation>
    </message>
</context>
<context>
    <name>IdentityWidget</name>
    <message>
        <location filename="../src/xlets/identity/identity.ui" line="167"/>
        <source>VOICEMAIL</source>
        <translation>BUZON DE VOZ</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.ui" line="205"/>
        <source>Call your voicemail</source>
        <translation>Llamar a su buzón de voz</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.ui" line="364"/>
        <source>call</source>
        <translation>llamada</translation>
    </message>
</context>
</TS>
