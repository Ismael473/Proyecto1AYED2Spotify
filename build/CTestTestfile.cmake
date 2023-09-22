# CMake generated Testfile for 
# Source directory: /home/ismael/Desktop/CancionesSpotify/Proyecto1AYED2Spotify
# Build directory: /home/ismael/Desktop/CancionesSpotify/Proyecto1AYED2Spotify/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(UnitTests "runUnitTest")
set_tests_properties(UnitTests PROPERTIES  _BACKTRACE_TRIPLES "/home/ismael/Desktop/CancionesSpotify/Proyecto1AYED2Spotify/CMakeLists.txt;14;add_test;/home/ismael/Desktop/CancionesSpotify/Proyecto1AYED2Spotify/CMakeLists.txt;0;")
subdirs("ext/googletest")
