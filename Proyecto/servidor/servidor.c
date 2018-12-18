/**
 * @file servidor.c
 * @date 11 Diciembre 2018
 * @brief Programa del Servidor
 *
 * Se inicializa el socket, se define la dirección del puerto, y se crean
 * los threads correspondientes a los clientes que piden conexion.
 */

#include "servidor.h"

void error(char *errorMessage) {
  perror(errorMessage);
  exit(1);
}

/**
 * @brief Función para establecer conexion con los clientes
 *
 * Aqui se crea/inicia el socket para escuchar peticiones (ciclo infinito),
 * cuendo se acepta un cliente, se crea un thread para poder tener varias
 * conexiones a la vez y se empieza a atender al cliente.
 *
 * @param argc Numero de argumentos
 * @param argv Argumentos en arreglo
 * @return ...
 */
int main(int argc, char **argv) {
  // Variables para el servidor
  pthread_t thread_id;
  int servfd; // File descriptor del servidor
  int clntfd; // File descriptor de un cliente
  struct sockaddr_in serv_addr; // Direccion del servidor
  struct sockaddr_in clnt_addr; // Direccion del cliente
  unsigned int clnt_len;
	
  // Inicializamos el socket
  if ((servfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    error("socket() fallo\n");
	
  // Definimos la direccion del servidor
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(SERV_PORT);
  printf("Creando un servidor con IP %s en puerto %d\n",
	 inet_ntoa(serv_addr.sin_addr), SERV_PORT);
	
  // Bind y listen del socket a la direccion
  if (bind(servfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("bind() fallo\n");
  // Escuchando 
  if (listen(servfd, 10) < 0)
    error("listen() fallo\n");
	
  // Se queda escuchando por conexiones infinitamente
  while(1){
    // Aceptamos a algun cliente
    clnt_len = sizeof(clnt_addr);
    if ((clntfd = accept(servfd, (struct sockaddr *) &clnt_addr,
			 &clnt_len)) < 0)
      error("accept() fallo\n");
    // Al aceptar, lo atendemos en un thread nuevo
    printf("Atendiendo al cliente %d con IP %s\n", clntfd,
	   inet_ntoa(clnt_addr.sin_addr));
    pthread_create(&thread_id, NULL, handle_client, (void *) &clntfd);	
    sleep(1);
  }
  return 0;
}	
