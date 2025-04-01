# Servidor Web Multithread em C

Este projeto é um servidor web simples, desenvolvido em C para Windows, utilizando a API Winsock2 para comunicação via sockets. Ele é capaz de servir arquivos HTML armazenados no diretório especificado e responder com uma página personalizada de erro 404 caso o arquivo solicitado não seja encontrado.

## Recursos
- Suporte a múltiplas conexões simultâneas usando threads.
- Responde a requisições HTTP GET.
- Retorna arquivos HTML armazenados no diretório raiz definido.
- Página de erro personalizada para arquivos não encontrados.
- Cabeçalhos HTTP corretamente formatados.

## Pré-requisitos
Para compilar e executar este servidor, você precisará:
- Windows com suporte a Winsock2
- Um compilador C compatível, como MinGW ou MSVC
- Biblioteca ws2_32.lib (já incluída no Windows SDK)

## Estrutura do Projeto
```
/ServidorWeb
  |-- server.c         # Código-fonte do servidor
  |-- index.html       # Página principal
  |-- notFound.html    # Página de erro 404
```

## Como Compilar
Caso esteja usando MinGW, você pode compilar o servidor com o seguinte comando:
```sh
gcc server.c -o servidor.exe -lws2_32 -pthread
```
Se estiver utilizando MSVC:
```sh
cl server.c /link ws2_32.lib
```

## Como Executar
1. Certifique-se de que os arquivos `index.html` e `notFound.html` estejam no diretório especificado no código (`D:\www` por padrão).
2. Execute o servidor:
   ```sh
   servidor.exe
   ```
3. O servidor estará escutando na porta 8080. Você pode acessá-lo através do navegador:
   ```
   http://localhost:8080/
   ```

## Funcionalidades e Implementação
- **Processamento de requisição**: O servidor verifica se a requisição HTTP é do tipo `GET`.
- **Servindo arquivos**: Se o arquivo existir no diretório raiz (`D:\www`), ele é lido e enviado ao cliente.
- **Página de erro 404**: Se o arquivo não for encontrado, o servidor responde com `notFound.html`.
- **Múltiplas conexões**: Usa `_beginthread()` para lidar com múltiplas conexões simultaneamente.

## Melhorias Futuras
- Suporte a outros métodos HTTP (POST, PUT, DELETE).
- Implementação de suporte a arquivos estáticos como imagens e CSS.
- Log de requisições para análise posterior.

## Autor
Projeto desenvolvido por mim, Igor Antonio.

---
Caso tenha dúvidas ou queira contribuir, sinta-se à vontade para sugerir melhorias!

