

all:
	$(MAKE) -C ./source bin/smbd
	install -m 755 ./source/bin/smbd $(INSTALL_DIR)/bin/smbd
#	install -m 755 ./source/bin/smbpasswd $(INSTALL_DIR)/bin/smbpasswd
	cp ./smb.conf $(INSTALL_DIR)/etc
	cp ./smb.passwd $(INSTALL_DIR)/etc
	

clean:
	if [ -f ./source/Makefile ];then $(MAKE) -C ./source clean;fi;

