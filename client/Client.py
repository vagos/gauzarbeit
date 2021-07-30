import telnetlib

IP = "127.0.0.1"
PORT = 23


def main():

    try:

        with telnetlib.Telnet(IP, PORT) as telnetClient:

            telnetClient.interact()

        return

    except Exception as e:

        print(e)


if __name__ == "__main__":
    main()
