#include "server.h"

char *clave; 
static void * descerializar_proceso (t_paquete* paquete); 

static char* recibir_string (int socket); 

int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);
	clave = NULL;

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);
	
	for (int i = 0; i < 1; i++) {
    op_code code = recibir_operacion(cliente_fd);

    switch (code) {
        case STRING: {
            char *string_recibido = recibir_estructura(code, descerializar_string, cliente_fd);
            printf("String recibido: %s\n", string_recibido);
            free(string_recibido);
            break;
        }
        case ENTERO: {
            uint32_t *entero_recibido = recibir_estructura(code, descerializar_entero, cliente_fd);
            printf("Entero recibido: %d\n", *entero_recibido);
            free(entero_recibido);
            break;
        }
		case ENTERO_BOOLEANO: {
			uint8_t *booleano_recibido = recibir_estructura(code, descerializar_entero_booleano, cliente_fd); 
		 	printf("Booleano recibido: %u\n", *booleano_recibido);
            free(booleano_recibido);
            break;
		}
		case PROCESO: {
			t_proceso *proceso_recibido = recibir_estructura(code, descerializar_proceso, cliente_fd);
			printf("PROCESO RECIBIDO. PID: %d \n", proceso_recibido->pid);
			printf("ACTIVO? %u\n", proceso_recibido->activo);
				if ((bool)proceso_recibido->activo) {
					printf("Nombre del proceso: %s\n", proceso_recibido->archivo);
					printf("Nombre del propietario: %s\n", proceso_recibido->nombre_propietario);
				}
			free(proceso_recibido->archivo);
			free(proceso_recibido->nombre_propietario);

			free(proceso_recibido);
            break;
		}
        default:
            printf("Operación desconocida\n");
            break;
    }
}


	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}

static void * descerializar_proceso (t_paquete* paquete) {
	t_proceso * proceso = malloc(sizeof(t_proceso));
	/*Respeto el mismo orden en que agregue las cosas al serializar*/

	proceso->pid = leer_de_paquete_uint32(paquete);
	/*liberar siempre los strings que leeamos de un paquete*/

	proceso->nombre_propietario= leer_de_paquete_string(paquete);
	proceso->archivo = leer_de_paquete_string(paquete);
	proceso->activo =  (bool)leer_de_paquete_uint8(paquete);
	return (void*)proceso;
}

static char* recibir_string (int socket) {

	t_paquete * paquete = NULL; 
	char *string = NULL;

	op_code code; 
	uint32_t size_buffer;
	uint32_t size_string; 

	//Función bloqueante 
	recv(socket, &code, sizeof(op_code), 0);
		if (code !=MENSAJE ) return NULL; 
	//En caso de solo un String por paquete tengo...
	//op_code , size_buffer , size_string (cuenta el \0) , string 
	recv(socket, &size_buffer, sizeof(uint32_t), 0); 

	//Una vez que tengo el size_buffer y el op_code puedo recrear el paquete 
	//traer con recv todos los datos ahí y leerlos!

	paquete = crear_paquete(code, size_buffer); 
	recv(socket, paquete->buffer->stream, size_buffer, 0);

	//Ahora me encargo de leer del paquete el buffer!
	
	//por dentro buffer_read_string se encarga de realizar el malloc
	string = buffer_read_string(paquete->buffer, &size_string);
	

	//libero el paquete
	eliminar_paquete(paquete); 
	return string; 
}
