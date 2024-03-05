#108303553's PA2 cpp compilier
.PHONY: all run clean
all: 108303553_PA2.o
	@g++ -std=c++11 108303553_PA2.o -o exe
run: 
	@./exe $(Testcase) $(Testcase).out
clean:
	@rm *.o
	@rm exe
108303553_PA2.o: 108303553_PA2.cpp
	@g++ -std=c++11 -c 108303553_PA2.cpp


