CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I ../../cadmium/include
INCLUDEDESTIMES=-I ../../DESTimes/include

#CREATE BIN AND BUILD FOLDERS TO SAVE THE COMPILED FILES DURING RUNTIME
bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

#TARGET TO COMPILE ALL THE TESTS TOGETHER (NOT SIMULATOR)

# message.o
main_top.o: top_model/main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) top_model/main.cpp -o build/main_top.o
	
main_heater_test.o: test/main_heater_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_heater_test.cpp -o build/main_heater_test.o
	
main_transduc_test.o: test/main_transduc_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_transduc_test.cpp -o build/main_transduc_test.o
	
main_thermo_test.o: test/main_thermo_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_thermo_test.cpp -o build/main_thermo_test.o



tests: main_heater_test.o main_transduc_test.o main_thermo_test.o
		$(CC) -g -o bin/HEATER_TEST build/main_heater_test.o 
		$(CC) -g -o bin/TRANSDUC_TEST build/main_transduc_test.o
		$(CC) -g -o bin/THERMO_TEST build/main_thermo_test.o
  
#TARGET TO COMPILE ONLY HOUSETOP SIMULATOR
simulator: main_top.o
	$(CC) -g -o bin/HOUSETOP build/main_top.o

	
#TARGET TO COMPILE EVERYTHING (HOUSETOP SIMULATOR + TESTS TOGETHER)
all: simulator tests

#CLEAN COMMANDS
clean: 
	rm -f bin/* build/*
	
#joshil