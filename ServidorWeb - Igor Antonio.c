#include <io.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#pragma comment(lib,"ws2_32.lib")
#define PORT 8080
#define MESSAGESIZE 255
#define MAXSOCKETS 10
#define ROOTDIR "D:\www"
#define DEFAULTFILE "index.html"

SOCKET new_socket[MAXSOCKETS];
int qtdsockets = 0;


int TipoRequisicao(char message[], char *filename) {
    if (strncmp(message, "GET ", 4) == 0) {
        sscanf(message + 4, "%s", filename);
        // Se o arquivo requisitado for "/", serve "index.html"
        if (strcmp(filename, "/") == 0) {
            strcpy(filename, "/" DEFAULTFILE);
        }
        return 1;
    }
    return 0;
}

int lerHTML(char** pagina, const char* caminho, int encontrado) {
    FILE *arquivo;
    int tamanhoPagina = 0, indice = 0, caractere;

    if (encontrado) {
        arquivo = fopen(caminho, "r");
    } else {
        arquivo = fopen(ROOTDIR "/notFound.html", "r");
    }

    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return -1;
    }

    fseek(arquivo, 0, SEEK_END);
    tamanhoPagina = ftell(arquivo);
    fseek(arquivo, 0, SEEK_SET);

    *pagina = (char *)malloc(tamanhoPagina + 1);
    if (*pagina == NULL) {
        perror("Erro na alocação de memória");
        fclose(arquivo);
        return -1;
    }

    while ((caractere = fgetc(arquivo)) != EOF) {
        (*pagina)[indice++] = (char)caractere;
    }
    (*pagina)[indice] = '\0';
    fclose(arquivo);

    return tamanhoPagina;
}
void pegaData(char data[])
{
    time_t rawtime;
    struct tm *info;
    time(&rawtime);
    info = gmtime(&rawtime);
    strftime(data, 80, "%a, %d %Y %X GMT", info);
}

int montaHttp(char **cabecalhoHttp, int ok, char data[], int tamanhoPagina, char **status) {
    if (ok) {
        *status = strdup("200 OK");
    } else {
        *status = strdup("404 Not Found");
    }

    *cabecalhoHttp = (char *)malloc(512);  // Aloca memória para o cabeçalho
    if (*cabecalhoHttp == NULL) {
        perror("Erro na alocação de memoria");
        return 0;
    }

    snprintf(*cabecalhoHttp, 512,
             "HTTP/1.1 %s\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %d\r\n"
             "Date: %s\r\n"
             "\r\n\r\n",
             *status, tamanhoPagina, data);

    printf("\n\n%s", *cabecalhoHttp);
    return 1;
}

void getdata(int pos) {
    char message[MESSAGESIZE], *pagina;
    char caminho[512], *cabecalhoHttp, *status;
    char encerramento[] = "\r\n\r\n";
    int len, encontrado = 1, tamanhoPagina;
    time_t agora = time(NULL); // Para pegar a data e hora
    char data[80];

    message[0]='\0';
    len = recv(new_socket[pos], message, MESSAGESIZE, 0);

    if (len > 0) {
        message[len] = '\0';  // Garantir que a mensagem seja nula terminada

        if (TipoRequisicao(message, caminho)) {
            // Construir o caminho completo
            char fullPath[512];
            snprintf(fullPath, sizeof(fullPath), "%s%s", ROOTDIR, caminho);

            FILE *arquivo = fopen(fullPath, "r");
            if (arquivo == NULL) {
                encontrado = 0;
                snprintf(fullPath, sizeof(fullPath), "%s/notFound.html", ROOTDIR);
            }
            tamanhoPagina = lerHTML(&pagina, fullPath, encontrado);
            pegaData(data);
            // Monta o cabeçalho HTTP usando a função montaHttp
            if (montaHttp(&cabecalhoHttp, encontrado, data, tamanhoPagina, &status)) {
                send(new_socket[pos], cabecalhoHttp, strlen(cabecalhoHttp), 0);
                send(new_socket[pos], pagina, tamanhoPagina, 0);
                free(cabecalhoHttp);
                free(status);
            }
            free(pagina);
        } else {
            // Responder com erro "Método não implementado"
            const char *erro501 = "HTTP/1.1 501 Not Implemented\r\nContent-Length: 0\r\n\r\n";
            send(new_socket[pos], erro501, strlen(erro501), 0);
        }
    }
    closesocket(new_socket[pos]);
}


int main(int argc , char *argv[]) {
  WSADATA wsa;
  SOCKET s;
  struct sockaddr_in server , client;
  int c, pos;


  printf("*** SERVER ***\n\nAguardando conexoes...\n\n");

  if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
    printf("\nFalha na inicializacao da biblioteca Winsock: %d",WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET) {
    printf("\nNao e possivel inicializar o socket: %d" , WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( PORT );

  if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR) {
    printf("\nNao e possivel construir o socket: %d" , WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  listen(s,3);
  c = sizeof(struct sockaddr_in);

  while(1) {
    pos=qtdsockets;
    new_socket[qtdsockets++] = accept(s, (struct sockaddr *)&client, &c);
    if (new_socket[pos] == INVALID_SOCKET) {
        printf("\nConexao não aceita. Codigo de erro: %d" , WSAGetLastError());
    } else {
        qtdsockets++;
        puts("\nConexao aceita.");
        printf("\nDados do cliente - IP: %s -  Porta: %d\n",inet_ntoa(client.sin_addr),htons(client.sin_port));
        _beginthread(getdata,NULL,pos);
    }
  }
  closesocket(s);
  WSACleanup();
}
