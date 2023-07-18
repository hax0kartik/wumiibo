source /etc/profile.d/devkit-env.sh

apt-get update && apt-get install p7zip-full -y

# Install ctrulib
cd build_dir
git clone https://github.com/smealum/ctrulib.git
cd ctrulib/libctru
make install release -j

# Install makerom and ctrtool
cd /build_dir
export ctr_ver=16
curl -LJO https://github.com/jakcron/Project_CTR/releases/download/v0."$ctr_ver"/makerom_0"$ctr_ver"_ctrtool.zip
7z e makerom_0"$ctr_ver"_ctrtool.zip -o/usr/bin Ubuntu/makerom
7z e makerom_0"$ctr_ver"_ctrtool.zip -o/usr/bin Ubuntu/ctrtool
chmod +x /usr/bin/makerom && chmod +x /usr/bin/ctrtool

#Build wumiibo
make clean && make -j2

#Create a folder for later use
mkdir 0004013000004002
mv 0004013000004002.cxi ./0004013000004002/0004013000004002.cxi

#Build helper
cd wumiibohelper
make clean && make -j
mv wumiibohelper.cia ../0004013000004002/wumiibohelper.cia