-------------------------------------------------------------------------------
---------------------------------- Bodyguard ----------------------------------
-------------------------------------------------------------------------------
Bodyguard is a plugin focused on reducing griefing in a PVE setting. It does so
primarily by restricting who can drag a player and his dead tames, as well as
access their inventories. Dragging includes dragging with a dino and grappling.
Secondarily, Bodyguard attempts to offer some crash protection. It can give
players immunity from most sources of damage for a period of time after going
offline, as well as a period of time after reconnecting.

-------------------------------------------------------------------------------
-------------------------------- Access Levels --------------------------------
-------------------------------------------------------------------------------
  There are four access levels that restrict body dragging and inventory access.
The first level is owner. This is the most restrictive level. Only the owner of
a body may drag it or access its inventory. The second level is tribe. Only the
owner of a body or a tribe member of the owner may drag it or access its
inventory. The third level is alliance. Only the owner of a body, a tribe
member of the owner, or an alliance member of the owner may drag it or access
its inventory. The fourth level is all. All places no restrictions on who can
drag a given body or access its inventory. This is the default ARK behavior.
Wild dinos are unaffected by these access levels. Only players and their dead
tames are affected.

  There is a quirk with dead tames when the access level is set to owner.
Depending upon tribe governance, taming can be set to tribe taming or personal
taming. If set to personal taming, only the personal owner may drag their
bodies or access their inventories. If set to tribe taming however, tames won't
have a personal owner, which doesn't make sense with regards to the owner
access level. Therefore, when the access level is set to owner, any tribe-owned
dino will behave as if the access level is set to tribe. That is, any tribe
member can drag their bodies or access their inventories, once dead.

-------------------------------------------------------------------------------
---------------------------------- Body Type ----------------------------------
-------------------------------------------------------------------------------
  There are four different body types considered by Bodyguard. The first type
is DeadDino. This type applies to the bodies of dead tames. The second type is
DeadPlayer. This type applies to the dead bodies of players. The third type is
OfflinePlayer. This type applies to the non-dead bodies of players who are
offline. The fourth type is TranqdPlayer. This type applies to the non-dead
bodies of players who are online and unconscious. For each body type, two
access levels can be set. The first applies to the dragging of that body type.
The second applies to the inventory access of that body type. The two access
levels don't need to be identical. For example, you can set DeadDinoDragging to
owner and DeadDinoInventory to all, if you like. By default, all access levels
for all body types are set to tribe.

-------------------------------------------------------------------------------
------------------------------- Admin Override --------------------------------
-------------------------------------------------------------------------------
  The restrictions enforced by Bodyguard can deter griefing but Admins might
need access to the restricted functionality. For this purpose, Bodyguard
implements admin overrides. For all access levels of all body types, there is
an admin override that can be set to true or false. If true, any admin will not
be restricted no matter the access level. If false, admins will be restricted
equally to normal players. For example, the admin override for
DeadPlayerInventory can be set to true. If a player falls beneath the mesh and
dies, this would allow an admin to access the dead body and retrieve the
player's items without restriction. If set to false, the admin wouldn't be
capable of this if the admin doesn't satisfy the access level.

-------------------------------------------------------------------------------
---------------------------------- Immunity -----------------------------------
-------------------------------------------------------------------------------
  When CrashGuard is enabled, players will be given immunity upon joining and
possibly upon leaving the server. This immunity is categorized as either
permanent or timed. Permanent immunity is given upon joining and has no
predetermined termination time. Timed immunity is given upon joining and
leaving. The goal of timed immunity is to protect players in the event of a
crash. CrashGuard is not currently capable of differentiating between a player
logging out and crashing. Therefore, all players are given immunity for some
period of time after going offline. The idea is that the immunity gives the
player enough time to reconnect without dying. Once connected, players are
given immunity for another period of time. The purpose of this immunity is to
give players enough time to get safe after reconnecting. In a typical
application, the reconnect immunity will be much shorter than the offline
immunity and fairly constant. The offline immunity should vary based on how
long it takes to join the server (number of mods to load, etc).

  What is immunity? Currently, immunity offers protection from common
damage sources. This includes damage from dino attacks, fall damage, and
recoil. It prevents torpidity changes as well as oxygen changes. Practically
speaking, that means no getting knocked unconscious or drowning. Conversely,
if you are unconscious or drowning, you will stay that way. Immunity does
not currently affect buffs/status effects. It is still possible to die from the
likes of hypo/hyperthermia. There are various damage sources which haven't been
tested. If not specifically listed here, assume it can hurt or kill you.

  Immunity is bound to the character to which it is given. If your character
manages to die with immunity, you won't have immunity upon respawning. Immunity
is also lost upon character destruction. Fast travelling, or teleportation in
general, can cause character destruction. As a quick test, create some folders
in your inventory. Perform an action (teleport, transfer, etc). If the folders
are gone, your character was destroyed in the process. Additionally, immunity
can only be given if a live character exists at the time you connect to the
server. If you died while offline, you won't get immunity upon reconnection
because there is no live character to which the immunity can be given.

-------------------------------------------------------------------------------
----------------------------------- Config ------------------------------------
-------------------------------------------------------------------------------
  Bodyguard uses a json file to store the configuration for all of the above.
The 'Admins' object stores all of the admin overrides. Each override should be
set to 'true' or 'false'. The 'Players' object stores all of the access levels
that Bodyguard uses to restrict body dragging and inventory access. Each access
level has four possible values: 'owner', 'tribe', 'alliance', or 'all'. These
values aren't case sensitive. The 'CrashGuard' object governs the settings
related to crash protection. 'Enable' can be set to 'true' or 'false' to enable
or disable crash protection. 'PermanentImmunity' can be set to 'true' or
'false'. If false, timed immunity will be enabled. When timed immunity is enabled,
'OfflineImmunitySeconds' and 'ReconnectImmunitySeconds' determine the length of
immunity players are given when they go offline and reconnect, respectively.
These should be positive numbers in the range of one to don't be ridiculous.

-------------------------------------------------------------------------------
------------------------------ Console Commands -------------------------------
-------------------------------------------------------------------------------
cheat Bodyguard.ReloadConfig
  Reloads config.json from file and applies that configuration to the running
  plugin. If it fails, the default configuration (not config.json) will be
  applied instead.
  
cheat Bodyguard.WriteDefaultConfigToFile
  Writes the default config to config.json. Useful if something happens to your
  config. May become more general in the future.