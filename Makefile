all: encoder decoder

encoder : encoder.cpp
	g++ --std=c++11 encoder.cpp -o encoder

decoder: decoder.cpp
	    g++ --std=c++11 decoder.cpp -o decoder

