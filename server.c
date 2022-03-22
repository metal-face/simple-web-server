#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * @brief This function is called when a system call fails. 
 * It displays a message about the error on stderr and then 
 * aborts the program.
 * 
 * @param msg 
 */
void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    /*  
        sockfd and newsockfd are file descriptors.
        These two variables store the values returned
        by the socket system call and the accept system call 
    */ 
    int sockfd, newsockfd;
    
    // portNumber stores the port number on which the server accepts connections.
    int portNumber;

    /* 
        clilength stores the size of the address of the client.
        this is needed for the accept system call.
    */
    int clilength;

    // The server reads characters from the socket connection into this buffer. 
    char buffer[256];

    /*
        sockaddr_in is a structure containing an internet address.
        
        This struct is defined in <netinet/in.h>
        
        serv_addr will contain the address of the client which 
        connects to the server.
    */
    struct sockaddr_in serv_addr, cli_addr;
    
    /* 
        This is the return value for read() and write() calls
        i.e. it contains the number of characters read or written
    */
    int n;

    /*
      The user needs to pass in the port number on which 
      the server will accept connections as an argument.
      
      This code displays an error message if the user fails to do this.
    */
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    /* 
       The socket() system call creates a new socket.
       
       AF_INET is an internet domain for anu two hosts
       on the Internet. 
       
       SOCK_STREAM is a stream socket in which characters
       are read in a continuous stream as if from a file
       or pipe.
       
       The last argument is the socket protocol. If 0,
       the operating system will choose the most appropriate
       protocol.
    */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("ERROR opening Socket");
    }

    /*
        bzero() sets all values in a buffer to zero.

        serv_addr is a pointer to the buffer.

        The second argument is the size of the buffer.
    */
    bzero((char *) &serv_addr, sizeof(serv_addr));

    /*
        The port number on which the server will listen
        for connections.

        This uses atoi() to convert this from a string
        of digits to an integer.
    */
    portNumber = atoi(argv[1]);

    /*
        serv_addr is a structure of type struct sockaddr_in.
        This structure has four fields. The first is 
        'short sin_family', which contains code for the 
        address family. This should always be set to the
        symbolic constant AF_INET.
    */
    serv_addr.sin_family = AF_INET;

    /*
        This field contains the IP address of the host.
        For server code, this will always be the IP address
        of the machine on which the server is running, and
        there is a symbolic constant INADDR_ANY which gets
        this address.
    */
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    /*
        This field contains the port number. 
        
        Instead of simply copying the port number to this field, 
        it is necessary to convert this to 'network byte
        order' using the function htons(). 
        
        This converts a port number in 'host byte order' 
        to a port number in 'network byte order'.
    */
    serv_addr.sin_port = htons(portNumber);

    /*
        The bind system call binds a socket to an address.
        
        In this case, the address of the current host and port
        number on which the server will run.

        It takes three arguments: 
            - the socket file descriptor
            - the address to which it is bound
            - the size of the address to which it is bound
    */
    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding");
    }

    /*
        The 'listen' system call allows the process to listen
        on the socket for connections.

        The first arg is the socket file descriptor and the
        second is the size of the backlog queue, i.e., the
        number of connections that can be waiting while the 
        process is handling a particular connection.

        The second arg should be set to 5, the max size
        permitted by most systems.

        If the first argument is a valid socket, this call
        cannot fail, and so the code doesn't check for errors.
    */
    listen(sockfd, 5);

    /*
        The accept() system call causes the process to block
        until a client connects to the server. Thus, it wakes
        up the process when a connection from a client has been
        successfully established. It returns a new file descriptor,
        and all communication on this connection should be done
        using the new file descriptor.

        The second arg is a reference pointer to the address of the
        client on the other end of the connection.

        The last arg is the size of the stucture.
    */
    clilength = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilength);
    if (newsockfd < 0)
    {
        error("ERROR on acccept");
    }


    /*
        NOTE: We would only get to this point after a client
        has successfully connected to our server.

        This code initializes the buffer using the bzero() 
        function, and then reads from the socket.
    */
    bzero(buffer[256]);

    /*
        NOTE: the read() call uses the new file descriptor
        returned by the socket(). Also note, that the read()
        will block until there is something for it to read in
        the socket, i.e. after the client has executed a write().

        It will read either the total number of characters in the
        socket or 255, whichever is less, and return the number
        of characters read.
    */
    n = read(newsockfd, buffer, 255);
    if (n < 0)
    {
        error("ERROR reading from socket");
    }
    printf("Here is the message: %s\n", buffer);

    /*
        Once a connection has been established, both ends can both
        read and write to the connection. Naturally, everything
        written by the client will be read by the server, and everything
        written by the server will be read by the client.

        The last argument of write() is the size of the message.
    */
    n = write(newsockfd, "I got your message", 18);
    if (n < 0)
    {
        error("ERROR writing socket");
    }

    // terminate the program.
    return 0;
}