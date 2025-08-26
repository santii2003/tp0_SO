#include "server.h"

char *clave; 

static char* recibir_string (int socket); 

int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);
	clave = NULL;

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

	clave = recibir_string(cliente_fd);  
	printf("%s\n ", clave); 


	t_proceso * proceso; 
	proceso = recibir_proceso(cliente_fd);

	printf(" Recibi del cliente un proceso el siguiente nombre: %s \n", proceso->archivo);

	printf(" Propietario: %s \n", proceso->nombre_propietario);
	printf(" PID : %d \n", proceso->pid);
	printf(" Booleano: %d \n", (int)proceso->activo); //debe mostrar 1 -> true en binario
	
	free(proceso->archivo);
	free(proceso->nombre_propietario);
	free(proceso);


	// t_list* lista;




	// while (1) {
	// 	int cod_op = recibir_operacion(cliente_fd);
	// 	switch (cod_op) {
	// 	case MENSAJE:
	// 		recibir_mensaje(cliente_fd);
	// 		break;
	// 	case PAQUETE:
	// 		lista = recibir_paquete(cliente_fd);
	// 		log_info(logger, "Me llegaron los siguientes valores:\n");
	// 		list_iterate(lista, (void*) iterator);
	// 		break;
	// 	case -1:
	// 		log_error(logger, "el cliente se desconecto. Terminando servidor");
	// 		return EXIT_FAILURE;
	// 	default:
	// 		log_warning(logger,"Operacion desconocida. No quieras meter la pata");
	// 		break;
	// 	}
	// }


	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
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
