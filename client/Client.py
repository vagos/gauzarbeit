import socket 
import threading

IP = "127.0.0.1"
PORT = 23

def GetServerData(socket):

    try:
        requestData = socket.recv( 1024 ).decode()
    except: 
        requestData = ""

    if requestData: print(requestData, end="")



def main():

    serverSocket = socket.socket( socket.AF_INET, socket.SOCK_STREAM )

    serverSocket.connect( (IP, PORT) )

    while (True):

        threading.Thread(target= lambda: GetServerData(serverSocket), name = "Client Thread").start()
        
        clientData = input("")

        serverSocket.send(clientData.encode("utf-8"))

    return 0


if __name__ == "__main__":
    main()
