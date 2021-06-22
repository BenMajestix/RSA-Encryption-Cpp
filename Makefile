##
#
#
# @file
# @version 0.1

all: build

build: rsa-encrypt.cpp
	c++ -o rsa-key-gen rsa-encrypt.cpp -lgmp

run: rsa-key-gen
	./rsa-key-gen

clean: rsa-key-gen
	rm rsa-key-gen

# end
