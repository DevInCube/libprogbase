LIBVERSION=0.2.0
CC=gcc
LOCALDIR=/usr/local

repo:
	@git remote -v

version:
	@echo "libprogbase "$(LIBVERSION)

git-pull:
	@echo "Updating libprogbase..."
	@git pull origin master

update: git-pull install

install: 
	@echo "Building and installing libprogbase..." $@.a
	@rm ./build -rf 
	@mkdir ./build
	@cd ./build && cmake .. && make && sudo make install
	@rm ./build -rf 
