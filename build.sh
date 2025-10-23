gcc main.c crypto.c sysinfo.c network.c -o dwm_status_bar -lcurl -lm -O2 -lssh;
cp dwm_status_bar /usr/local/bin/dwm_status_bar
