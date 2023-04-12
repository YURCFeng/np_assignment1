#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <calcLib.h>
// Enable if you want debugging to be printed, see examble below.
// Alternative, pass CFLAGS=-DDEBUG to make, make CFLAGS=-DDEBUG
// #define DEBUG
#define SERVERPORT "5000" // the port users will be connecting to
#define MAXDATASIZE 10000
// Included to get the support library

void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{

  /*
    Read first input, assumes <ip>:<port> syntax, convert into one string (Desthost) and one integer (port).
     Atm, works only on dotted notation, i.e. IPv4 and DNS. IPv6 does not work if its using ':'.
  */
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  int numbytes;
  char buf[MAXDATASIZE];
  char bufline[MAXDATASIZE];
  char s[INET6_ADDRSTRLEN];
  char input[INET6_ADDRSTRLEN];

  char delim[] = ":";
  char *Desthost = strtok(argv[1], delim); // dizhi
  char *Destport = strtok(NULL, delim);
  int port = atoi(Destport); // 5000
  printf("Host %s, and port %d.\n", Desthost, port);
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }
  for (p = servinfo; p != NULL; p = p->ai_next)
  {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      perror("talker: socket");
      continue;
    }

    break;
  }
  if (p == NULL)
  {
    fprintf(stderr, "talker: failed to create socket\n");
    return 2;
  }

  if (connect(sockfd, p->ai_addr, p->ai_addrlen) < 0)
  {
    perror("talker2: connect .\n");
    exit(1);
  }
  else
  {
    char myAddress[20];

    struct sockaddr_in local_sin;
    socklen_t local_sinlen = sizeof(local_sin);
    getsockname(sockfd, (struct sockaddr *)&local_sin, &local_sinlen);

    inet_ntop(local_sin.sin_family, &local_sin.sin_addr, myAddress, sizeof(myAddress));
#ifdef DEBUG
#else
    printf("Connected to %s:%s  local %s:%d\n", Desthost, Destport, myAddress, ntohs(local_sin.sin_port));
#endif
  }

  // inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
  //           s, sizeof s);
  // printf("%s\n", p->ai_addr);
  freeaddrinfo(servinfo);

  if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
  {
    perror("recv");
    exit(1);
  }

  // buf[numbytes] = '\0';
  // bool supports_protocol_1_0 = strstr(buf, "TEXT TCP 1.0") != NULL;
  // bool supports_protocol_1_1 = strstr(buf, "TEXT TCP 1.1") != NULL;
  printf(" %s", buf);
  fgets(input, sizeof(input), stdin);

  if (strcmp(input, "OK\n") == 0)
  {
    char Word[] = "OK\n";
    numbytes = send(sockfd, Word, strlen(Word), 0);
    // printf("sent %d bytes\n", numbytes);

    double f1, f2, fresult;
    int i1, i2, iresult;
    char op[MAXDATASIZE];
    // printf(":>");
    memset(&buf, 0, sizeof buf);
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
    {
      perror("recv");
      exit(1);
    }
    if (numbytes == 0)
    {
      printf("got zero.\n");
    }
    printf("ASSIGNMENT: %s", buf);

    if (buf[0] == 'f')
    {
      // printf("Float\t");
      rv = sscanf(buf, "%s %lg %lg", op, &f1, &f2);
      if (rv == EOF)
      {
        printf("Sscanf1 failed.\n");
        // free(lineBuffer); // This is needed for the getline() as it will allocate memory (if the provided buffer is NUL).
        exit(1);
      }
      if (strcmp(op, "fadd") == 0)
      {
        fresult = f1 + f2;
      }
      else if (strcmp(op, "fsub") == 0)
      {
        fresult = f1 - f2;
      }
      else if (strcmp(op, "fmul") == 0)
      {
        fresult = f1 * f2;
      }
      else if (strcmp(op, "fdiv") == 0)
      {
        fresult = f1 / f2;
      }
      // printf("%s %8.8g %8.8g = %8.8g\n", op, f1, f2, fresult);
      char Word1[9] = {0};
      sprintf(Word1, "%f\n", fresult);

      numbytes = send(sockfd, Word1, strlen(Word1), 0);
      // printf("|%s|", Word1);
      // printf("sent %d bytes\n", numbytes);

      recv(sockfd, bufline, MAXDATASIZE - 1, 0);
      printf(" %s\n", bufline);

      printf("(mysesult=%8.8g)\n", fresult);
    }
    else
    {
      // printf("Int\t");
      rv = sscanf(buf, "%s %d %d", op, &i1, &i2);
      if (rv == EOF)
      {
        printf("Sscanf2 failed.\n");
        // free(lineBuffer); // This is needed for the getline() as it will allocate memory (if the provided buffer is NUL).
        exit(1);
      }
      if (strcmp(op, "add") == 0)
      {
        iresult = i1 + i2;
      }
      else if (strcmp(op, "sub") == 0)
      {
        iresult = i1 - i2;
      }
      else if (strcmp(op, "mul") == 0)
      {
        iresult = i1 * i2;
      }
      else if (strcmp(op, "div") == 0)
      {
        iresult = i1 / i2;
      }
      else
      {
        printf("No match\n");
      }
      // printf("%s %d %d = %d \n",op,i1,i2,iresult);
#ifdef DEBUG

#else
      printf("Calculated the result to %d\n", iresult);
#endif
      char Word2[9]{0};
      sprintf(Word2, "%d\n", iresult);
      numbytes = send(sockfd, Word2, strlen(Word2), 0);
      // printf("|%s|", Word2);
      // printf("sent %d bytes\n", numbytes);

      recv(sockfd, bufline, MAXDATASIZE - 1, 0);

      printf(" %s\n", bufline);

      printf("(mysesult=%d)\n", iresult);
    }
    close(sockfd);
  }
  else
  {
    close(sockfd);
    printf("Not Accpet And Client exit\n");
    exit(0);
  }
}
