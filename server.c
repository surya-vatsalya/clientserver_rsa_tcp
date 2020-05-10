#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>

typedef struct
{
    int x;
    int n;
}key;

key public;
key private;

typedef struct 
{
 int d;
 int x;
 int y;
} EE;
 
int modulo(int x, int n)
{
    return (x%n+n)%n;
}

 EE extended_euclid(int a, int b) {
 EE ee1, ee2, ee3;
 if (b == 0) {
  ee1.d = a;
  ee1.x = 1;
  ee1.y = 0;
  return ee1;
 } else {
  ee2 = extended_euclid(b, a % b);
  ee3.d = ee2.d;
  ee3.x = ee2.y;
  ee3.y = ee2.x - floor(a / b) * ee2.y;
  return ee3;
 }
}

int euclid(int e, int phi){
 EE ee;
 ee = extended_euclid(e, phi);
 return modulo(ee.x, phi); 
}


void error(char *msg)
{
    perror(msg);
    exit(1);
}

int gcd(int a, int b)
{
    if (a == 0)
        return b;
    return gcd(b % a, a);
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
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    int portno = atoi(argv[1]);
    int sockfd, newsockfd, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    //RSA Algorithm calculations
    //setting primes

    int p = 401;
    int q = 107;


    public.n = p * q;
    private.n = public.n;
    int phi = (p - 1) * (q - 1);

    //calculating public key
    public.x = 229;
    //manually calucating a prime which has gcd of 1 with phi

    //calculating private key
    private.x = euclid(public.x,phi);

    printf("generated keys are following :\n");
    printf("public = %d, %d\n",public.x, public.n);
    printf("private = %d, %d\n", private.x, private.n);

    //send public key

    bzero(buffer, 256);
    char buff_new[30];
    sprintf(buff_new, "%d", public.n);
    n = write(newsockfd, buff_new, 30);
    bzero(buff_new, 30);
    n = read(newsockfd, buff_new, 30);
    bzero(buff_new, 30);
    sprintf(buff_new, "%d", public.x);
    n = write(newsockfd, buff_new, 30);
    bzero(buff_new, 30);


    n = read(newsockfd, buff_new, 30);
    int length = atoi(buff_new);
    for (int i = 0; i < length; i++)
    {
        bzero(buff_new, 30);
        n = read(newsockfd, buff_new, 30);
        int c = atoi(buff_new);
        c = power(c, private.x, private.n);
        printf("%c", c);
    }
    return 0;
}
