/**
 * @file servidor.h
 * @date 11 Diciembre 2018
 * @brief Definiciones/Mensajes del servidor y el cliente
 *
 * Se define la tabla de códigos del cliente y servidor, asi como de uso general
 * y de error. Ademas se definen funciones adicionales para el servidor.
 */

#ifndef __servidor__
#define __servidor__

#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

// Definiciones para el Servidor
#define MAX_POK		151     /**< Pokemon máximos */
#define SERV_PORT	9999    /**< Puerto para conexion */
#define BUFF_SIZE	2049    /**< Tamaño de Buffer */

// Mensajes del Cliente
#define ASK_POK		10	/**< (1B op) Pide por un Pokemon */
#define BALL		11	/**< (1B op) Tira pokebola al Pokemon */
#define ROCK		12	/**< (1B op) Tira piedra al Pokemon */
#define BAIT		13	/**< (1B op) Tira comida al Pokemon */
#define RUN 		14	/**< (1B op) Escapa del encuentro */
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
 * @brief Función para el manejo de errores
 *
 * Muestra un error del lado del servidor.
 *
 * @param errorMessage El mensaje de error
 * @return Void
 */
void error(char *errorMessage);

/**
 * @brief Funcion que atiende al cliente
 *
 * Corrobora que el usuario tenga su archivo correspondiente, obtiene los datos
 * y comienza el ciclo de captura. Se validan las acciones del usuario, asi
 * como si se capturan o no los pokemon, tambien se calcula si los pokemon 
 * escapan. (el conteo de las pokebolas igual)
 * 
 * Al final se pregunta si se quiere ver el pokedex del usuario en consola y se
 * termina el thread.
 *
 * @param vargp Descriptor de archivo (desde la creación del thread)
 * @return NULL?
 */
void *handle_client(void *vargp);

/**
 * @brief Envia un codigo de error al cliente
 *
 * @param clntfd Descriptor de archivo del cliente
 * @param code El codigo de error
 * @return NULL?
 */
void *err_send(int clntfd, char code);	

#endif	// __servidor__
