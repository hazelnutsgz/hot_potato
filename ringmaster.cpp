#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class player_info {
public:
    int player_fd;
    int port; //Address info of the player
    string ip;
    int id;
};

int listen_step(const char* port) {
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    const char *hostname = NULL;

    memset(&host_info, 0, sizeof(host_info));

    host_info.ai_family   = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags    = AI_PASSIVE;

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        cerr << "Error: cannot get address info for host" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    } //if

    socket_fd = socket(host_info_list->ai_family, 
                host_info_list->ai_socktype, 
                host_info_list->ai_protocol);
    if (socket_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    } //if

    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot bind socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    }

    status = listen(socket_fd, 100);
    if (status == -1) {
        cerr << "Error: cannot listen on socket" << endl; 
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    }

    cout << "Waiting for connection on port " << port << endl;
    
    return socket_fd;
}

int main(int argc, char const *argv[])
{
    /* code */
    
    // Check arguments
    if (argc != 4) {
        cout << "format: ringmaster <port_num> <num_players> <num_hops>" << endl;
        return -1;
    }
    const char* port_number = argv[1];
    int num_players = atoi(argv[2]);
    int num_hops = atoi(argv[3]);

    if (num_players <= 1 || num_hops < 0 || num_hops > 512) return -1;
    int ringmaster_listen_fd = listen_step(port_number);

    cout << "Potato Ringmaster" << endl;
    cout << "Players = " << num_players << endl;
    cout << "Hops = " << num_hops << endl;

    vector<player_info> player_infos;
    //Try to accept all players.
    for (int i = 0 ; i < num_players; ++i) {
        player_info p_info;
        struct sockaddr_storage socket_addr;
        socklen_t socket_addr_len = sizeof(socket_addr);
        int client_connection_fd;
        client_connection_fd = 
            accept(ringmaster_listen_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
        if (client_connection_fd == -1) {
            cerr << "Error: cannot accept connection on socket" << endl;
            return -1;
        } //if
        p_info.player_fd = client_connection_fd;

        struct sockaddr_in *s_addr = (struct sockaddr_in *) &socket_addr;
        p_info.ip = inet_ntoa(s_addr->sin_addr);
        p_info.port = ntohs(s_addr->sin_port);

        send(p_info.player_fd, &i, sizeof(i), 0); //Send id to player

        player_infos.push_back(p_info);
    }

    //Notify the neighbor info for each player
    for (int i = 0; i < num_players; ++i) {

    }




    //Begin the game 
    int random_start_player = rand() % num_players;

    cout << "Ready to start the game, sending potato to player " << random_start_player << endl;
}
