LIBVERSION=0.3.0

repo:
	@git remote -v

version:
	@echo "libprogbase "$(LIBVERSION)

git-pull:
	@echo "Updating libprogbase..."
	@git pull origin master

update: git-pull install

install: 
	@echo "Building and installing libprogbase..."
	@rm ./build -rf 
	@mkdir ./build
	@cd ./build && cmake .. && make && sudo make install
	@rm ./build -rf <progbase.h>

build:
	cmake . && make
