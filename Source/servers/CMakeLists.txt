# Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# Enforce compileparameters for corebuilds under GCC
# This to stop a few silly crashes that could have been avoided IF people
# weren't doing some -O3 psychooptimizations etc.

add_subdirectory(Shared)
add_subdirectory(DyNetSocket)
add_subdirectory(DyNetMysql)
add_subdirectory(WorldServer)
add_subdirectory(LogServer)
add_subdirectory(DBServer)
add_subdirectory(GameServer)
add_subdirectory(LoginServer)
add_subdirectory(GatewayServer)
add_subdirectory(Client)
add_subdirectory(HelpTools)
