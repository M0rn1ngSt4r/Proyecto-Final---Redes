/**
 * @file cliente.h
 * @date 11 Diciembre 2018
 * @brief Definiciones/Mensajes del servidor y el cliente
 *
 * Se define la tabla de códigos del cliente y servidor, asi como de uso general
 * y de error.
 */


#ifndef __cliente__
#define __cliente__

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

// Definiciones para el Servidor
#define MAX_POK		151     /**< Pokemon máximos */
#define SERV_PORT	9999    /**< Puerto para conexion */
#define BUFF_SIZE	2049    /**< Tamaño de Buffer */

// Mensajes del Cliente
#define ASK_POK		10	/**< (1B op) Pide por un Pokemon */
#define BALL		11	/**< (1B op) Tira pokebola al Pokemon */
#define ROCK		12	/**< (1B op) Tira piedra al Pokemon */
#define BAIT		13	/**< (1B op) Tira comida al Pokemon */
#define RUN			14	/**< (1B op) Escapa del encuentro */
#define TRY_USER	15	/**< (1B op,nB us) Conectar con el usuario us */

// Mensajes del Servidor
#define USER		20	/**< (1B op) Si existe el usuario */
#define ENCOUNTER	21	/**< (1B op, 1B id, 1B pb) Pokemon id con pb */
#define ESCAPE		22	/**< (1B op) El Pokemon se escapo */
#define SUCCESS		23	/**< (1B op) El Pokemon se atrapo */
#define NO_TRIES	24	/**< (1B op) Ya no hay mas pokebolas */
#define POKEDEX		25	/**< (1B op, 151B pk) Se envia a pokedex */

// Mensajes Generales
#define OK  		30	/**< (1B op) Aceptar */
#define NO  		31	/**< (1B op) Rechazar */
#define END_SESSION	32	/**< (1B op) Termino de Sesion */

// Mensajes de Error
#define TIMEOUT		40	/**< (1B op) Hubo un timeout */
#define NO_USER		41	/**< (1B op) No existe el usuario */
#define ERROR_CODE	42	/**< (1B op) El codigo no se esperaba */

// Declaraciones de funciones
/**
 * @brief Uso del programa
 *
 * @return ... 
 * 
 * Imprime el uso del programa en caso de ser necesario
 */
int howto();

/**
 * @brief Notifica al usuario cuando sucede algun error
 *
 * @param sockfd Descriptor de Archivo
 * @param code Codigo de error
 * @return Void
 */
void send_error(int clntfd, char code);

#endif	//__cliente__
