uc=mpls
walker_mininet = walker_mininet
TOPO = topology/${uc}_topology.json
SWITCH_EXE = ../sw_dataplane/bin/main
CONTROLLER = ../sw_dataplane/bin/rp4_run_by_web

BUILD_DIR = build
PCAP_DIR = pcaps
LOG_DIR = logs

RP4 = p4/rp4/${uc}
RUN_SCRIPT = ${walker_mininet}/walker_mininet.py
BUILD_ENTRY = ${walker_mininet}/${uc}/${uc}_build_entry.py
LAUNCH_SERVER_SCRIPT = ${walker_mininet}/${uc}/${uc}_launch.py
UPDATE_SCRIPT = ${walker_mininet}/${uc}/${uc}_update.py
INIT_SCRIPT = ${walker_mininet}/${uc}/${uc}_init.py
SEND1 = ${walker_mininet}/${uc}/${uc}_1send.py
SEND2 = ${walker_mininet}/${uc}/${uc}_2send.py
SEND3 = ${walker_mininet}/${uc}/${uc}_3send.py
RECV = ${walker_mininet}/${uc}/${uc}_recv.py

all: mininet

mininet: stop
	sudo python $(RUN_SCRIPT) -t $(TOPO) -e $(SWITCH_EXE)

data_plane:
	sudo python $(BUILD_ENTRY)
	sudo python $(LAUNCH_SERVER_SCRIPT)

controller:
	rm -f ../sw_dataplane/temp/*
	sudo ${CONTROLLER}

init:
	sudo python $(BUILD_ENTRY)
	sh ${RP4}/auto.sh
#	sudo python $(INIT_SCRIPT)

update:
	sudo python $(BUILD_ENTRY)
	sh ${RP4}/auto_update.sh
#	sudo python $(UPDATE_SCRIPT)

send1:
	sudo python $(SEND1)

send2:
	sudo python $(SEND2)

send3:
	sudo python $(SEND3)

recv:
	sudo python $(RECV)

web_server:
	pm2 start /home/xilinx_0/webtest/apiserver/app.js
	sudo /usr/local/nginx/sbin/nginx -c /usr/local/nginx/conf/nginx.conf

stop:
	sudo mn -c

clean: stop
	rm -f *.pcap
	rm -rf $(BUILD_DIR) $(PCAP_DIR) $(LOG_DIR)
