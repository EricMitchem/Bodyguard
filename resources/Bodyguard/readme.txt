-------------------------------------------------------------------------------
---------------------------------- Bodyguard ----------------------------------
-------------------------------------------------------------------------------
Bodyguard is a plugin focused on reducing griefing in a PVE setting. It does so
primarily by restricting who can drag a player and his dead tames, as well as
access their inventories. Dragging includes dragging with a dino and grappling.

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
----------------------------------- Config ------------------------------------
-------------------------------------------------------------------------------
  Bodyguard uses a json file to store the configuration for all of the above.
The 'Admins' object stores all of the admin overrides. Each override should be
set to 'true' or 'false'. The 'Players' object stores all of the access levels
that Bodyguard uses to restrict body dragging and inventory access. Each access
level has four possible values: 'owner', 'tribe', 'alliance', or 'all'. These
values aren't case sensitive. There is a 'CrashGuard' object as well, but it
isn't currently in use.

-------------------------------------------------------------------------------
------------------------------ Console Commands -------------------------------
-------------------------------------------------------------------------------
cheat Bodyguard.ReloadConfig
  Reloads config.json from file and applies that configuration to the running
  plugin. If it fails, the default configuration will be applied instead.
  
cheat Bodyguard.WriteDefaultConfigToFile
  Writes the default config to config.json. Useful if something happens to your
  config. May become more general in the future.