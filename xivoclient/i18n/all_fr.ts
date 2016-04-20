<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="fr">
<context>
    <name>BaseEngine</name>
    <message>
        <location filename="src/baseengine.cpp" line="505"/>
        <source>Connection lost with XiVO CTI server</source>
        <translation>Connexion au server CTI interrompue</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="766"/>
        <source>Could not modify the Services data.</source>
        <translation>Impossible de modifier les données pour les Services.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="766"/>
        <source>Maybe Asterisk is down.</source>
        <translation>Asterisk est peut-être arrêté.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1103"/>
        <source>Failed to start a secure connection.</source>
        <translation>Impossible d&apos;établir une connexion sécurisée</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1104"/>
        <source>Do you want to disable secure connections?</source>
        <translation>Voulez-vous désactiver les connexions sécurisées?</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1128"/>
        <source>Server has sent an Error.</source>
        <translation>Le serveur a renvoyé une Erreur.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1134"/>
        <source>Your registration name &lt;%1@%2&gt; is not known by the XiVO CTI server on %3:%4.</source>
        <translation>Votre nom &lt;%1@%2&gt; est inconnu par le serveur CTI XiVO sur %3:%4.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1139"/>
        <source>You entered a wrong login / password.</source>
        <translation>Vous avez fourni un mauvais login / password.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1142"/>
        <source>You have no profile defined.</source>
        <translation>Votre profil n&apos;est pas défini.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1146"/>
        <source>The server %1:%2 did not reply to the last keepalive packet.</source>
        <oldsource>The XiVO CTI server on %1:%2 did not reply to the last keepalive.</oldsource>
        <translation>Le serveur %1:%2 n&apos;a pas répondu au paquet keepalive.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1152"/>
        <source>You defined an IP address %1 that is probably an unresolved host name.</source>
        <translation>Vous avez défini une adresse IP %1 qui est probablement un nom d&apos;hôte non résolu.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1156"/>
        <source>Socket timeout (~ 60 s) : you probably attempted to reach, via a gateway, an IP address %1 that does not exist.</source>
        <translation>Timeout de socket (~ 60 s) : vous avez probablement essayé de joindre, via une passerelle, une adresse IP %1 qui n&apos;existe pas.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1161"/>
        <source>There seems to be a machine running on this IP address %1, and either no CTI server is running, or your port %2 is wrong.</source>
        <translation>Il semble qu&apos;une machine soit présente à cette adresse IP %1, mais il n&apos;y a pas de serveur CTI, ou votre port %2 est faux.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1166"/>
        <source>An error occurred on the network while attempting to join the IP address %1 :
- no external route defined to access this IP address (~ no timeout)
- this IP address is routed but there is no machine (~ 5 s timeout)
- a cable has been unplugged on your LAN on the way to this IP address (~ 30 s timeout).</source>
        <translation>Une erreur s&apos;est produite sur le réseau en essayant de joindre l&apos;adresse IP %1 :
- aucune route externe n&apos;est définie pour accéder à cette adresse IP (~ pas de timeout)
- cette adresse IP est routée mais il n&apos;y a pas de machine (~ 5 s de timeout)
- un câble a été débranché sur votre réseau local, sur le chemin vers cette adresse IP (~ 30 s de timeout).</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1173"/>
        <source>It seems that the server with IP address %1 does not accept encryption on its port %2. Please change either your port or your encryption setting.</source>
        <translation>Il semble que le serveur à l&apos;adresse IP %1 n&apos;accepte pas le chiffrage sur son port %2. Changez soit le port soit la configuration de chiffrage.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1178"/>
        <source>An unknown socket error has occured while attempting to join the IP address:port %1:%2.</source>
        <translation>Une erreur de socket inconnue s&apos;est produite en essayant de joindre l&apos;IP %1:%2 (adresse:port).</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1183"/>
        <source>An unmanaged (number %1) socket error has occured while attempting to join the IP address:port %1:%2.</source>
        <translation>Une erreur de socket non gérée (numéro %1) s&apos;est produite en essayant de joindre l&apos;IP %1:%2 (adresse:port).</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1188"/>
        <source>The XiVO CTI server on %1:%2 has just closed the connection.</source>
        <translation>Le serveur CTI XiVO sur %1:%2 vient de fermer la connexion.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1192"/>
        <source>The XiVO CTI server on %1:%2 has just been stopped.</source>
        <translation>Le serveur CTI XiVO sur %1:%2 vient d&apos;être arrêté.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1195"/>
        <source>The XiVO CTI server on %1:%2 has just been reloaded.</source>
        <translation>Le serveur CTI XiVO sur %1:%2 vient d&apos;être rechargé.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1199"/>
        <source>You are already connected to %1:%2.</source>
        <oldsource>You are already connected from %1:%2.</oldsource>
        <translation>Vous êtes déjà connecté à %1:%2.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1201"/>
        <source>No capability allowed.</source>
        <translation>Aucune capacité autorisée.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1204"/>
        <source>Max number (%1) of XiVO Clients already reached.</source>
        <translation>Le nombre maximal (%1) de XiVO Clients est déjà connecté.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1206"/>
        <source>Missing Argument(s)</source>
        <translation>Argument(s) manquant(s)</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1211"/>
        <source>Your client&apos;s protocol version (%1)
is not the same as the server&apos;s (%2).</source>
        <translation>La version du protocole de votre client (%1)
est différente de celle du serveur (%2).</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1219"/>
        <source>Your server version (%1) is too old for this client.
Please upgrade it to %2 at least.</source>
        <translation>La version du serveur (%1) ne correspond pas avec celle du client. Merci de mettre à jour votre client à la version (%2) ou supérieur.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1224"/>
        <source>Your server version (%1) is too old for this client.
Please upgrade it.</source>
        <translation>La version du serveur (%1) ne correspond pas avec celle du client. Merci de mettre à jour votre client.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1228"/>
        <source>You were disconnected by the server.</source>
        <translation>Vous avez été déconnecté par le serveur.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1230"/>
        <source>You were forced to disconnect by the server.</source>
        <translation>Vous avez été déconnecté de force par le serveur.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1232"/>
        <source>Could not log agent: invalid extension.</source>
        <oldsource>Invalid extension number</oldsource>
        <translation>Impossible de loguer l&apos;agent: extension invalide.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1234"/>
        <source>Could not log agent: extension already in use.</source>
        <translation>Impossible de loguer l&apos;agent: extension déjà utilisée.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1237"/>
        <source>Unreachable number: %1</source>
        <translation>Numéro %1 injoignable</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1239"/>
        <source>The authentication server could not fulfill your request.</source>
        <translation>Le serveur d&apos;authentification a rencontré une erreur.</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1248"/>
        <source>ERROR</source>
        <translation>ERREUR</translation>
    </message>
    <message>
        <location filename="src/baseengine.cpp" line="1404"/>
        <source>Attempting to reconnect to server</source>
        <translation>Tentative de reconnexion au serveur</translation>
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
        <translation>Téléphone</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="37"/>
        <source>N/A</source>
        <translation>N/D</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_action_generator.cpp" line="342"/>
        <source>Copy</source>
        <translation>Copier</translation>
    </message>
    <message>
        <location filename="../src/xletlib/lookup_directory_entry.cpp" line="98"/>
        <source>Remote search result</source>
        <translation>Résultat d&apos;une recherche externe</translation>
    </message>
    <message>
        <location filename="../src/xletlib/mobile_directory_entry.cpp" line="84"/>
        <source>Mobile phone</source>
        <translation>Mobile</translation>
    </message>
    <message>
        <location filename="../src/xletlib/current_filter_directory_entry.cpp" line="65"/>
        <source>Current Search</source>
        <translation>Recherche en cours</translation>
    </message>
</context>
<context>
    <name>QueueAgentStatus</name>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="51"/>
        <source>Dynamic membership</source>
        <translation>Adhésion dynamique</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="55"/>
        <source>Static/RT membership</source>
        <translation>Adhésion statique/RT</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="67"/>
        <source>Agent not in Queue</source>
        <translation>Agent pas dans la File</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="71"/>
        <location filename="src/storage/queue_agent_status.cpp" line="83"/>
        <source>Agent in Queue</source>
        <translation>Agent dans la File</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="72"/>
        <location filename="src/storage/queue_agent_status.cpp" line="76"/>
        <source>Logged</source>
        <oldsource>Logged in</oldsource>
        <translation>Logué</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="75"/>
        <source>Agent Called or Busy</source>
        <translation>Agent Appelé ou Occupé</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="79"/>
        <source>Agent in Queue but Invalid</source>
        <translation>Agent dans la File mais Invalide</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="84"/>
        <source>Unlogged</source>
        <oldsource>Logged out</oldsource>
        <translation>Délogué</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="92"/>
        <source>Unpaused</source>
        <oldsource>Not paused</oldsource>
        <translation>Prêt</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="96"/>
        <source>Paused</source>
        <translation>En retrait</translation>
    </message>
    <message>
        <location filename="src/storage/queue_agent_status.cpp" line="100"/>
        <source>Not relevant</source>
        <translation>Non pertinent</translation>
    </message>
</context>
<context>
    <name>QueueEntries</name>
    <message>
        <location filename="../src/xlets/queue_entries/queue_entries.cpp" line="50"/>
        <source>Calls of a Queue</source>
        <translation>Appels d&apos;une file d&apos;attente</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_entries/queue_entries.cpp" line="107"/>
        <source>&lt;b&gt;%1&lt;/b&gt; (%2): %3 call(s)</source>
        <translation>&lt;b&gt;%1&lt;/b&gt; (%2): %3 appel(s)</translation>
    </message>
</context>
<context>
    <name>XletAgents</name>
    <message>
        <location filename="../src/xlets/agents/agents.cpp" line="46"/>
        <source>Agents&apos; List (plain)</source>
        <translation>Liste des Agents (classique)</translation>
    </message>
</context>
<context>
    <name>QueueMembersHeader</name>
    <message numerus="yes">
        <location filename="../src/xlets/queue_members/queue_members_header.cpp" line="109"/>
        <source>%n agent(s)</source>
        <translation>
            <numerusform>%n agent</numerusform>
            <numerusform>%n agents</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location filename="../src/xlets/queue_members/queue_members_header.cpp" line="110"/>
        <source>%n phone(s)</source>
        <translation>
            <numerusform>%n téléphone</numerusform>
            <numerusform>%n téléphones</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_header.cpp" line="111"/>
        <source>&lt;b&gt;%1&lt;/b&gt; (%2@%3) : %4 and %5</source>
        <translation>&lt;b&gt;%1&lt;/b&gt; (%2@%3) : %4 et %5</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_header.cpp" line="123"/>
        <source>No selected queue</source>
        <translation>Aucune file d&apos;attente sélectionnée</translation>
    </message>
</context>
<context>
    <name>QueueMembersModel</name>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="59"/>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="60"/>
        <source>ID</source>
        <translation>Identifiant</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="61"/>
        <source>Number</source>
        <translation>Numéro</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="62"/>
        <source>Phone number of the queue</source>
        <translation>Numéro de la file</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="63"/>
        <source>Firstname</source>
        <translation>Prénom</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="64"/>
        <source>Queue member&apos;s firstname</source>
        <translation>Prénom du membre de la file</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="65"/>
        <source>Lastname</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="66"/>
        <source>Queue member&apos;s lastname</source>
        <translation>Nom du membre de la file</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="67"/>
        <source>Logged in</source>
        <translation>Logué</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="68"/>
        <source>Queue member&apos;s login status</source>
        <translation>État logué du membre de la file</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="69"/>
        <source>Paused</source>
        <translation>En retrait</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="70"/>
        <source>Queue member&apos;s pause status</source>
        <translation>Retrait du membre de la file</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="71"/>
        <source>Answered calls</source>
        <translation>Appels répondus</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="72"/>
        <source>Number of answered calls</source>
        <translation>Nombre d&apos;appels répondus</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="73"/>
        <source>Last call</source>
        <translation>Dernier appel</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="74"/>
        <source>Hangup date of the last answered call</source>
        <translation>Date de fin du dernier appel</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="75"/>
        <source>Penalty</source>
        <translation>Pénalité</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="76"/>
        <source>Queue member&apos;s penalty</source>
        <translation>Pénalité du membre de la file</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="196"/>
        <source>Always</source>
        <translation>Toujours</translation>
    </message>
    <message>
        <location filename="../src/xlets/queue_members/queue_members_model.cpp" line="198"/>
        <source>Never</source>
        <translation>Jamais</translation>
    </message>
</context>
<context>
    <name>XletQueueMembers</name>
    <message>
        <location filename="../src/xlets/queue_members/queue_members.cpp" line="47"/>
        <source>Queue Members</source>
        <translation>Membres d&apos;une file</translation>
    </message>
</context>
<context>
    <name>Directory</name>
    <message>
        <location filename="../src/xlets/directory/directory.cpp" line="44"/>
        <source>Directory</source>
        <translation>Répertoire</translation>
    </message>
</context>
<context>
    <name>DirectoryWidget</name>
    <message>
        <location filename="../src/xlets/directory/directory_widget.ui" line="14"/>
        <source>Form</source>
        <translation>Formulaire</translation>
    </message>
</context>
<context>
    <name>CallAction</name>
    <message>
        <location filename="../src/xlets/people/people_action_generator.cpp" line="230"/>
        <source>Call</source>
        <translation>Appeler</translation>
    </message>
</context>
<context>
    <name>ChatAction</name>
    <message>
        <location filename="../src/xlets/people/people_action_generator.cpp" line="297"/>
        <source>Send a message</source>
        <translation>Envoyer un message</translation>
    </message>
</context>
<context>
    <name>ContactDialogWidget</name>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="17"/>
        <source>Personal Contact</source>
        <translation>Contact personnel</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="37"/>
        <source>Last Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="44"/>
        <source>First Name</source>
        <translation>Prénom</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="78"/>
        <source>Phone Number</source>
        <translation>Numéro de téléphone</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="85"/>
        <source>Mobile Number</source>
        <translation>Numéro de mobile</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="105"/>
        <source>Company</source>
        <translation>Entreprise</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="121"/>
        <source>Fax</source>
        <translation>Fax</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/contact_dialog_widget.ui" line="128"/>
        <source>Email</source>
        <translation>Email</translation>
    </message>
</context>
<context>
    <name>People</name>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="50"/>
        <source>People</source>
        <translation>Social</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="60"/>
        <source>all</source>
        <translation>tous</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="61"/>
        <source>favorites</source>
        <translation>favoris</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="62"/>
        <source>my contacts</source>
        <translation>mes contacts</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="220"/>
        <source>Import Results</source>
        <translation>Résultat d&apos;import</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="221"/>
        <source>%1 contacts created
%2 contacts failed</source>
        <translation>%1 contacts ajoutés
%2 contacts échoués</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="228"/>
        <source>Following lines are errors</source>
        <translation>Ces lignes sont invalides</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="231"/>
        <source>
line %1: </source>
        <translation>
ligne %1: </translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="291"/>
        <source>Removing this contact</source>
        <translation>Suppression du contact</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="292"/>
        <source>Removing this contact.
Are you sure ?</source>
        <translation>Suppression du contact.
Êtes-vous sûr(e) ?</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="307"/>
        <source>Deleting all personal contacts</source>
        <translation>Suppression de tous les contacts personnels</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="308"/>
        <source>Deleting all personal contacts.
Are you sure ?</source>
        <translation>Suppression de tous les contacts personnels.
Êtes-vous certain?</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="399"/>
        <source>Save Personal Contacts</source>
        <translation>Enregistrer le fichier de contacts</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="401"/>
        <source>CSV Files (*.csv);;All Files (*)</source>
        <translation>Fichiers CSV (*.csv);;Tous les Fichiers (*)</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="427"/>
        <source>Import Personal Contacts</source>
        <translation>Import de contacts personnels</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="429"/>
        <source>CSV Files (UTF-8)(*.csv);;All Files (*)</source>
        <translation>Fichiers CSV (UTF-8) (*.csv);;Tous les Fichiers (*)</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="479"/>
        <source>Failed</source>
        <translation>Échec</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people.cpp" line="501"/>
        <source>Waiting</source>
        <translation>En attente</translation>
    </message>
</context>
<context>
    <name>PeopleEntryNumberDelegate</name>
    <message>
        <location filename="../src/xlets/people/people_entry_delegate.cpp" line="124"/>
        <source>CALL</source>
        <translation>APPELER</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_entry_delegate.cpp" line="124"/>
        <source>ACTIONS</source>
        <translation>ACTIONS</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_entry_delegate.cpp" line="254"/>
        <source>BLIND TRANSFER</source>
        <translation>TRANSFERT DIRECT</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_entry_delegate.cpp" line="256"/>
        <source>ATTENDED TRANSFER</source>
        <translation>TRANSFERT INDIRECT</translation>
    </message>
</context>
<context>
    <name>PeoplePersonalMigration</name>
    <message>
        <location filename="../src/xlets/people/people_personal_migration.cpp" line="89"/>
        <source>Contacts Migration</source>
        <translation>Migration des contacts</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_personal_migration.cpp" line="90"/>
        <source>Your local contacts will be migrated to the People Xlet. %1</source>
        <translation>Vos contacts locaux vont être migrés vers la Xlet Social. %1</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_personal_migration.cpp" line="94"/>
        <source>Your local contacts are currently stored on your machine in:
%1
They will be migrated to the XiVO server in your personal database.</source>
        <translation>Vos contacts locaux sont actuellement stockés sur votre machine, dans le fichier:
%1
Ils vont être migrés sur le serveur XiVO, dans votre base de données personnelle.</translation>
    </message>
</context>
<context>
    <name>PeopleWidget</name>
    <message>
        <location filename="../src/xlets/people/people_widget.ui" line="54"/>
        <source>Contacts</source>
        <translation>Liste de contacts</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_widget.ui" line="105"/>
        <source>search</source>
        <translation>rechercher</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_widget.ui" line="133"/>
        <source>NEW CONTACT</source>
        <translation>NOUVEAU CONTACT</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_widget.ui" line="149"/>
        <source>IMPORT</source>
        <translation>IMPORTER</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_widget.ui" line="165"/>
        <source>EXPORT</source>
        <translation>EXPORTER</translation>
    </message>
    <message>
        <location filename="../src/xlets/people/people_widget.ui" line="181"/>
        <source>DELETE ALL CONTACTS</source>
        <translation>SUPPRIMER TOUS LES CONTACTS</translation>
    </message>
</context>
<context>
    <name>XletAgentDetails</name>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="49"/>
        <source>Agent Details</source>
        <translation>Détails des Agents</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="56"/>
        <source>Queues</source>
        <translation>Files d&apos;attente</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="57"/>
        <source>Joined</source>
        <translation>Jointes</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="58"/>
        <source>Paused</source>
        <translation>Retrait</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="62"/>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="207"/>
        <source>Login</source>
        <translation>Loguer</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="197"/>
        <source>on &lt;b&gt;%1&lt;/b&gt;</source>
        <translation>sur &lt;b&gt;%1&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="203"/>
        <source>Logout</source>
        <translation>Déloguer</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="227"/>
        <source>Not in use</source>
        <translation>Libre</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="231"/>
        <source>In use</source>
        <translation>ECT</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="235"/>
        <source>Int. Incoming </source>
        <translation>Interne reçu</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="239"/>
        <source>Ext. Incoming</source>
        <translation>Externe reçu</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="243"/>
        <source>Int. Outgoing</source>
        <translation>Interne émis</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="247"/>
        <source>Ext. Outgoing</source>
        <translation>Externe émis</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="251"/>
        <source>Unlogged</source>
        <translation>Délogué</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="255"/>
        <source>%1 since %2</source>
        <translation>%1 depuis %2</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="346"/>
        <source>Server: %1</source>
        <translation>Serveur: %1</translation>
    </message>
    <message>
        <location filename="../src/xlets/agentdetails/agentdetails.cpp" line="347"/>
        <source>Context: %1</source>
        <translation>Contexte: %1</translation>
    </message>
</context>
<context>
    <name>Conference</name>
    <message>
        <location filename="../src/xlets/conference/conference.cpp" line="42"/>
        <source>Conference</source>
        <translation>Chambre de conférence</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference.cpp" line="50"/>
        <source>Room list</source>
        <translation>Liste des chambres</translation>
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
        <translation>Nom</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="38"/>
        <source>Number</source>
        <translation>Numéro</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="39"/>
        <source>PIN code</source>
        <translation>Code PIN</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="40"/>
        <source>Member count</source>
        <translation>Membres</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="41"/>
        <source>Started since</source>
        <translation>Commencée depuis</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="107"/>
        <source>Yes</source>
        <translation>Oui</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="109"/>
        <source>No</source>
        <translation>Non</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="137"/>
        <source>Not started</source>
        <translation>Pas commencée</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_list_model.cpp" line="139"/>
        <source>Unknown</source>
        <translation>Inconnu</translation>
    </message>
</context>
<context>
    <name>ConferenceRoomModel</name>
    <message>
        <location filename="../src/xlets/conference/conference_room_model.cpp" line="42"/>
        <source>Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_room_model.cpp" line="43"/>
        <source>Number</source>
        <translation>Numéro</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_room_model.cpp" line="44"/>
        <source>Since</source>
        <translation>Depuis</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_room_model.cpp" line="158"/>
        <source>Mute/UnMute</source>
        <translation>(Dés)activer le micro</translation>
    </message>
    <message>
        <location filename="../src/xlets/conference/conference_room_model.cpp" line="174"/>
        <source>Unknown</source>
        <translation>Inconnu</translation>
    </message>
</context>
<context>
    <name>ConferenceWidget</name>
    <message>
        <location filename="../src/xlets/conference/conference_widget.ui" line="54"/>
        <source>Conference</source>
        <translation>Chambre de conférence</translation>
    </message>
</context>
<context>
    <name>CurrentCall</name>
    <message>
        <location filename="../src/xlets/switchboard/current_call.cpp" line="50"/>
        <source>Attended T</source>
        <translation>T Indirect</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.cpp" line="51"/>
        <source>Call</source>
        <translation>Appeler</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.cpp" line="52"/>
        <source>Complete T</source>
        <translation>Compléter T</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.cpp" line="53"/>
        <source>Hangup</source>
        <translation>Raccrocher</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.cpp" line="54"/>
        <source>Cancel</source>
        <translation>Annuler</translation>
    </message>
</context>
<context>
    <name>CurrentCallWidget</name>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="14"/>
        <source>Form</source>
        <translation>Form</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="66"/>
        <source>Current Call</source>
        <translation>Appel en cours</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="82"/>
        <source>Enter</source>
        <translation>Entrée</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="98"/>
        <source>Answer</source>
        <translation>Répondre</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="147"/>
        <source>Call</source>
        <translation>Appeler</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="183"/>
        <source>Blind Transfer</source>
        <translation>Transfert direct</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="186"/>
        <source>Blind T</source>
        <translation>T Direct</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="189"/>
        <source>F4</source>
        <translation>F4</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="222"/>
        <source>Attended Transfer</source>
        <translation>Transfert indirect</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="225"/>
        <source>Attended T</source>
        <translation>T Indirect</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="228"/>
        <source>F5</source>
        <translation>F5</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="261"/>
        <source>Hold</source>
        <translation>Mettre en attente</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="264"/>
        <source>F7</source>
        <translation>F7</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/current_call.ui" line="301"/>
        <source>Hangup</source>
        <translation>Raccrocher</translation>
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
        <translation>POPC</translation>
    </message>
    <message numerus="yes">
        <location filename="../src/xlets/switchboard/switchboard.cpp" line="323"/>
        <location filename="../src/xlets/switchboard/switchboard.cpp" line="333"/>
        <source>%n call(s)</source>
        <translation>
            <numerusform>%n appel</numerusform>
            <numerusform>%n appels</numerusform>
        </translation>
    </message>
</context>
<context>
    <name>SwitchboardPanel</name>
    <message>
        <location filename="../src/xlets/switchboard/switchboard.ui" line="35"/>
        <source>Incoming Calls (F6)</source>
        <translation>Appels entrants (F6)</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/switchboard.ui" line="62"/>
        <location filename="../src/xlets/switchboard/switchboard.ui" line="132"/>
        <source>0 call</source>
        <translation>0 appel</translation>
    </message>
    <message>
        <location filename="../src/xlets/switchboard/switchboard.ui" line="105"/>
        <source>Waiting Calls (F9)</source>
        <translation>Appels en attente (F9)</translation>
    </message>
</context>
<context>
    <name>QueuesModel</name>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="42"/>
        <source>Number</source>
        <translation>Numéro</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="43"/>
        <source>Phone number of the queue</source>
        <translation>Numéro d&apos;appel de la file d&apos;attente</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="44"/>
        <source>Queues</source>
        <translation>Files d&apos;attente</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="45"/>
        <source>Queue names</source>
        <translation>Noms des files d&apos;attente</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="46"/>
        <source>Waiting calls</source>
        <translation>Appels en
attente</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="47"/>
        <source>Number of waiting calls</source>
        <translation>Nombre d&apos;appels en attente</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="48"/>
        <source>EWT</source>
        <translation>TAE</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="49"/>
        <source>Estimated Waiting Time</source>
        <translation>Temps d&apos;attente estimée</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="50"/>
        <source>Longest wait</source>
        <translation>Plus longue
attente</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="51"/>
        <source>Longest waiting call</source>
        <translation>Plus longue attente des appels en cours</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="52"/>
        <source>Logged in</source>
        <translation>Logués</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="53"/>
        <source>Number of agents logged in</source>
        <translation>Nombre d&apos;agents logués</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="54"/>
        <source>Available</source>
        <translation>Disponibles</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="55"/>
        <source>Number of agents ready to take a call</source>
        <translation>Nombre d&apos;agents prêts à recevoir un appel</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="56"/>
        <source>Talking</source>
        <translation>En conversation</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="57"/>
        <source>Number of agents talking</source>
        <translation>Nombre d&apos;agents en conversation</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="58"/>
        <source>Received</source>
        <translation>Reçus</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="59"/>
        <source>Number of received calls</source>
        <translation>Nombre d&apos;appels reçus</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="60"/>
        <source>Answered</source>
        <translation>Répondus</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="61"/>
        <source>Number of answered calls</source>
        <translation>Nombre d&apos;appels répondus</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="62"/>
        <source>Abandoned</source>
        <translation>Abandonnés</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="63"/>
        <source>Number of abandoned calls</source>
        <translation>Nombre d&apos;appels abandonnés</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="64"/>
        <source>Mean Waiting Time</source>
        <translation>Tps Moy Attente </translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="65"/>
        <source>Mean waiting time before getting an agent</source>
        <translation>Temps moyen d&apos;attente avant d&apos;être en communication avec un agent</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="66"/>
        <source>Max Waiting Time</source>
        <translation>Tps Attente Max</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="67"/>
        <source>Maximum waiting time before getting an agent</source>
        <translation>Plus grand temps d&apos;attente avant d&apos;obtenir un agent</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="68"/>
        <source>Efficiency</source>
        <translation>Efficacité</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="69"/>
        <source>Ratio (Answered) / (Received)</source>
        <translation>Ratio répondus / reçus</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="70"/>
        <source>QOS</source>
        <translation>QOS</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queuesmodel.cpp" line="71"/>
        <source>Ratio (Calls answered in less than X sec / Number of calls answered)</source>
        <translation>Ratio (Appels répondus en moins de X sec) / (Nombre d&apos;appels reçus)</translation>
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
        <translation>Files d&apos;attente</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="120"/>
        <source>Configure</source>
        <translation>Configurer</translation>
    </message>
</context>
<context>
    <name>XletQueuesConfigure</name>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="156"/>
        <source>Queues configuration</source>
        <translation>Configuration des files</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="158"/>
        <source>Choose which queue should be displayed, and the
queues parameters for the Stats on slice:</source>
        <translation>Choisissez quelles files d&apos;attente doivent être affichées, 
ainsi que les paramètres pour les statistiques:</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="169"/>
        <source>&amp;Close</source>
        <translation>&amp;Fermer</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="182"/>
        <source>Queue</source>
        <translation>File d&apos;attente</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="183"/>
        <source>Qos - X (s)</source>
        <translation>Qos - X (s)</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="184"/>
        <source>This is the threshold in seconds to consider that the answer to a call was too late to be accounted as an answer of quality.</source>
        <translation>Ceci est le seuil en secondes de la durée d&apos;attente pour considérer la réponse à un appel comme étant trop tardive pour être de qualité.</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="188"/>
        <source>Window (s)</source>
        <translation>Fenêtre (s)</translation>
    </message>
    <message>
        <location filename="../src/xlets/queues/queues.cpp" line="189"/>
        <source>The window is the period of time used to compute the statistics</source>
        <translation>La fenêtre est la durée sur laquelle les statistiques sont calculées</translation>
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
        <translation>Tableau de bord des agents</translation>
    </message>
</context>
<context>
    <name>ConfigWidget</name>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="92"/>
        <source>System Language</source>
        <translation>Langue du système</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="93"/>
        <source>Dutch</source>
        <translation>Néerlandais</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="94"/>
        <source>English</source>
        <translation>Anglais</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="95"/>
        <source>French</source>
        <translation>Français</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="96"/>
        <source>Italian</source>
        <translation>Italien</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="97"/>
        <source>Portuguese</source>
        <translation>Portugais</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="98"/>
        <source>Spanish (Chile)</source>
        <translation>Espagnol (Chili)</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="99"/>
        <location filename="../src/config_widget/config_widget.cpp" line="100"/>
        <location filename="../src/config_widget/config_widget.cpp" line="101"/>
        <location filename="../src/config_widget/config_widget.cpp" line="102"/>
        <source>incomplete</source>
        <translation>incomplet</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="99"/>
        <source>German</source>
        <translation>Allemand</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="100"/>
        <source>Hungarian</source>
        <translation>Hongrois</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="101"/>
        <source>Japanese</source>
        <translation>Japonais</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.cpp" line="102"/>
        <source>Spanish</source>
        <translation>Espagnol</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="20"/>
        <source>XiVO Client Configuration</source>
        <translation>Configuration du XiVO Client</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="37"/>
        <source>User</source>
        <translation>Utilisateur</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="45"/>
        <source>Language</source>
        <translation>Langue</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="80"/>
        <location filename="../src/config_widget/config_widget.ui" line="152"/>
        <location filename="../src/config_widget/config_widget.ui" line="722"/>
        <source>You must restart the program for this setting to apply.</source>
        <translation>Vous devez redémarrer le programme pour appliquer ce paramètre.</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="113"/>
        <source>Auto-reconnect </source>
        <translation>Re-connexion automatique</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="120"/>
        <source>Connect at startup</source>
        <translation>Connexion au démarrage</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="127"/>
        <source>Start systrayed</source>
        <translation>Minimiser au démarrage </translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="134"/>
        <source>Show agent options</source>
        <translation>Afficher les options d&apos;agent sur l&apos;écran d&apos;identification</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="155"/>
        <source>Show window when calling from external application</source>
        <translation>Afficher la fenêtre lorsqu&apos;un appel est passé par une application externe</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="190"/>
        <source>Reset dock positions</source>
        <translation>Ré-initialiser la position des docks</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="226"/>
        <source>Features</source>
        <translation>Fonctionnalités</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="250"/>
        <source>Presence</source>
        <translation>Présence</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="256"/>
        <source>Presence reporting</source>
        <translation>Rapport de présence</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="275"/>
        <source>Screen popup</source>
        <translation>Remontée de fiches</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="284"/>
        <source>Max screen popups</source>
        <translation>Nombre maximum de fiches</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="304"/>
        <source>Screen Popup</source>
        <translation>Remontée de fiches</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="314"/>
        <source>Allow automatic URL opening</source>
        <translation>Autoriser l&apos;ouverture automatique d&apos;URL</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="333"/>
        <source>History</source>
        <translation>Historique</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="342"/>
        <source>History size</source>
        <translation>Taille de l&apos;historique</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="368"/>
        <source>Queue list</source>
        <translation>Files d&apos;attente</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="376"/>
        <source>Show longest wait time</source>
        <translation>Afficher la colonne &quot;plus longue attente&quot;</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="390"/>
        <source>Orange</source>
        <translation>Orange</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="400"/>
        <source>Red</source>
        <translation>Rouge</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="410"/>
        <source>Longest wait time thresholds</source>
        <translation>Seuils d&apos;affichage des files pour la plus longue attente</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="437"/>
        <source>Waiting calls thresholds</source>
        <translation>Seuils d&apos;affichage des files pour les appels en attente</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="464"/>
        <source>calls</source>
        <translation>appels</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="471"/>
        <source>seconds</source>
        <translation>secondes</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="502"/>
        <source>Queue Members</source>
        <translation>Membres d&apos;une file d&apos;attente</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="508"/>
        <source>Hide unlogged agents</source>
        <translation>Masquer les agent délogués</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="524"/>
        <source>Switchboard</source>
        <translation>POPC</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="533"/>
        <source>Switchboard call on hold queue name</source>
        <translation>Nom de la file d&apos;attente des appels en attentes du POPC</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="543"/>
        <source>Switchboard queue name</source>
        <translation>Nom de la file d&apos;attente du POPC</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="565"/>
        <source>Administration</source>
        <translation>Administration</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="573"/>
        <source>Server</source>
        <translation>Serveur</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="587"/>
        <location filename="../src/config_widget/config_widget.ui" line="634"/>
        <source>Port</source>
        <translation>Port</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="620"/>
        <source>Backup server</source>
        <translation>Serveur de secours</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="709"/>
        <source>Advanced</source>
        <translation>Avancé</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="715"/>
        <source>Display the configuration profile</source>
        <translation>Afficher le profil de configuration</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="725"/>
        <source>Allow multiple instances</source>
        <translation>Autoriser plusieurs instances simultanées du client</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="736"/>
        <source>Auto-reconnect interval</source>
        <translation>Délai de reconnexion</translation>
    </message>
    <message>
        <location filename="../src/config_widget/config_widget.ui" line="775"/>
        <source>Keepalive interval</source>
        <translation>Délai de maintien de la connexion</translation>
    </message>
</context>
<context>
    <name>CustomerInfoPanel</name>
    <message>
        <location filename="../src/xlets/customerinfo-builtin/customerinfopanel.cpp" line="35"/>
        <source>Sheets</source>
        <translation>Fiches</translation>
    </message>
</context>
<context>
    <name>LoginWidget</name>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="20"/>
        <source>Login Screen</source>
        <translation>Écran d&apos;identification</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="144"/>
        <source>LOGIN</source>
        <translation>CONNEXION</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="271"/>
        <source>XiVO is a unified communication system that connects phones inside an organization with public and mobile telephone networks.</source>
        <translation>XiVO est un système de communication unifiée qui relie les téléphones à l&apos;intérieur d&apos;une organisation avec les réseaux téléphoniques publics ainsi que les réseaux mobiles.</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="306"/>
        <source>login</source>
        <translation>utilisateur</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="340"/>
        <source>password</source>
        <translation>mot de passe</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="376"/>
        <source>Remember me</source>
        <translation>Se souvenir de moi</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="405"/>
        <source>No Agent</source>
        <translation>Sans agent</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="410"/>
        <source>Agent (logged off)</source>
        <translation>Agent (non logué)</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="415"/>
        <source>Agent (logged in)</source>
        <translation>Agent (logué)</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="423"/>
        <source>phone</source>
        <translation>téléphone</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.ui" line="471"/>
        <source>CONNECT</source>
        <translation>ME CONNECTER</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.cpp" line="139"/>
        <source>Connecting</source>
        <translation>Connexion en cours</translation>
    </message>
    <message>
        <location filename="../src/login_widget/login_widget.cpp" line="152"/>
        <source>Failed</source>
        <translation>Échec</translation>
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
        <translation>&amp;Aide</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="88"/>
        <source>Confi&amp;gure</source>
        <translation>Confi&amp;gurer</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="91"/>
        <source>Configure account and connection options</source>
        <translation>Configurer votre compte et vos options de connexion</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="99"/>
        <source>To S&amp;ystray</source>
        <translation>&amp;Minimiser</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="102"/>
        <source>Enter the system tray</source>
        <translation>Minimiser l&apos;application dans la zone de notification système</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="107"/>
        <source>&amp;Connect</source>
        <translation>&amp;Connecter</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="110"/>
        <source>Connect to the server</source>
        <translation>Se connecter au serveur</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="118"/>
        <source>&amp;Disconnect</source>
        <translation>&amp;Déconnecter</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="121"/>
        <source>Disconnect from the server</source>
        <translation>Se déconnecter du serveur</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="129"/>
        <source>&amp;Quit</source>
        <translation>&amp;Quitter</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="132"/>
        <source>Close the application</source>
        <translation>Fermer l&apos;application</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="135"/>
        <source>Ctrl+Q</source>
        <translation>Ctrl+Q</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="143"/>
        <source>&amp;Show window</source>
        <translation>&amp;Afficher la fenêtre</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="146"/>
        <source>Leave the system tray</source>
        <translation>Sortir l&apos;application de la zone de notification système</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="154"/>
        <source>&amp;About XiVO Client</source>
        <translation>À propos de &amp;XiVO Client</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="159"/>
        <source>About &amp;Qt</source>
        <translation>À propos de &amp;Qt</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.ui" line="164"/>
        <source>&amp;Credits</source>
        <translation>&amp;Crédits</translation>
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
        <translation>XiVO CTI Erreur</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="194"/>
        <source>Client %1 (%2 profile)</source>
        <translation>Client %1 (profil %2)</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="317"/>
        <source>&amp;Availability</source>
        <translation>&amp;Disponibilité</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="389"/>
        <source>Fetch the last one</source>
        <translation>Récupérer la dernière</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="396"/>
        <source>About XiVO Client</source>
        <translation>À propos de XiVO Client</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="398"/>
        <source>The XiVO CTI applications, once connected to the proper server, complete the XiVO telephony system, by providing computer tools, such as customer information display, directory searches, or various supervision &apos;X-lets&apos; for call centers.</source>
        <translation>Les applications CTI XiVO, une fois connectées au serveur adéquat, complètent le système téléphonique XiVO, en fournissant des outils informatiques, tels que la remontée de fiches, les recherches dans des répertoires, ainsi que divers &apos;X-lets&apos; de supervision pour les centres d&apos;appel.</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="403"/>
        <source>&lt;u&gt;Version Information&lt;/u&gt;</source>
        <translation>&lt;u&gt;Information de version&lt;/u&gt;</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="404"/>
        <source>Major : &lt;b&gt;%1&lt;/b&gt; (XiVO branch)</source>
        <translation>Majeure : &lt;b&gt;%1&lt;/b&gt; (branche XiVO)</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="405"/>
        <source>Minor : git hash &lt;b&gt;%1&lt;/b&gt; from : %2</source>
        <translation>Mineure : git hash &lt;b&gt;%1&lt;/b&gt; du : %2</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="408"/>
        <source>(Application Built on : %1)</source>
        <translation>(Application compilée le : %1)</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="409"/>
        <source>(Application Launched on : %1)</source>
        <translation>(Application lancée le : %1)</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="410"/>
        <source>(Config File Location : %1)</source>
        <translation>(Fichier de Configuration : %1)</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="413"/>
        <source>E-mail : </source>
        <translation>E-mail : </translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="414"/>
        <source>Phone : </source>
        <translation>Téléphone : </translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="415"/>
        <source>Authors : </source>
        <translation>Auteurs : </translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="415"/>
        <source>Avencall Development Team</source>
        <translation>Équipe de développement Avencall</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="417"/>
        <source>License : </source>
        <translation>Licence : </translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="419"/>
        <source>with a special exception allowing you to link it with OpenSSL, under some conditions.</source>
        <translation>avec une exception particulière vous permettant de le lier avec OpenSSL, sous certaines conditions.</translation>
    </message>
    <message>
        <location filename="../src/main_window/main_window.cpp" line="425"/>
        <source>Credits</source>
        <translation>Crédits</translation>
    </message>
</context>
<context>
    <name>Statusbar</name>
    <message>
        <location filename="../src/main_window/statusbar.cpp" line="78"/>
        <source>Connected</source>
        <translation>Connecté</translation>
    </message>
    <message>
        <location filename="../src/main_window/statusbar.cpp" line="84"/>
        <source>Disconnected</source>
        <translation>Déconnecté</translation>
    </message>
</context>
<context>
    <name>loading_dialog</name>
    <message>
        <location filename="../src/main_window/loading_dialog.ui" line="17"/>
        <source>Loading Screen</source>
        <translation>Écran de chargement</translation>
    </message>
    <message>
        <location filename="../src/main_window/loading_dialog.ui" line="34"/>
        <source>Loading...</source>
        <translation>Chargement en cours...</translation>
    </message>
</context>
<context>
    <name>History</name>
    <message>
        <location filename="../src/xlets/history/history.cpp" line="39"/>
        <source>History</source>
        <translation>Historique</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history.cpp" line="52"/>
        <source>All calls</source>
        <translation>Tous les appels</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history.cpp" line="53"/>
        <source>Sent calls</source>
        <translation>Appels émis</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history.cpp" line="54"/>
        <source>Received calls</source>
        <translation>Appels reçus</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history.cpp" line="55"/>
        <source>Missed calls</source>
        <translation>Appels manqués</translation>
    </message>
</context>
<context>
    <name>HistoryModel</name>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="41"/>
        <source>Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="42"/>
        <source>Number</source>
        <translation>Numéro</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="43"/>
        <source>Date</source>
        <translation>Date</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="44"/>
        <source>Duration</source>
        <translation>Durée</translation>
    </message>
    <message>
        <location filename="../src/xlets/history/history_model.cpp" line="160"/>
        <source>%1 hr %2 min %3 s</source>
        <translation>%1 h %2 min %3 s</translation>
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
        <translation>Historique</translation>
    </message>
</context>
<context>
    <name>AgentsModel</name>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="43"/>
        <source>Number</source>
        <translation>Numéro</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="44"/>
        <source>First name</source>
        <translation>Prénom</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="45"/>
        <source>Last name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="46"/>
        <location filename="../src/xletlib/agents_model.cpp" line="200"/>
        <source>Listen</source>
        <translation>Activer l&apos;écoute</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="47"/>
        <source>Status since</source>
        <translation>État depuis</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="48"/>
        <location filename="../src/xletlib/agents_model.cpp" line="390"/>
        <source>Logged in</source>
        <translation>Logué</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="49"/>
        <source>Joined
queues</source>
        <translation>Files
jointes</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="50"/>
        <location filename="../src/xletlib/agents_model.cpp" line="411"/>
        <source>Paused</source>
        <translation>En retrait</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="51"/>
        <source>Paused
queues</source>
        <translation>Files
en retrait</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="268"/>
        <source>Agent ready to receive a call</source>
        <translation>Agent prêt à recevoir un appel</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="270"/>
        <source>Agent processing a call or paused</source>
        <translation>Agent En Cours de Traitement</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="272"/>
        <source>Agent receiving an internal call out of queue</source>
        <translation>L&apos;agent reçoit un appel interne hors file d&apos;attente</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="274"/>
        <source>Agent receiving an external call out of queue</source>
        <translation>L&apos;agent reçoit un appel externe hors file d&apos;attente</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="276"/>
        <source>Agent emitting an internal call</source>
        <translation>L&apos;agent émet un appel interne hors file d&apos;attente</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="278"/>
        <source>Agent emitting an external call</source>
        <translation>L&apos;agent émet un appel externe hors file d&apos;attente</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="334"/>
        <source>Not in use</source>
        <translation>Libre</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="336"/>
        <source>In use</source>
        <translation>ECT</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="338"/>
        <source>Int. Incoming</source>
        <translation>Interne reçu</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="340"/>
        <source>Ext. Incoming</source>
        <translation>Externe reçu</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="342"/>
        <source>Int. Outgoing</source>
        <translation>Interne émis</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="344"/>
        <source>Ext. Outgoing</source>
        <translation>Externe émis</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="392"/>
        <source>Logged out</source>
        <translation>Délogué</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="409"/>
        <source>Unpaused</source>
        <translation>Prêt</translation>
    </message>
    <message>
        <location filename="../src/xletlib/agents_model.cpp" line="413"/>
        <source>Partially Paused</source>
        <translation>Retrait partiel</translation>
    </message>
</context>
<context>
    <name>ChitChatWindow</name>
    <message>
        <location filename="../src/xletlib/chitchat.cpp" line="117"/>
        <source>&amp;Clear history</source>
        <translation>E&amp;ffacer le dialogue</translation>
    </message>
    <message>
        <location filename="../src/xletlib/chitchat.cpp" line="118"/>
        <source>&amp;Send</source>
        <translation>&amp;Envoyer</translation>
    </message>
    <message>
        <location filename="../src/xletlib/chitchat.cpp" line="139"/>
        <source>chat window opened with &quot;%1&quot;</source>
        <translation>fenêtre de discussion ouverte avec &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/xletlib/chitchat.cpp" line="183"/>
        <source>you said: </source>
        <translation>Vous avez dit:</translation>
    </message>
    <message>
        <location filename="../src/xletlib/chitchat.cpp" line="205"/>
        <source>chitchat - %1</source>
        <translation>chitchat - %1</translation>
    </message>
</context>
<context>
    <name>DirectoryPanel</name>
    <message>
        <location filename="../src/xletlib/directorypanel.cpp" line="37"/>
        <source>Remote Directory Panel</source>
        <translation>Répertoire distant</translation>
    </message>
    <message>
        <location filename="../src/xletlib/directorypanel.cpp" line="40"/>
        <source>Remote Di&amp;rectory</source>
        <translation>Répe&amp;rtoire distant</translation>
    </message>
    <message>
        <location filename="../src/xletlib/directorypanel.cpp" line="48"/>
        <source>Search</source>
        <translation>Rechercher</translation>
    </message>
    <message>
        <location filename="../src/xletlib/directorypanel.cpp" line="131"/>
        <source>Double-click to send an E-mail to</source>
        <translation>Double-cliquer pour envoyer un e-mail à</translation>
    </message>
    <message>
        <location filename="../src/xletlib/directorypanel.cpp" line="133"/>
        <source>Double-click to call</source>
        <translation>Double-cliquer pour appeler</translation>
    </message>
</context>
<context>
    <name>ExtendedTableWidget</name>
    <message>
        <location filename="../src/xletlib/extendedtablewidget.cpp" line="63"/>
        <source>&amp;Dial</source>
        <translation>&amp;Composer</translation>
    </message>
    <message>
        <location filename="../src/xletlib/extendedtablewidget.cpp" line="66"/>
        <source>Send an E-mail</source>
        <translation>Envoyer un E-mail</translation>
    </message>
</context>
<context>
    <name>ExternalPhoneDialog</name>
    <message>
        <location filename="../src/xletlib/externalphonedialog.cpp" line="46"/>
        <source>External phone number</source>
        <translation>Numéro de téléphone externe</translation>
    </message>
    <message>
        <location filename="../src/xletlib/externalphonedialog.cpp" line="49"/>
        <source>Label</source>
        <translation>Etiquette</translation>
    </message>
    <message>
        <location filename="../src/xletlib/externalphonedialog.cpp" line="51"/>
        <source>Phone number</source>
        <translation>Numéro de téléphone</translation>
    </message>
</context>
<context>
    <name>NumberButtonDelegate</name>
    <message>
        <location filename="../src/xletlib/number_button_delegate.cpp" line="68"/>
        <source>CALL</source>
        <translation>APPELER</translation>
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
        <translation>Position</translation>
    </message>
    <message>
        <location filename="../src/xletlib/queue_entries/queue_entries_model.cpp" line="56"/>
        <source>Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <location filename="../src/xletlib/queue_entries/queue_entries_model.cpp" line="57"/>
        <source>Number</source>
        <translation>Numéro</translation>
    </message>
    <message>
        <location filename="../src/xletlib/queue_entries/queue_entries_model.cpp" line="58"/>
        <source>Time</source>
        <translation>Durée</translation>
    </message>
</context>
<context>
    <name>SearchDialog</name>
    <message>
        <location filename="../src/xletlib/searchdialog.cpp" line="40"/>
        <source>Search :</source>
        <translation>Recherche :</translation>
    </message>
</context>
<context>
    <name>IdentityDisplay</name>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="53"/>
        <source>Identity</source>
        <translation>Identité</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="60"/>
        <source>Current User Panel</source>
        <translation>Panneau des détails utilisateur</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="98"/>
        <source>Login</source>
        <translation>Login</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="102"/>
        <source>Logout</source>
        <translation>Logout</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="110"/>
        <source>Pause</source>
        <translation>Retrait</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="114"/>
        <source>Unpause</source>
        <translation>Prêt</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="377"/>
        <source>DND</source>
        <translation>NPD</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="378"/>
        <source>Do Not Disturb</source>
        <translation>Ne Pas Déranger</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="380"/>
        <source>UNC %1</source>
        <translation>INC %1</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="381"/>
        <source>Unconditional Forward towards %1</source>
        <translation>Renvoi Inconditionnel vers %1</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="383"/>
        <source>BUSY %1</source>
        <translation>OCCUPÉ %1</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="384"/>
        <source>Busy Forward towards %1</source>
        <translation>Renvoi sur Occupation vers %1</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="386"/>
        <source>FNA %1</source>
        <translation>RNR %1</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="387"/>
        <source>Non-Answer Forward towards %1</source>
        <translation>Renvoi sur Non-Réponse vers %1</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="389"/>
        <source>CALL FILTER</source>
        <translation>FILTRAGE D&apos;APPEL</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.cpp" line="392"/>
        <source>AVAILABLE</source>
        <translation>DISPONIBLE</translation>
    </message>
</context>
<context>
    <name>IdentityWidget</name>
    <message>
        <location filename="../src/xlets/identity/identity.ui" line="167"/>
        <source>VOICEMAIL</source>
        <translation>BOÎTE VOCALE</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.ui" line="205"/>
        <source>Call your voicemail</source>
        <translation>Appeler votre boîte vocale</translation>
    </message>
    <message>
        <location filename="../src/xlets/identity/identity.ui" line="364"/>
        <source>call</source>
        <translation>appeler</translation>
    </message>
</context>
<context>
    <name>XletDatetime</name>
    <message>
        <location filename="../src/xlets/datetime/datetime.cpp" line="42"/>
        <source>Date and Time</source>
        <translation>Date et Heure</translation>
    </message>
</context>
<context>
    <name>DirDialog</name>
    <message>
        <location filename="../src/xlets/fax/dirdialog.cpp" line="36"/>
        <source>Directory</source>
        <translation>Répertoire</translation>
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
        <translation>
            <numerusform>%n page envoyée</numerusform>
            <numerusform>%n pages envoyées</numerusform>
        </translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="86"/>
        <source>Failed to send</source>
        <translation>Échec de l&apos;envoi</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="98"/>
        <source>Open Fax File</source>
        <translation>Ouvrir le fichier fax</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="100"/>
        <source>PDF Files (*.pdf);;All Files (*)</source>
        <translation>Fichiers PDF (*.pdf);;Tous les fichiers (*)</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="123"/>
        <source>Missing file and fax number</source>
        <translation>Fichier et numéro de fax manquants</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="126"/>
        <source>Missing file</source>
        <translation>Fichier manquant</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="129"/>
        <source>Missing fax number</source>
        <translation>Numéro de fax manquant</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="145"/>
        <source>File empty</source>
        <translation>Fichier vide</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="148"/>
        <source>File not found</source>
        <translation>Fichier introuvable</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="157"/>
        <source>Unreachable number</source>
        <translation>Numéro injoignable</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax.cpp" line="166"/>
        <source>Sending...</source>
        <translation>Envoi en cours...</translation>
    </message>
</context>
<context>
    <name>FaxWidget</name>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="43"/>
        <source>Send fax</source>
        <translation>Envoyer un fax</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="69"/>
        <source>1.</source>
        <translation>1.</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="84"/>
        <source>Choose file to send</source>
        <translation>Choisir le fichier à envoyer</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="122"/>
        <source>file</source>
        <translation>fichier</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="134"/>
        <source>BROWSE...</source>
        <translation>PARCOURIR...</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="163"/>
        <source>2.</source>
        <translation>2.</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="178"/>
        <source>Choose destination number</source>
        <translation>Choisir un numéro de destinataire</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="216"/>
        <source>fax number</source>
        <translation>numéro de fax</translation>
    </message>
    <message>
        <location filename="../src/xlets/fax/fax_widget.ui" line="286"/>
        <source>SEND FAX</source>
        <translation>ENVOYER LE FAX</translation>
    </message>
</context>
<context>
    <name>ServicesPanel</name>
    <message>
        <location filename="../src/xlets/services/servicespanel.cpp" line="41"/>
        <source>Services</source>
        <translation>Services</translation>
    </message>
</context>
<context>
    <name>ServicesWidget</name>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="46"/>
        <source>Services</source>
        <translation>Services</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="72"/>
        <source>Call &amp;filtering</source>
        <translation>&amp;Filtrage d&apos;appel</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="84"/>
        <source>Do not &amp;disturb</source>
        <translation>Ne pas &amp;déranger</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="114"/>
        <source>Call forwards</source>
        <translation>Renvois d&apos;appel</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="147"/>
        <source>Simple call forwards</source>
        <translation>Renvois d&apos;appel simples</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="214"/>
        <source>&amp;Unconditional forward to</source>
        <translation>Renvoi &amp;inconditionnel vers</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="235"/>
        <source>No call forward</source>
        <translation>Aucun renvoi d&apos;appel</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="393"/>
        <source>Forward on &amp;busy to</source>
        <translation>Renvoi sur &amp;occupation vers</translation>
    </message>
    <message>
        <location filename="../src/xlets/services/services_widget.ui" line="424"/>
        <source>Forward on &amp;no answer to</source>
        <translation>Renvoi sur non réponse vers</translation>
    </message>
</context>
</TS>
