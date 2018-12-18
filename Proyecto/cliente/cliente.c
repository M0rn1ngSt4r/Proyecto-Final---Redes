/**
 * @file cliente.c
 * @date 11 Diciembre 2018
 * @brief Programa del Cliente
 *
 * Se validan los datos del usuario, direcciones IP del servidor y el puerto,
 * despues se empieza la conexion al servidor (si se puede claro...)
 */

#include "cliente.h"

const char *pokedex[MAX_POK] =
  {
   "Bulbasaur", "Ivysaur", "Venusaur", "Charmander", "Charmeleon", "Charizard",
   "Squirtle", "Wartortle", "Blastoise", "Caterpie", "Metapod", "Butterfree",
   "Weedle", "Kakuna", "Beedrill", "Pidgey", "Pidgeotto", "Pidgeot", "Rattata", 
   "Raticate", "Spearow", "Fearow", "Ekans", "Arbok","Pikachu", "Raichu", 
   "Sandshdrew", "Sandslash", "Nidoran(F)", "Nidorina", "Nidoqueen",
   "Nidoran(M)", "Nidorino", "Nidoking", "Clefairy", "Clefable", "Vulpix",
   "Ninetales", "Jigglypuff", "Wigglytuff", "Zubat", "Golbat", "Oddish",
   "Gloom", "Vileplume", "Paras", "Parasect", "Venonat", "Venomoth", "Diglett",
   "Dugtrio", "Meowth", "Persian", "Psyduck", "Golduck", "Mankey", "Primeape",
   "Growlite", "Arcanine", "Poliwag", "Poliwhirl", "Poliwrath", "Abra",
   "Kadabra", "Alakazam", "Machop", "Machoke", "Machamp", "Bellsprout",
   "Weepinbell", "Victreebel", "Tentacool", "Tentacruel", "Geodude", "Graveler",
   "Golem", "Ponyta", "Rapidash", "Slowpoke", "Slowbro", "Magnemite",
   "Magneton", "Farfetch'd", "Doduo", "Dodrio", "Seel", "Dewgong", "Grimer",
   "Muk", "Shellder", "Cloyster", "Gastly", "Haunter", "Gengar", "Onix",
   "Drowzee", "Hypno", "Krabby", "Kingler", "Voltorb", "Electrode", "Exeggcute",
   "Exeggutor", "Cubone", "Marowak", "Hitmonlee", "Hitmonchan", "Lickitung",
   "Koffing", "Weezing", "Rhyhorn", "Rhydon", "Chansey", "Tangela",
   "Kangaskhan", "Horsea", "Seedra", "Goldeen", "Seaking", "Staryu", "Starmie", 
   "Mr. Mime", "Scyther", "Jynx", "Electabuzz", "Magmar", "Pinsir", "Tauros", 
   "Magikarp", "Gyarados", "Lapras", "Ditto", "Eevee", "Vaporeon", "Jolteon", 
   "Flareon", "Porygon", "Omanyte", "Omastar", "Kabuto", "Kabutops",
   "Aerodactyl", "Snorlax", "Articuno", "Zapdos", "Moltres", "Dratini",
   "Dragonair", "Dragonite", "Mewtwo", "Mew"
  }; /**< Pokedex Total (arreglo) */

int howto() {
  printf("Uso:\nszclient <username> <IP addr> <port num>\n\n");
  return 0;
}

void send_error(int sockfd, char code) {
  char msg[1] = {code};
  write(sockfd, msg, 1);
  switch(code) {
  case NO_USER:
    printf("Error: El usuario no existe.\n");
    break;
  case ERROR_CODE:
    printf("Error: Se recibio un codigo no esperado.\n");
    break;
  default:
    printf("Error: No hubo respuesta del servidor.\n");
  } 
  sleep(1);
  exit(1);
}

/**
 * @brief Función para comunicarse con el Servidor
 *
 * Aqui se validan los datos antes de intentar una conexion al servidor, en caso
 * de error, se manda mensaje al usuario, debe haber 4 argumentos totales, si
 * no los hay, se manda el uso del programa. Si se tiene exito, se comienza
 * la comunicación al servidor por medio de un menu.
 *
 * @param argc Numero de argumentos
 * @param argv Argumentos en arreglo
 * @return ...
 */
int main(int argc, char **argv)
{
  if (argc != 4)
    return howto();
  char cwd[256];
  getlogin_r(cwd, sizeof(cwd));
  // Variables para la comunicacion
  int sockfd = 0; // Descriptor de Archivo 
  char msg[BUFF_SIZE]; // Mensaje
  int msg_size;	// Tamaño de mensaje 
  struct sockaddr_in serv_addr;	// Direccion del servidor 
  FILE *fp; // Archivo para el pokemon
  struct timeval tout; // Tiempo de espera
  tout.tv_sec = 60;    // Le damos un minuto de espera
  tout.tv_usec = 0;
  char box_dir[64]; // Directorio de almacenamiento de pokemon 
 
  // Se crea el socket para conectarse con el servidor
  memset(msg, '0' ,sizeof(msg));
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0) {
    printf("\n Error : No se pudeo crear el socket \n");
    return 1;
  }
  // Especificamos la direccion del servidor
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(atoi(argv[3])); // Puerto
  serv_addr.sin_addr.s_addr = inet_addr(argv[2]); // IP
  // Intenta la conexion con el servidor
  if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0) {
    printf("\n Error : Fallo de conexion.\n");
    return 1;
  }
  
  // Se agrega la opcion para el timeout
  if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tout,
		 sizeof(tout)) < 0) {
    perror("setsockopt falló.\n");
    return 1;
  }
	
  // Intenta conectarse con el nombre de usuario de los argumentos
  msg[0] = TRY_USER; // Codigo 
  strcpy(msg + 1, argv[1]); // Copiamos el nombre del usuario
  write(sockfd, msg, strlen(argv[1]) + 2); // Enviamos el mensaje
  
  // Revisamos si se introdujo un nombre que existe
  if ((msg_size = read(sockfd, msg, BUFF_SIZE)) > 0)
    // Revisamos el codigo 
    switch(msg[0]) {
      // Si existe el usuario
    case USER:
      printf("Bienvenido al Safari Zone!\n"); 
      msg[0] = ASK_POK; write(sockfd, msg, 1);
      break;
      // Cualquier otro codigo
    default:
      send_error(sockfd, msg[0]);
      return 0;
    }
  else {
    printf("No se pudo leer respuesta.\n"); // Si no leyo nada manda error
    return 0;
  }
  
  // Inicia el ciclo de mensajes entre cliente y servidor
  int fst_enc = 0;
  do  {
	
    // Leemos el mensaje
    if ((msg_size = read(sockfd, msg, BUFF_SIZE)) >= 0) {
      char code = msg[0]; // Codigo de operacion
      //Si hubo un timeout entonces solo lo reconocemos
      if (code == TIMEOUT){
        printf("Error: Se desconectó el servidor.\n");
        sleep(1);
        return 1;
      }
      
      char resp; // Respuesta del cliente
      // Creamos respuesta dependiendo del codigo
      switch(code) {
	// Hay un encuentro con un pokemon
      case ENCOUNTER:	
	if (!(fst_enc++))
	  printf("Apareció un %s! tienes %d pokebolas.\n",
		 pokedex[(unsigned char)msg[1]], msg[2]);
	else
	  printf("%s está observando cuidadosamente, tienes %d pokebolas.\n",
		 pokedex[(unsigned char)msg[1]], msg[2]);
	printf("Acción:\n1 POKEBOLA\t2 PIEDRA\n3 CARNADA\t4 CORRER\n\n");
	scanf(" %c", &resp);
	msg[0] = BALL + (resp - '1')%4; // Mensaje es el numero de accion
	break;
	// Termino la interaccion con el pokemon
      case SUCCESS:	// Carga la imagen del pokemon
	sprintf(box_dir, "/home/%s/Proyecto/cliente/box/%s.png", cwd,
		pokedex[(unsigned char)msg[1]]);
	fp = fopen(box_dir, "wb");
	// Debe de existir la carpeta box
	if (!fp)
	  return 0; 
	int img_size = *((int *)(msg + 2));
	// Escribimos la imagen
	fwrite(msg+6, sizeof(char), msg_size - 6, fp);
	fclose(fp);
      case NO_TRIES:
      case ESCAPE: // Muestra mensajes dependiendo del caso
	if (code == SUCCESS)
	  printf("Lograste capturar al Pokemon! ");
	else if (code == NO_TRIES)
	  printf("Se te acabaron las Pokebolas, ");
	else
	  printf("Oh! Se escapó el Pokemon, ");
	// Ver si el usuario desea ver su Pokedex
	printf("deseas ver tu Pokedex? [y/n]\n");
	scanf(" %c", &resp);
	msg[0] = (resp == 'y' ? OK : NO); // Enviar respuesta
	break;
	// Se envio la pokedex del usuario
      case POKEDEX:	// Imprime la pokedex en pantalla
	for (int i = 0; i < MAX_POK; i++)
	  printf("%d\t%s %c\n", i+1
		 , (msg[i+1]? pokedex[i]: "???")
		 , (msg[i+1]<= 0?' ': 'o'));
	// El servidor termino la sesion
      case END_SESSION:
	printf("Gracias por visitarnos!\n"); return 0;
	//Cualquier otro codigo
      default:
	send_error(sockfd, msg[0]);
      }
    } else 
      send_error(sockfd, TIMEOUT);  //si se tardo mandamos un timeout
    
    write(sockfd, msg, 1);
    sleep(1);
  } while (msg_size > 0);
	
  return 0;
}
