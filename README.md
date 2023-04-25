1. To create executable files, put the following commands in terminal
       gcc -Wall server.c -o server
       gcc -Wall client.c -o client

2. To run the executables,
   ./server <port_number>
   ./client <ip_address> <port_number>

   Example =>
       ./server 4444 => from terminal 1
       ./client 127.0.0.1 4444 => from terminal 2
       
3. There are 5 Traders with trader id 1,2,3,4,5. Password of each trader is stored in passwd.txt file in line number equal to trader id.
	Example => for trader 1, password is 190101029

4. First we will be asked about p/q? If we enter 'p', that will take us to login part, else we will end our trading completely.

5. After login, we will be given y/n? If we enter 'y', that means the user will go to trading options, else the user will be logged out.

6. After that we will be given 1/2/3/4? options, which the user can use to do the operations.
