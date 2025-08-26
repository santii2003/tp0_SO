#include "gutils.h"

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
	//inciializo el buffer en NULL 
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->offset = 0;
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

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
	buffer_destroy(paquete->buffer); 
	free(paquete);
}

t_buffer *buffer_create(uint32_t size) {
	t_buffer * buf = malloc(sizeof(t_buffer)); 
	buf->size = size; 
	buf->stream = malloc(buf->size);
	buf->offset = 0; 
	return buf; 
}

void buffer_destroy (t_buffer *buffer) {
	free(buffer->stream); 
	free(buffer); 
}

void buffer_add(t_buffer *buffer, void *data, uint32_t size) {
	memcpy(buffer->stream + buffer->offset, data, size);
	buffer->offset += size; 

}

void buffer_read (t_buffer *buffer, void *data, uint32_t size) {
	memcpy(data, buffer->stream + buffer->offset, size);
    buffer->offset += size;
}

void buffer_add_uint32(t_buffer * buffer, uint32_t data) {
	buffer_add(buffer, &data, sizeof(uint32_t));	
}

uint32_t buffer_read_uint32 (t_buffer * buffer) {
	uint32_t i; 
	buffer_read(buffer, &i ,sizeof(uint32_t)); 
	return i;
}

void buffer_add_uint8(t_buffer * buffer, uint8_t data) {
	buffer_add(buffer, &data, sizeof(uint8_t));
}

uint8_t buffer_read_uint8(t_buffer * buffer) {
	uint8_t i; 
	buffer_read(buffer, (void*) &i, sizeof(uint8_t)); 	
	return i;
}

void buffer_add_string (t_buffer * buffer, uint32_t length, char *string) {
	buffer_add(buffer, &length, sizeof(uint32_t));

	buffer_add(buffer, string, length);
}

char *buffer_read_string (t_buffer * buffer, uint32_t *length){
	/*primero leemos el tamaño de la cadena*/
	buffer_read(buffer, length, sizeof(uint32_t)); 
	/*Una vez leído, su tamaño está en el *length  */

	/*reservo memoria para ese string y lo devuelvo*/
	char *string = malloc(*length);
	buffer_read(buffer, string, *length);
	return string; 
}

char *buffer_read_string_v2(t_buffer * buffer ) {
	uint32_t length;
	buffer_read(buffer, &length, sizeof(uint32_t));

	char *string = malloc (length);
	buffer_read(buffer, string, length);
	return string; 
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

void enviar_proceso (int socket, t_proceso *proceso) {
	uint32_t l1, l2; 

	//seguir orden, si primero serializo algo, después será lo primero a descerializar
	//por el offset
	l1= strlen(proceso->archivo) + 1;
	//por cada string añadido, necesito sumarle +4 por el size
	l2 = strlen(proceso->nombre_propietario) +1;


	t_paquete * paquete =  crear_paquete_vacio (PROCESO); 

	//agrego por orden del proceso al paquete..
	agregar_a_paquete_string(paquete, l1, proceso->archivo);
	agregar_a_paquete_string(paquete, l2, proceso->nombre_propietario); 
	agregar_a_paquete_uint32(paquete, proceso->pid);
	agregar_a_paquete_uint8(paquete, (uint8_t)proceso->activo );

	enviar_paquete(paquete, socket);	

	eliminar_paquete(paquete);
	
}

t_proceso * recibir_proceso (int socket) {
	t_proceso *proceso = malloc(sizeof(t_proceso)); 
	t_paquete *paquete = NULL; 

	op_code code; 
	uint32_t size_buffer;

	recv(socket, &code, sizeof(op_code), 0);
		if (code !=PROCESO ) return NULL; 

	recv(socket, &size_buffer, sizeof(uint32_t), 0);

	paquete = crear_paquete(code, size_buffer);
	//recibo todo el stream de bytes
	recv(socket, paquete->buffer->stream, size_buffer, 0);

	//leo en el orden que envie el stream. 
	proceso->archivo = buffer_read_string_v2(paquete->buffer);
	proceso->nombre_propietario = buffer_read_string_v2(paquete->buffer);
	proceso->pid = buffer_read_uint32(paquete->buffer);
	proceso->activo = (bool) buffer_read_uint8(paquete->buffer);

	eliminar_paquete(paquete);

	return proceso;
}