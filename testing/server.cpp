#include "SFML/Network.hpp"
#include <SFML/Network/Socket.hpp>
#include <bits/c++config.h>
#include <iostream>

int main()
{
        sf::TcpListener listener;

       // bind the listener to a port
        if (listener.listen(1234) != sf::Socket::Done)
        {
            // error...
        }

        // accept a new connection
        sf::TcpSocket client;
        if (listener.accept(client) != sf::Socket::Done)
        {
           return 1; 
        }

        std::string sGreeting{"\e[2J\e[HSuccesfully connected!\r\n"};

        client.send(sGreeting.data(), sGreeting.size());

        char data[100];
        std::size_t received;

        while (true)
        {
            if (client.receive(data, 100, received) == sf::Socket::Done)
            {
                if (data[received - 1] == '\n')
                {
                    for (int i = 0; i < received; i++)
                    {
                        std::cout << data[i];
                    }

                    std::cout << " Received: " << received;
                }
            }

        }

        

    return 0;
}
