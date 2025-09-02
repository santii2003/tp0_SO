#include "gutils.h"

/*STATIC FUNCTIONS*/

	/*Funciones de abstracción de la Guía de Serialización*/

	// Crea un buffer vacío de tamaño size y offset 0
	//Primero antes de serializar se DEBE Saber cuántos bytes entra en el buffer. 
	static t_buffer *buffer_create(uint32_t size);

	// Libera la memoria asociada al buffer
	static void buffer_destroy(t_buffer *buffer);

	// Agrega un stream al buffer en la posición actual y avanza el offset
	static void buffer_add(t_buffer *buffer, void *data, uint32_t size);

	// Guarda size bytes del principio del buffer en la dirección data y avanza el offset
	static void buffer_read(t_buffer *buffer, void *data, uint32_t size);

	// Agrega un uint32_t al buffer
	static void buffer_add_uint32(t_buffer *buffer, uint32_t data);

	// Lee un uint32_t del buffer y avanza el offset
	static uint32_t buffer_read_uint32(t_buffer *buffer);

	// Agrega un uint8_t al buffer
	static void buffer_add_uint8(t_buffer *buffer, uint8_t data);

	// Lee un uint8_t del buffer y avanza el offset
	static uint8_t buffer_read_uint8(t_buffer *buffer);

	// Agrega string al buffer con un uint32_t adelante indicando su longitud, CONTANDO EL CARÁCTER NULO EN LENGTH. 
	static void buffer_add_string(t_buffer *buffer, uint32_t length, char *string);

	// Lee un string y su longitud del buffer y avanza el offset
	static char *buffer_read_string(t_buffer *buffer, uint32_t *length);

	// Lee un string y su longitud del buffer y avanza el offset. 
	static char *buffer_read_string_v2(t_buffer * buffer );



void *serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(op_code));
	desplazamiento+= sizeof(op_code);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(uint32_t));
	desplazamiento+= sizeof(uint32_t);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}


t_paquete* crear_paquete(op_code codigo_operacion, uint32_t size_buffer)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo_operacion;
	paquete->buffer = buffer_create(size_buffer);
	return paquete;
}

t_paquete *crear_paquete_vacio(op_code codigo_operacion) {
	t_paquete * paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo_operacion;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->offset = 0;
	//inciializo el stream en NULL para posterior realloc
	paquete->buffer->stream = NULL;  
	return paquete; 
}



void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);
	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + sizeof(uint32_t) + sizeof(op_code);
	void* a_enviar = serializar_paquete(paquete, bytes);

	if (send(socket_cliente, a_enviar, bytes, 0) == -1) {
		fprintf(stderr, "Error en send, FUNCIÓN: enviar_paquete : %s\n", strerror(errno));
		return; 
	}
	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
	buffer_destroy(paquete->buffer); 
	free(paquete);
}


void agregar_a_paquete_uint8(t_paquete * paquete, uint8_t dato) {
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + sizeof(uint8_t));
	paquete->buffer->size +=sizeof(uint8_t);
	buffer_add_uint8(paquete->buffer, dato);
}

void agregar_a_paquete_uint32(t_paquete* paquete, uint32_t dato) {
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + sizeof(uint32_t));
	paquete->buffer->size +=sizeof(uint32_t);
	buffer_add_uint32(paquete->buffer, dato);
}

void agregar_a_paquete_string(t_paquete* paquete, uint32_t length_string, char *string) {
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + sizeof(uint32_t) + length_string );
	paquete->buffer->size += sizeof(uint32_t) + length_string ;
	buffer_add_string(paquete->buffer, length_string, string); 
}


void agregar_a_paquete_string_v2(t_paquete *paquete, char *string){
	uint32_t length_string = strlen(string)+1; 
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + sizeof(uint32_t) + length_string);
	paquete->buffer->size += sizeof(uint32_t) + length_string ;
	buffer_add_string(paquete->buffer, length_string, string); 
}


uint8_t leer_de_paquete_uint8(t_paquete* paquete) {
	return buffer_read_uint8(paquete->buffer);
}
uint32_t leer_de_paquete_uint32(t_paquete* paquete) {
	return buffer_read_uint32(paquete->buffer);
}
char * leer_de_paquete_string (t_paquete* paquete) {
	return buffer_read_string_v2(paquete->buffer); 
}


int enviar_estructura (void * estructura, serializador_t serializador, int socket){
	t_paquete * a_enviar =  serializador (estructura); 
	enviar_paquete(a_enviar, socket); 
	eliminar_paquete(a_enviar); 
	return 0;
}

op_code recibir_operacion (int socket) {
	op_code code;
	if  (recv(socket, &code, sizeof(op_code), MSG_WAITALL) != -1 ) {
		return code;
	} else {
		fprintf(stderr, "Error en recv, FUNCIÓN: recibir_operacion : %s\n", strerror(errno));
		return ERROR; 
	}

}

uint32_t recibir_size_buffer(int socket) {
	uint32_t size_buffer; 
	if  (recv(socket, &size_buffer, sizeof(uint32_t), MSG_WAITALL) == -1 ) {
		fprintf(stderr, "Error en recv, FUNCIÓN: recibir_size_buffer : %s\n", strerror(errno));
		return 1; 
	} 
	return size_buffer; 
}

//Asume que el op_code ya viene recibido. Esto para que pueda ser tratada en un switch case. 
void * recibir_estructura (op_code code, descerializador_t descerializador, int socket) {
	uint32_t size_buffer = recibir_size_buffer (socket); 
	t_paquete * paquete = crear_paquete(code, size_buffer); 
	if (recv(socket, paquete->buffer->stream, size_buffer, MSG_WAITALL) != -1) {
		void * to_return = descerializador(paquete);
		eliminar_paquete(paquete);
		return to_return; 
	} else {
		fprintf(stderr, "Error en recv, FUNCIÓN: recibir_estructura : %s\n", strerror(errno));
		return NULL; 
	}
}


t_paquete * serializar_entero_booleano (void * dato) {
	uint8_t entero_booleano = *(uint8_t*) dato; 
	t_paquete * paquete = crear_paquete_vacio(ENTERO_BOOLEANO);	
	agregar_a_paquete_uint8(paquete, entero_booleano);
	return paquete;
}


t_paquete * serializar_entero (void *dato) {
	uint32_t entero = *(uint32_t*) dato;
	t_paquete * paquete = crear_paquete_vacio(ENTERO);
	agregar_a_paquete_uint32(paquete, entero); 
	return paquete; 
}

t_paquete * serializar_string (void *dato){
	char * string = (char *) dato; 
	t_paquete * paquete = crear_paquete_vacio(STRING); 
	agregar_a_paquete_string_v2(paquete, string);
	return paquete; 
}


void * descerializar_entero_booleano (t_paquete* paquete) {
	uint8_t* entero_booleano =  malloc(sizeof(uint8_t)); 
	*entero_booleano = leer_de_paquete_uint8 (paquete); 
	return (void *) entero_booleano; 
}
void * descerializar_entero (t_paquete * paquete) {
	uint32_t* entero =  malloc(sizeof(uint32_t)); 
	*entero = leer_de_paquete_uint32 (paquete); 
	return (void *) entero; 
}

void * descerializar_string (t_paquete * paquete) {
	char * string = leer_de_paquete_string(paquete);
	return (void*) string;
}


/*STATIC FUNCTIONS */
	static t_buffer *buffer_create(uint32_t size) {
		t_buffer * buf = malloc(sizeof(t_buffer)); 
		buf->size = size; 
		buf->stream = malloc(buf->size);
		buf->offset = 0; 
		return buf; 
	}

	static void buffer_destroy (t_buffer *buffer) {
		free(buffer->stream); 
		free(buffer); 
	}

	static void buffer_add(t_buffer *buffer, void *data, uint32_t size) {
		memcpy(buffer->stream + buffer->offset, data, size);
		buffer->offset += size; 

	}

	static void buffer_read (t_buffer *buffer, void *data, uint32_t size) {
		memcpy(data, buffer->stream + buffer->offset, size);
		buffer->offset += size;
	}

	static void buffer_add_uint32(t_buffer * buffer, uint32_t data) {
		buffer_add(buffer, &data, sizeof(uint32_t));	
	}

	static uint32_t buffer_read_uint32 (t_buffer * buffer) {
		uint32_t i; 
		buffer_read(buffer, &i ,sizeof(uint32_t)); 
		return i;
	}

	static void buffer_add_uint8(t_buffer * buffer, uint8_t data) {
		buffer_add(buffer, &data, sizeof(uint8_t));
	}

	static uint8_t buffer_read_uint8(t_buffer * buffer) {
		uint8_t i; 
		buffer_read(buffer, (void*) &i, sizeof(uint8_t)); 	
		return i;
	}

	static void buffer_add_string (t_buffer * buffer, uint32_t length, char *string) {
		buffer_add(buffer, &length, sizeof(uint32_t));
		buffer_add(buffer, string, length);
	}

	static char *buffer_read_string (t_buffer * buffer, uint32_t *length){
		/*primero leemos el tamaño de la cadena*/
		buffer_read(buffer, length, sizeof(uint32_t)); 
		/*Una vez leído, su tamaño está en el *length  */

		/*reservo memoria para ese string y lo devuelvo*/
		char *string = malloc(*length);
		buffer_read(buffer, string, *length);
		return string; 
	}

	static char *buffer_read_string_v2(t_buffer * buffer ) {
		uint32_t length;
		buffer_read(buffer, &length, sizeof(uint32_t));
		char *string = malloc (length);
		buffer_read(buffer, string, length);
		return string; 
	}
