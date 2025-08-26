#include "client.h"

static void enviar_string (char * string, int socket); 


int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();	
	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"
	log_info(logger, "Hola! Soy un log"); 


	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */
	config = iniciar_config();

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	valor = config_get_string_value(config, "CLAVE");
	// Loggeamos el valor de config
	log_info(logger, valor);
	/* ---------------- LEER DE CONSOLA ---------------- */

	// leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);
	// Enviamos al servidor el valor de CLAVE como mensaje -> UTILIZO EL PAQUETE

	enviar_string(valor, conexion); 

	t_proceso * proceso = malloc(sizeof(t_proceso));
	proceso->archivo = "CLIENTE.C";
	proceso->nombre_propietario = "santii2003";
	proceso->pid = 0;
	proceso->activo = true; 

	enviar_proceso(conexion, proceso);

	free(proceso);




	// // Armamos y enviamos el paquete
	// paquete(conexion);
	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log* iniciar_logger(void)
{	
	//uso de ruta absoluta
	t_log* nuevo_logger = log_create("/home/utnso/tp0_SistemasOperativos/client/logs/tp0.log","Cliente", true,LOG_LEVEL_INFO);
	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config = config_create("cliente.config");
	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	char* leido;

	// La primera te la dejo de yapa

	//número de líneas leídas
	int nl = 0; 
	while (1)
	{	
		leido = readline ("> ");

		if (leido) {
            add_history(leido);
        }
        if (!strcmp(leido, "")) {
			
			printf("Bucle infinito terminado, número de líneas leídas por readline: %d\n", nl);
            free(leido);
            break;
        }
		log_info(logger, leido);
		free(leido);
		nl++;
	}
	

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío


	// ¡No te olvides de liberar las leidos antes de regresar!

}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete;

	// Leemos y esta vez agregamos las leidos al paquete


	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	
}


void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	close(conexion);
	config_destroy(config); 
	log_destroy(logger);
}

static void enviar_string (char * string, int socket) {
	/*cuento el carácter Nulo*/
	uint32_t longitud_string = strlen(string)+1;
	//Como tal utilizo el op_code MENSAJE pero se puede generalizar el caso para string
	t_paquete *paquete = crear_paquete_vacio(MENSAJE); 

	//Agrego al paquete solo el string
	agregar_a_paquete_string(paquete, longitud_string, string); 
	//envío por socket
	enviar_paquete(paquete, socket); 

	eliminar_paquete(paquete); 
}