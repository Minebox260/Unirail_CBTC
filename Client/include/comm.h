#define MAXOCTETS 150
#define MAXDATA 10
typedef struct {
	int id;
	int code;
	char *data[MAXDATA];
} message_t;

void send_data(const int sd, const struct sockaddr_in * send_adr, const message_t message);
void receive_data(const int sd, struct sockaddr_in * recv_adr, message_t *message);