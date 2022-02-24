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
#include <algorithm> 

#include "potato.h"

using namespace std;

class player_info {
public:
    int player_fd;
    int listen_port; //The listening port of the client
    string ip;
    int id;
};

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
	//cout << "string info:" << p_info.ip << endl;
        send_waitall(p_info.player_fd, &i, sizeof(i)); //Send id to player
        send_waitall(p_info.player_fd, &num_players, sizeof(num_players));
        recv(p_info.player_fd, &(p_info.listen_port), sizeof(p_info.listen_port), MSG_WAITALL);
        player_infos.push_back(p_info);
        cout << "player " << i << " is ready to play" << endl;
    }
    
    //Notify the neighbor info for each player
    for (int i = 0; i < num_players; ++i) {
        int next_index = (i + 1) % num_players;
        int ip_size = player_infos[next_index].ip.length();
        send_waitall(player_infos[i].player_fd, 
                    &(ip_size), sizeof(ip_size)); //neigbor ip size
        send_waitall(player_infos[i].player_fd, 
                (void* ) player_infos[next_index].ip.c_str(), 
                player_infos[next_index].ip.length()); //neigbor ip
        send_waitall(player_infos[i].player_fd, 
                &(player_infos[next_index].listen_port),
                sizeof(player_infos[next_index].listen_port)); //neibor port
    }

    //Begin the game 
    srand((unsigned int)time(NULL));
    int random_start_player = rand() % num_players;

    if (num_hops > 0)
        cout << "Ready to start the game, sending potato to player " << random_start_player << endl;

    // potato p = {
    //     .index = 0,
    //     .remain_hops = num_hops
    // };
    potato p;
    p.index = 0;
    p.remain_hops = num_hops;
    memset(p.player_list, 512*sizeof(int), 0);
    send_waitall(player_infos[random_start_player].player_fd, &p, sizeof(p));

    vector<int> fd_list;
    for (auto& info: player_infos) {
        fd_list.push_back(info.player_fd);
    }

    int nfds = *max_element(fd_list.begin(), fd_list.end()) + 1;
    fd_set readfds;

    while (true) {
        FD_ZERO(&readfds);
        for (int i = 0; i < num_players; ++i) { 
            FD_SET(fd_list[i], &readfds);
        }
        
        select(nfds, &readfds, NULL, NULL, NULL);
        potato p;

        for (int i = 0; i < num_players; ++i) { 
            if (FD_ISSET(fd_list[i], &readfds)) {
                recv(fd_list[i], &p, sizeof(p), MSG_WAITALL);
                if (num_hops != 0) {
                    cout << "Trace of potato:" << endl;
                    for (int i = 0; i < num_hops; ++i) {
                        cout << p.player_list[i];
                        if (i != num_hops - 1) cout << ",";
                    }
                    cout << endl;
                }
                p.remain_hops = 0;
                
                //Close players
                for (auto fd: fd_list) {
                    send_waitall(fd, &p, sizeof(p));
                    close(fd);
                }
                return 0;
            }
        }
    }

    
}
