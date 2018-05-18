repo:
	@git remote -v

version:
	@sh scripts/print_installed_version.sh

uninstall:
	@sh scripts/uninstall.sh

install: 
	@sh scripts/install.sh

install-ssl: 
	@sh scripts/install_open_ssl.sh
	@sh scripts/install.sh

update: 
	@sh scripts/install_latest_release.sh

build:
	cmake . && make
