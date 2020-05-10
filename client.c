#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

typedef struct
{
    long long x;
    long long n;
}key;

key public;

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int power(int x, unsigned int y, int p)
{
    int res = 1;
    while (y != 0)
    {
        res = (res * (x % p)) % p;
        y--;
    }
    return res;
}


int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    bzero(buffer, 256);
    n = read(sockfd, buffer, 255);
    public.n = atoi(buffer);
    bzero(buffer, 256);
    char *ret_str = "received key";
    write(sockfd, ret_str, 30);

    //public key sending

    bzero(buffer, 30);
    n = read(sockfd, buffer, 30);
    public.x = atoi(buffer);
    printf("public key x,n = %d %d", public.x, public.n);
    char input[256];
    bzero(input, 256);
    printf("Please enter message\n");
    fgets(input, 256, stdin);
    int len = strlen(input);
    
    //sending len of string
    bzero(buffer, 256);
    sprintf(buffer, "%d", len);
    n = write(sockfd, buffer, 30);

    //encoding and sending string
    for (int i = 0; i < len; i++)
    {
        int c = input[i];
        c = power(c, public.x, public.n);
        bzero(buffer, 30);
        sprintf(buffer, "%d", c);
        n = write(sockfd, buffer, 30);
    }
    return 0;
}
