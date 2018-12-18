/**
 * @file clnt_hand.c
 * @date 11 Diciembre 2018
 * @brief Definiciones/Mensajes del servidor y el cliente
 *
 * Aqui se atienden las peticiones del cliente.
 */

#include "servidor.h"

void *err_send(int clntfd, char code) {
  char msg[1] = {code};
  write(clntfd, msg, 1);
  switch(code) {
  case NO_USER:
    printf("-El usuario del cliente %d no existe.\n", clntfd);
    break;
  case ERROR_CODE:
    printf("-Se recibio un código del cliente %d no esperado.\n", clntfd);
    break;
  default:
    printf("-No hubo respuesta del cliente %d.\n", clntfd);
    break;
  } 
  sleep(1);
  close(clntfd);
  return (void *) 0;
}

void *handle_client(void *vargp) {
  // Variables para mensajes
  int clntfd = *((int *)vargp);	// Descriptor de archivo del cliente
  char cwd[256];
  getlogin_r(cwd, sizeof(cwd));
  char user[256]; // Direccion del archivo de usuario
  sprintf(user, "/home/%s/Proyecto/servidor/uss/", cwd);
  FILE *uf; // Archvio de usuario
  struct timeval tout; // Tiempo de espera
  tout.tv_sec = 60;    // Le damos un minuto de espera
  tout.tv_usec = 0;
  srand(time(NULL)); // Semilla Random 
  char msg[BUFF_SIZE]; // Mensaje
  int msg_size; // Tamaño de mensaje

  // Variables para el pokemon
  char pok[MAX_POK]; // Pokedex del usuario
  unsigned char pok_act; // Pokemon actual
  // Propiedades para la captura de pokemon
  int angry = 0, eating = 0; //contadores de enojado y comiendo 
  unsigned char catch_rate = (rand() % 80) + 20;
  unsigned char init_cr = catch_rate;
  unsigned char speed = (rand() % 40) + 5;
  int pk_balls = 10; // Numero de pokebolas inicial

  // Se agrega la opcion para el timeout
  if (setsockopt(clntfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tout,
		 sizeof(tout)) < 0) {
    perror("setsockopt falló.\n");
    return (void *) 0;
  }

  // Recibir el usuario del cliente
  msg_size = read(clntfd, msg, BUFF_SIZE);
  if (msg[0] != TRY_USER)
    return err_send(clntfd, ERROR_CODE);
		
  // Ver si existe el usuario (su archivo)
  strcat(user, msg+1);
  strcat(user, ".uf");
  uf = fopen(user, "rb");
  if (!uf) 
    return err_send(clntfd, NO_USER);
	
  // Obtener datos y enviar confirmacion al cliente
  fread(&pok, sizeof(pok), 1, uf);
  fclose(uf);
  msg[0] = USER;
  write(clntfd, msg, 1);
	
  // Comienza el ciclo de captura
  int state = 0; // Distintos estados
  do {
	
    // Leemos mensaje del cliente
    if ((msg_size = read(clntfd, msg, BUFF_SIZE)) >= 0) {
      // Codigo
      char code = msg[0];
      //Si hubo un timeout entonces solo lo reconocemos
      if (code == TIMEOUT){
        printf("-Se desconectó del cliente %d.\n", clntfd);
        sleep(1);
        close(clntfd);
        return (void *) 0;
      }
      
      if (state == 0)
	switch(code) {
	  // El cliente pidio un pokemon
	case ASK_POK:	// Generamos el pokemon y lo enviamos al usuario
	  msg[0] = ENCOUNTER; 
	  msg[1] = pok_act = rand()%151; 
	  msg[2] = pk_balls;
	  msg_size = 3;
	  pok[pok_act] = (pok[pok_act]? pok[pok_act] : -1);
	  state = 1; break;
	  // Se obtuvo un codigo erroneo
	default:
	  return err_send(clntfd, ERROR_CODE);
	}
      else if (state == 1) {
	switch(code) {
	  // El usuario se fue
	case RUN: // Mensaje de termino de sesion
	  msg[0] = END_SESSION; 
	  msg_size = 1;
	  state = 3;
	  break;
	  // El usuario lazo una pokebola
	case BALL:
	  // El usuario lanzo una piedra (es mas facil de atrapar y de que
	  // escape)
	case ROCK:	
	  // El usuario lanzo comida (es mas dificil de atrapar y de que escape)
	case BAIT:	
	  if (code == BALL) {
	    // Disminuimos pokebolas en 1
	    pk_balls--;
	    if (rand()%255 <= catch_rate) {
	      // Si se capturo regresar la imagen del pokemon
	      msg[0] = SUCCESS;
	      msg[1] = pok_act;
	      char pok_dir[16];
	      sprintf(pok_dir, "/home/%s/Proyecto/servidor/pok/%u.png", cwd,
		      pok_act + 1);
	      // Abriendo imagen
	      FILE *fp = fopen(pok_dir, "rb");
	      fseek(fp, 0L, SEEK_END);
	      int *size_loc = (int*) (&msg[2]);
	      *size_loc = ftell(fp);
	      rewind(fp);
	      // Leyendo contenidos
	      msg_size = 6 + fread(msg+6, sizeof(char), BUFF_SIZE - 6, fp);
	      fclose(fp); // Cerramos imagen
	      pok[pok_act] = (pok[pok_act] <= 0 ? 1 : pok[pok_act]+1);
	      state = 2; 
	      break;
	    } else if (!pk_balls){
	      // Avisamos si no hay pokebolas
	      msg[0] = NO_TRIES;
	      msg_size = 1;
	      state = 2; 
	      break;
	    }
	  } else if (code == ROCK) {
	    // Calculamos el estado de enojo
	    eating = 0;
	    angry = (angry + (rand()%5)+1)%255;
	    catch_rate = (catch_rate*2)%255;
	  } else {
	    // Calculamos el estado al estar comiendo
	    angry = 0;
	    eating = (eating + (rand()%5)+1)%255;
	    catch_rate = (catch_rate/2)%255;
	  }
	  // Vemos si se escapa despues del turno (si ya se atrapo se evita)
	  if (rand()%255 <= (angry? speed*2: (eating? speed/2: speed))) { 
	    msg[0] = ESCAPE;
	    msg_size = 1;
	    state = 2;
	  } else {
	    // Si no escapa entonces volver a preguntar al cliente
	    msg[0] = ENCOUNTER;
	    msg[1] = pok_act;
	    msg[2] = pk_balls;
	    msg_size = 3;
	    state = 1;
	  }
	  break;
	  // Se obtuvo un codigo erroneo
	default:
	  return err_send(clntfd, ERROR_CODE);
	}
	// Actualizamos contador de carnada y enojado
	eating -= (eating ? 1 : 0);
	angry -= (angry ? 1 : 0);
	if (!eating && !angry)
	  catch_rate = init_cr;
      }
      // Al terminar el encuentro se pregunta si se quiere ver la pokedex
      else if (state == 2) {
	switch(code) {
	  // El cliente dijo que si
	case OK: // Mandamos el contenido del archivo
	  msg[0] = POKEDEX;
	  memcpy(msg+1, pok, sizeof(pok));
	  msg_size = sizeof(pok) +1;
	  state = 3;
	  break;
	  // El cliente dijo que no
	case NO:
	  msg[0] = END_SESSION; 
	  msg_size = 1;
	  state = 3;
	  break;
	  // Se obtuvo un codigo erronea
	default:	return err_send(clntfd, ERROR_CODE);
	}
      }
    } else
      return err_send(clntfd, TIMEOUT);  //no hay respuesta mandamos timeout
    
    write(clntfd, msg, msg_size); // Test de Timeout (comentando esta linea)
    sleep(1);
  } while (state != 3 || msg_size > 0);
	
  // Cerramos conexion
  close(clntfd);
  printf("Desconectandose del cliente %d.\n", clntfd);
  
  // Actualizar la pokedex del usuario en su archivo
  uf = fopen(user, "wb");
  fwrite(pok, sizeof(char), sizeof(pok), uf);
  fclose(uf);
	
  return (void *)0;	// Terminamos el thread
}
