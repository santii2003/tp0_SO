#ifndef GUTILS_H_
#define GUTILS_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>
#include<netdb.h>



typedef enum
{
	MENSAJE,
	LINEAS_MENSAJE,
	PAQUETE,
	PROCESO, 
	/*SOLO UTILIZADO PARA CASOS DE ERROR EN recv op_code*/ 
	ERROR
}op_code;

typedef struct {
	char *archivo; 
	char *nombre_propietario; 
	uint32_t pid; 
	bool activo; 
}t_proceso; 

typedef struct
{
	uint32_t size;  //	Tamaño del payload
	uint32_t offset; // 	Desplazamiento dentro del payload
	void* stream;	//payload
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

/*Funciones del TP0 cambiadas*/
	/// @brief / SOLO UTILIZADO PARA DESCERIALIZAR. 
	/// @param codigo_operacion 
	/// @param size_buffer tamaño del buffer, obligatorio saberlo de antemano
	/// @return el paquete creado y listo para agregar datos
	t_paquete* crear_paquete(op_code codigo_operacion, uint32_t size_buffer); 


	/// @brief Crea un paquete con buffer vacío y op code. SOLO USAR PARA SERIALIZAR. 
	/// @param codigo_operacion 
	/// @return 
	t_paquete *crear_paquete_vacio(op_code codigo_operacion); 

	void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);


	/// @brief Esta función serializa el paquete una vez agregados todos los datos.
	/// @param paquete 
	/// @param bytes 
	/// @return un 'choclo' de bytes que seran enviados por sockets
	void *serializar_paquete(t_paquete* paquete, int bytes);

	void enviar_paquete(t_paquete* paquete, int socket_cliente);
	
	void eliminar_paquete(t_paquete* paquete);


/*Funciones de abstracción de la Guía de Serialización*/

	// Crea un buffer vacío de tamaño size y offset 0
	//Primero antes de serializar se DEBE Saber cuántos bytes entra en el buffer. 
	t_buffer *buffer_create(uint32_t size);

	// Libera la memoria asociada al buffer
	void buffer_destroy(t_buffer *buffer);

	// Agrega un stream al buffer en la posición actual y avanza el offset
	void buffer_add(t_buffer *buffer, void *data, uint32_t size);

	// Guarda size bytes del principio del buffer en la dirección data y avanza el offset
	void buffer_read(t_buffer *buffer, void *data, uint32_t size);

	// Agrega un uint32_t al buffer
	void buffer_add_uint32(t_buffer *buffer, uint32_t data);

	// Lee un uint32_t del buffer y avanza el offset
	uint32_t buffer_read_uint32(t_buffer *buffer);

	// Agrega un uint8_t al buffer
	void buffer_add_uint8(t_buffer *buffer, uint8_t data);

	// Lee un uint8_t del buffer y avanza el offset
	uint8_t buffer_read_uint8(t_buffer *buffer);

	// Agrega string al buffer con un uint32_t adelante indicando su longitud, CONTANDO EL CARÁCTER NULO EN LENGTH. 
	void buffer_add_string(t_buffer *buffer, uint32_t length, char *string);

	// Lee un string y su longitud del buffer y avanza el offset
	char *buffer_read_string(t_buffer *buffer, uint32_t *length);

	// Lee un string y su longitud del buffer y avanza el offset. 
	char *buffer_read_string_v2(t_buffer * buffer );

/*Funciones que agregan a paquete*/
	void agregar_a_paquete_uint8(t_paquete * paquete, uint8_t dato); 
	void agregar_a_paquete_uint32(t_paquete* paquete, uint32_t dato);
	void agregar_a_paquete_string(t_paquete* paquete, uint32_t length_string, char *string);
	void agregar_a_paquete_string_v2(t_paquete *paquete, char *string); 

/* Para Pruebas */
	void enviar_proceso (int socket, t_proceso *proceso); 
	t_proceso * recibir_proceso (int socket); 

#endif /* GUTILS_H_ */
