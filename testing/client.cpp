
#include <SFML/Network.hpp>
#include <SFML/Network/Socket.hpp>
#include <iostream>

int main()
{
    sf::TcpSocket socket;

    sf::Socket::Status status = socket.connect("127.0.0.1", 1234);
    
    if (status != sf::Socket::Done)
    {
        std::cout << "Error!";
    }

    char data[100] = "Hello world!\r\n"; 

    if (socket.send(data, 100) != sf::Socket::Done)
    {
        std::cout << "Error!";
    }

    return 0;
}
