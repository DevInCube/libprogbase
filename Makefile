repo:
	@git remote -v

version:
	@sh scripts/print_installed_version.sh

uninstall:
	@sh scripts/uninstall.sh

install: 
	@sh scripts/install.sh

update: 
	@sh scripts/install_latest_release.sh

build:
	cmake . && make
