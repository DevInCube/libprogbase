LIB_PATH=/usr/local/lib/libprogbase.a

repo:
	@git remote -v

version:
	@echo "libprogbase " & strings $(LIB_PATH) | grep Version -m 1 | cut -d " " -f 2

update: 
	@sh scripts/install_latest_release.sh

build:
	cmake . && make
