### Download, compilação e instalação de uma nova versão do Linux ###

#Download do kernel
wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.10.47.tar.xz

#Descompressão do kernel
unxz -v linux-5.10.47.tar.xz

#Download da assinatura digital do kernel
wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.10.47.tar.sign

#Verificação da assinatura do kernel
gpg --keyserver hkps://keyserver.ubuntu.com --auto-key-retrieve --verify linux-5.10.47.tar.sign

#Extração do diretório com os arquivos do kernel
tar xvf linux-5.10.47.tar
cd linux-5.10.47

#Criação do arquivo de configuração
cp -v /boot/config-$(uname -r) .config
vim .config #Edit version and key

#Altere as seguintes linhas no arquivo de configuração
#CONFIG_BUILD_SALT="5.10.47"
#CONFIG_SYSTEM_TRUSTED_KEYS=""

#Instalação de alguns utilitárioas para a compilação
sudo apt install build-essentials libncurses-dev bison flex libssl-dev libelf-dev

#Compilação do kernel
make

#Instalação do kernel
sudo make modules_install
sudo make install

#Alteração do kernel no sistema
cd /
sudo rm vmlinuz
sudo ln -s boot/vmlinuz-5.10.47 vmlinuz

#Reinicialização
reboot #ou 'shutdown -r now' ou 'systemctl poweroff'


#Se der problema para bootar tente novamente com Ctrl + Alt + Del (na máquina virtual é necessário pressionar a tecla host também)
#Se ainda não bootar, entre com uma iso de instalação e refaça o link simbólico original:
#mkdir temp
#mount temp /dev/sda<particao_raiz>
#chroot temp
#cd /
#rm vmlinuz
#ln -s boot/vmlinuz-<versao_original_do_kernel> vmlinuz
