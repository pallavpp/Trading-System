#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>


int min(int a, int b) {
    if(a < b) return a;
    return b;
}
int SIZE = 1800;
char buffer[1500];
int N; // number_of_trades;
int B; // number_of_buying_requests;
int S; // number_of_selling_requests;
struct trade{
    int seller_id;
    int buyer_id;
    int item_id;
    int quantity;
    int unit_price;
} *T;

struct buy_queue{
    int buyer_id;
    int item_id;
    int quantity;
    int unit_price;
} *BQ;

struct sell_queue{
    int seller_id;
    int item_id;
    int quantity;
    int unit_price;
} *SQ;

void init(int n, int b, int s, struct trade *t, struct buy_queue *bq, struct sell_queue *sq) {
    T = malloc(SIZE * sizeof(*T));
    BQ = malloc(SIZE * sizeof(*BQ));
    SQ = malloc(SIZE * sizeof(*SQ));
    N = n; B = b; S = s;
    int i;
    for(i = 0; i < n; ++i) {
        T[i] = t[i];
    }
    for(i = 0; i < b; ++i) {
        BQ[i] = bq[i];
    }
    for(i = 0; i < s; ++i) {
        SQ[i] = sq[i];
    }
}



void buy(int trader_id, int item_id, int quantity, int unit_price) {
    while (1) {
        if (quantity == 0) {
            break;
        }
        int best_buy = -1;
        int seller = -1;
        int i;
        for(i = 0; i < S; ++i) {
            // see all the sellers
            if (SQ[i].seller_id != trader_id && SQ[i].item_id == item_id && SQ[i].quantity != 0 && SQ[i].unit_price <= unit_price) {
                if (best_buy == -1) {
                    best_buy = SQ[i].unit_price;
                    seller = i;
                }
                else if (best_buy > SQ[i].unit_price) {
                    seller = i;
                    best_buy = SQ[i].unit_price;
                }
            }
        }
        if (seller == -1) {
            BQ[B].buyer_id = trader_id;
            BQ[B].item_id = item_id;
            BQ[B].quantity = quantity;
            BQ[B].unit_price = unit_price;
            B++;
            break;
        }
        else {
            T[N].buyer_id = trader_id;
            T[N].seller_id = SQ[seller].seller_id;
            T[N].item_id = item_id;
            T[N].quantity = min(quantity, SQ[seller].quantity);
            T[N].unit_price = best_buy;
            quantity -= T[N].quantity;
            SQ[seller].quantity -= T[N].quantity;
            N++;
        }
    }
}

void sell(int trader_id, int item_id, int quantity, int unit_price) {
    // FCFS
    int i;
    for(i = 0; i < B; ++i) {
        if (quantity == 0) {
            break;
        }
        if (BQ[i].buyer_id != trader_id && BQ[i].item_id == item_id && BQ[i].quantity != 0 && BQ[i].unit_price >= unit_price) {
            T[N].buyer_id = BQ[i].buyer_id;
            T[N].seller_id = trader_id;
            T[N].item_id = item_id;
            T[N].quantity = min(quantity, BQ[i].quantity);
            T[N].unit_price = unit_price;
            quantity -= T[N].quantity;
            BQ[i].quantity -= T[N].quantity;
            N++;
        } 
    }

    if(quantity != 0) {
        SQ[S].seller_id = trader_id;
        SQ[S].item_id = item_id;
        SQ[S].quantity = quantity;
        SQ[S].unit_price = unit_price;
        S++;
    }
}

void order_status(/*int trader_id*/) {
    bzero(buffer, 1500);
    sprintf(buffer + strlen(buffer),"The full History of Orders are:\n");
    int i;
    for(i = 0; i < N; ++i) {
        sprintf(buffer + strlen(buffer),"Item = %d, Seller = %d, Buyer = %d, Quantity = %d, Unit Price = %d\n", T[i].item_id, T[i].seller_id, 
            T[i].buyer_id, T[i].quantity, T[i].unit_price);
    }
    sprintf(buffer + strlen(buffer),"\n\n");   
    int best_buy[11], best_sell[11];
    int sell_quantity[11], buy_quantity[11]; 
    for(i = 1; i <= 10; ++i) {
        best_buy[i] = -1;
        best_sell[i] = -1;
        sell_quantity[i] = 0;
        buy_quantity[i] = 0;
    }

    for(i = 0; i < S; ++i) {
        // if(SQ[i].seller_id == trader_id) continue;
        int item = SQ[i].item_id, price = SQ[i].unit_price;
        if(SQ[i].quantity != 0) {
            if(best_sell[item] == -1) {
                best_sell[item] = price;
                sell_quantity[item] = SQ[i].quantity;
            }
            else if(best_sell[item] > price) {
                best_sell[item] = price;
                sell_quantity[item] = SQ[i].quantity;
            }
        }
    }

    for(i = 0; i < B; ++i) {
        // if(BQ[i].buyer_id == trader_id) continue;
        int item = BQ[i].item_id, price = BQ[i].unit_price;
        if(BQ[i].quantity != 0) {
            if(best_buy[item] == -1) {
                best_buy[item] = price;                
                buy_quantity[item] = BQ[i].quantity;
            }
            else if(best_buy[item] < price) {
                best_buy[item] = price;
                buy_quantity[item] = BQ[i].quantity;
            }
        }
    }

    for(i = 1; i <= 10; ++i) {
        sprintf(buffer + strlen(buffer),"Item = %d, Best Buy = %d, Best Sell = %d, Buy Quantity = %d, Sell Quantity = %d\n", i, best_buy[i], 
            best_sell[i], buy_quantity[i], sell_quantity[i]);
    }
}

void trade_status(int trader_id) {
    int i;
    buffer[0] = '\0';
    sprintf(buffer + strlen(buffer), "\nSelling History of Trader %d:\n", trader_id);
    for(i = 0; i < N; ++i) {
        if(T[i].seller_id == trader_id) {
            sprintf(buffer + strlen(buffer), "Item = %d, Buyer = %d, Quantity = %d, Unit Price = %d\n", T[i].item_id, 
            T[i].buyer_id, T[i].quantity, T[i].unit_price);
        }
    }
    sprintf(buffer + strlen(buffer), "\nBuying History of Trader %d:\n", trader_id);
    for(i = 0; i < N; ++i) {
        if(T[i].buyer_id == trader_id) {
            sprintf(buffer + strlen(buffer), "item = %d, Seller = %d, Quantity = %d, Unit Price = %d\n", T[i].item_id, 
            T[i].seller_id, T[i].quantity, T[i].unit_price);
        }
    }
    // write()
}

void rcv_ack(int socket_fd, struct sockaddr_in *client)
{
    char *ip = inet_ntoa(client->sin_addr);
    // Convert Internet number in IN to ASCII representation. The return value
    // is a pointer to an internal array containing the string.
    // i.e  the client IP address

    int port = client->sin_port;
    // Port of the client

    printf("\nNEW CLIENT CONNECTION [%s:%d] ESTABLISHED\n", ip, port);

     //Buffer used to store,sending messages

    // Wait for message from client till close connection request is recieved
    while (1) {
        bzero(buffer, 1500);                    // Set the 1500 bytes of buffet as 0
        buffer[0] = '\0';
        int st = read(socket_fd, buffer, 1500); // Read the message from the socket
        if (buffer[0] == '1') {

            printf("Someone asking to buy\n");
            int trader_id = buffer[2] - '0', item_id = 0, quantity = 0, unit_price = 0;
            int i = 4;
            char I[20], Q[20], U[20];
            I[0] = '\0';
            Q[0] = '\0';
            U[0] = '\0';
            int p = 1;
            //printf("%s\n", buffer);
            while (1) {
                if (buffer[i] == ':') {
                    int j = 0;
                    while (p > 1) {
                        p /= 10;
                        item_id += (p * (I[j] - '0'));
                        ++j;
                    }
                    ++i;
                    break;
                }
                else {
                    strncat(I, &buffer[i], 1);
                    p *= 10;
                }
                ++i;
            }
            p = 1;
            while (1) {
                if (buffer[i] == ':') {
                    int j = 0;
                    while (p > 1) {
                        p /= 10;
                        quantity += (p * (Q[j] - '0'));
                        ++j;
                    }
                    ++i;
                    break;
                }
                else {
                    strncat(Q, &buffer[i], 1);
                    p *= 10;
                }
                ++i;
            }
            p = 1;
            while (1) {
                if (buffer[i] == '\0') {
                    for(int j = 0; j < strlen(U); ++j) {
                        p /= 10;
                        unit_price += (p * (U[j] - '0'));
                    }
                    break;
                }
                else {
                    strncat(U, &buffer[i], 1);
                    p *= 10;
                }
                ++i;
            }

            // printf("%d\n%d\n%d\n", item_id, quantity, unit_price);

            // printf("%s\n", buffer);
            buy(trader_id, item_id, quantity, unit_price);


            // printf("\nMessage received from client %s : %d\n\tEncoded Message: %s\n", ip, port, buffer + 1); // Print the encoded message
            // printf("\tDecoded Message: %s\n", decoder(buffer + 1));                                           // Print the decoded message

            bzero(buffer, 1500);                      // Set the 1500 bytes of buffet as 0
            // buffer[0] = '2';                          // Message type 2 refers to acknowledgement by the server
            // strcpy(buffer + 1, encoder("ACK"));        // Append the encoded value of "ACK" after the message type
            // write(socket_fd, buffer, strlen(buffer)); // Send the acknowledgement

        }
        else if (buffer[0] == '2') {
            int trader_id = buffer[2] - '0', item_id = 0, quantity = 0, unit_price = 0;
            int i = 4;
            char I[20], Q[20], U[20];
            I[0] = '\0';
            Q[0] = '\0';
            U[0] = '\0';
            int p = 1;
            //printf("%s\n", buffer);
            while (1) {
                if (buffer[i] == ':') {
                    int j = 0;
                    while (p > 1) {
                        p /= 10;
                        item_id += (p * (I[j] - '0'));
                        ++j;
                    }
                    ++i;
                    break;
                }
                else {
                    strncat(I, &buffer[i], 1);
                    p *= 10;
                }
                ++i;
            }
            p = 1;
            while (1) {
                if (buffer[i] == ':') {
                    int j = 0;
                    while (p > 1) {
                        p /= 10;
                        quantity += (p * (Q[j] - '0'));
                        ++j;
                    }
                    ++i;
                    break;
                }
                else {
                    strncat(Q, &buffer[i], 1);
                    p *= 10;
                }
                ++i;
            }
            p = 1;
            while (1) {
                if (buffer[i] == '\0') {
                    for(int j = 0; j < strlen(U); ++j) {
                        p /= 10;
                        unit_price += (p * (U[j] - '0'));
                    }
                    break;
                }
                else {
                    strncat(U, &buffer[i], 1);
                    p *= 10;
                }
                ++i;
            }

            // printf("%d\n%d\n%d\n", item_id, quantity, unit_price);

            // printf("%s\n", buffer);
            sell(trader_id, item_id, quantity, unit_price);
            bzero(buffer, 1500); 

        }
        else if (buffer[0] == '3') {
            // int trader_id = buffer[2] - '0';
            order_status();
            write(socket_fd, buffer, strlen(buffer));
        }
        else if (buffer[0] == '4') {
            int trader_id = buffer[2] - '0';
            trade_status(trader_id);
            write(socket_fd, buffer, strlen(buffer));
        }
        else {
            break;
        }
    }

    close(socket_fd); // Close the  connection
    printf("\nCLIENT CONNECTION [%s:%d] CLOSED\n", ip, port);
    exit(0);
}

int main(int argc, char *argv[]) {
    // init();
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); //AF_NET is for IPv4, SOCK_STREAM indicates that TCP socket is created and 0 is for IP

    // Unable to create the server socket
    if (server_socket == -1) {
        printf("SERVER SOCKET CREATION FAILED\n");
        exit(0);
    }

    struct sockaddr_in server, client;

    server.sin_family = AF_INET;                   // code for address family - IPv4
    server.sin_addr.s_addr = INADDR_ANY;           // Address of host set to accept any incoming messages. Binds the socket to all available local interfaces
    server.sin_port = htons(atoi(argv[1]));        // Endianness of server port number changed
    memset(&server.sin_zero, 8, 0);                // Pad to size of `struct sockaddr' i.e a array of size 8.
    socklen_t length = sizeof(struct sockaddr_in); // Length of the server socket

    //If the server port is already in use then the binding fails
    if (bind(server_socket, (struct sockaddr *)&server, length) < 0)
    {
        printf("\nBINDING FAILED\n");
        exit(0);
    }

    // Prepare to accept connections on server socket.
    // 25 connection requests will be queued before further requests are refused.
    // Returns 0 on success, -1 for errors.

    if (listen(server_socket, 25) == -1) {
        printf("\nLISTEN FAILED\n");
        exit(0);
    }
    printf("SERVER WORKING\n");

    fflush(stdout); // This function is a possible cancellation point and therefore not marked with __THROW.

    while (1) {
        int client_socket = accept(server_socket, (struct sockaddr *)&client, &length); // Client tries to connect to the socket

        // Server client connection could not be made
        if (client_socket < 0) {
            printf("\nSERVER-CLIENT CONNECTION COULD NOT BE ESTABLISHED\n");
            exit(0);
        }

        int concurrent = fork();
        // Fork used to create a child process to handle the client
        // Hence multiple clients can be handled concurrently.

        if (concurrent == 0) {
            init(N, B, S, T, BQ, SQ);
            close(server_socket);            // Server socket is handled by parent process
            rcv_ack(client_socket, &client); // Used to handle a client
        }
        else if (concurrent == -1) {
            // error while creating child process
            printf("\nCOULDN'T ESTABLISH CONNECTION\n");
        }
        else {
            close(client_socket); // Client socket handled by a child process
        }
    }

    return 0;
}