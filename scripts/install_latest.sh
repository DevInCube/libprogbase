GREEN='\033[1;32m'
NC='\033[0m'

echo "Updating libprogbase..."
git pull origin master
git fetch --tags
latestTag=$(git describe --tags `git rev-list --tags --max-count=1`)
git checkout $latestTag
printf "${GREEN}Installing libprogbase $latestTag:${NC}\n"
echo "Building and installing libprogbase..."
rm ./build -rf 
mkdir ./build
cd ./build && cmake .. && make && sudo make install && cd ../
rm ./build -rf
printf "${GREEN}Checking installed version:${NC}\n"
make version
