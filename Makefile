repo:
	@git remote -v

version:
	@sh scripts/print_installed_version.sh

update: 
	@sh scripts/install_latest_release.sh

build:
	cmake . && make
