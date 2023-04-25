#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include <arpa/inet.h>
#include <unistd.h>

void connection_close(char buffer[], int client_socket)
{
    buffer = "5\0";                                 // Type 3 message as it is used for closing the connection
    printf("CONNCECTION CLOSED\n");                   // Message on console to be printed
    write(client_socket, buffer, strlen(buffer));     // Write in client socket
}

char *pass[6];
void get_pass() {
	FILE *fp;
    if ((fp = fopen("passwd.txt","r")) == NULL){
        printf("Error! opening file");
        exit(1);
    }
    int i;
    for(i = 0; i < 6; ++i) {
    	pass[i] = malloc(20 * sizeof(char));
    }

    for(i = 1; i <= 5; ++i) {
    	fscanf(fp, "%s", pass[i]);
    }
    fclose(fp); 
    return;
}

int main(int argc, char* argv[]) {

	// trader login
	// init();
	get_pass();
	char buffer[1500]; // Buffer to store recieving and modified sending value
	char colon = ':';
	// printf("%d", trader_id);

	// connect to the server
	int client_socket = socket(AF_INET, SOCK_STREAM, 0); 
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	int server_port = atoi(argv[2]);
    server.sin_port = htons(server_port);

	int status = connect(client_socket, (struct sockaddr *) &server, sizeof(server));
	if(status == -1) {
	 	printf("ERROR CONNECTING TO THE SERVER\n");
	 	exit(0);
	}

	printf("General Rules:\nWhenever asked for 1/2/3/4:\n1: Send Buy Request\n2: Send Sell Request\n3: View Order Status\n4: View Trade Status\n\n");
	printf("Whenever asked for y/n:\ny: Continue, else: Logout User\n\n");
	printf("Whenever asked for p/q:\np: Proceed, else: Terminate Trading\n\n");

   // trades

	while (1) {
		printf("p/q?\n");
		char p = getchar();
		getchar();
		if(p != 'p') {
			connection_close(buffer, client_socket);
			return 0;
		}
		printf("Enter Trader Code: ");
		int trader_id;
		scanf("%d", &trader_id);
		getchar();
		printf("Enter Trader Password: ");
		char *pas;
		pas = malloc(20 * sizeof(*pas));
		scanf("%s", pas);
		getchar();
		printf("\n");
		if(trader_id > 5 || trader_id < 1) {
			printf("Invalid Trader.\n");
			continue;
		}

		if(strcmp(pass[trader_id], pas) != 0) {
			printf("Incorrect Password.\n");
			continue;
		} 
		char trader = (char)(trader_id + '0');
		while (1) {
	    	printf("y/n?\n");
	    	char y = getchar();
	    	getchar();
	    	if(y != 'y') {
	    		// connection_close(buffer, client_socket);
	    		break;
	    	}	
	    	
	    	bzero(buffer, 1500);
	    	buffer[0] = '\0';
	    	printf("1/2/3/4?\n");
	    	char type = getchar();
	    	getchar();
	    	if (type == '1') {
	    		// send buy request to the server
	    		int item_id, quantity, unit_price;
	    		printf("Item Code : ");
	    		scanf("%d", &item_id);
	    		getchar();
	    		printf("Quantity  : ");
	    		scanf("%d", &quantity);
	    		getchar();
	    		printf("Unit Price: ");
	    		scanf("%d", &unit_price);
	    		getchar();
	    		buffer[0] = '\0';
	    		char one = '1';
	    		strncat(buffer, &one, 1);
	    		strncat(buffer, &colon, 1);
	    		strncat(buffer, &trader, 1);
	    		strncat(buffer, &colon, 1);
	    		char I[20], Q[20], U[20];
	    		snprintf(I, 20, "%d", item_id);
	    		snprintf(Q, 20, "%d", quantity);
	    		snprintf(U, 20, "%d", unit_price);
	    		strncat(buffer, I, 20);
	    		strncat(buffer, &colon, 1);
	    		strncat(buffer, Q, 20);
	    		strncat(buffer, &colon, 1);
	    		strncat(buffer, U, 20);
	    		write(client_socket, buffer, strlen(buffer));
	    	}
	    	else if (type == '2') {
	    		// send sell request to the server
	    		int item_id, quantity, unit_price;
	    		printf("Item Code : ");
	    		scanf("%d", &item_id);
	    		getchar();
	    		printf("Quantity  : ");
	    		scanf("%d", &quantity);
	    		getchar();
	    		printf("Unit Price: ");
	    		scanf("%d", &unit_price);
	    		getchar();
	    		buffer[0] = '\0';
	    		char two = '2';
	    		strncat(buffer, &two, 1);
	    		strncat(buffer, &colon, 1);
	    		strncat(buffer, &trader, 1);
	    		strncat(buffer, &colon, 1);
	    		char I[20], Q[20], U[20];
	    		snprintf(I, 20, "%d", item_id);
	    		snprintf(Q, 20, "%d", quantity);
	    		snprintf(U, 20, "%d", unit_price);
	    		strncat(buffer, I, 20);
	    		strncat(buffer, &colon, 1);
	    		strncat(buffer, Q, 20);
	    		strncat(buffer, &colon, 1);
	    		strncat(buffer, U, 20);
	    		write(client_socket, buffer, strlen(buffer));
	    	}
	    	else if (type == '3') {
	    		// view order status
	    		buffer[0] = '\0';
	    		char three = '3';
	    		strncat(buffer, &three, 1);
	    		write(client_socket, buffer, strlen(buffer));
	    		read(client_socket, buffer, 1500);
	    		printf("%s", buffer);

	    	}
	    	else if (type == '4'){
	    		// view trade status
	    		buffer[0] = '\0';
	    		char four = '4';
	    		strncat(buffer, &four, 1);
	    		strncat(buffer, &colon, 1);
	    		strncat(buffer, &trader, 1);
	    		write(client_socket, buffer, strlen(buffer));
	    		read(client_socket, buffer, 1500);
	    		printf("%s", buffer);
	    	}
	    	else {
	    		printf("Invalid Operation Request\n");
	    	}
	    	printf("\n");
	    	// function(type);
	    }
	}
    


    close(client_socket);
    // fclose(fp); 
	return 0;
}