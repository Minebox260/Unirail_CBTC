all: client supervisor

CLIENT_SSH_IP1 = 192.168.1.173
CLIENT_SSH_IP2 = 192.168.1.172

CLIENT_SSH_USER = pi
CLIENT_SSH_PASS = raspberry
CLIENT_SSH_PATH = /home/pi/CBTC

client:
	$(MAKE) -C Client

supervisor:
	$(MAKE) -C Supervisor

clean:
	$(MAKE) -C Client clean
	$(MAKE) -C Supervisor clean

install:
	sshpass -p $(CLIENT_SSH_PASS) scp -r ./Client/* $(CLIENT_SSH_USER)@$(CLIENT_SSH_IP1):$(CLIENT_SSH_PATH)