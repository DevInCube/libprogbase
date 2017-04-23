LIB_PATH=/usr/local/lib/libprogbase.a

repo:
	@git remote -v

version:
	@echo "libprogbase " & strings $(LIB_PATH) | grep Version -m 1 | cut -d " " -f 2

git-pull:
	@echo "Updating libprogbase..."
	@git pull origin master
	@git fetch --tags
	latestTag=$(git describe --tags `git rev-list --tags --max-count=1`)
	@git checkout $latestTag
	@echo "Switch to release version " + $latestTag

update: git-pull install

install: 
	@echo "Building and installing libprogbase..."
	@rm ./build -rf 
	@mkdir ./build
	@cd ./build && cmake .. && make && sudo make install
	@rm ./build -rf

build:
	cmake . && make
