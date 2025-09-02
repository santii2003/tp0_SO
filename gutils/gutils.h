#ifndef GUTILS_H_
#define GUTILS_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>
#include<netdb.h>
#include <errno.h>


typedef enum
{
	MENSAJE,
	LINEAS_MENSAJE,
	PAQUETE,
	PROCESO, 
	ENTERO_BOOLEANO,
	ENTERO,
	STRING, 
	/*SOLO UTILIZADO PARA CASOS DE ERROR EN recv op_code*/ 
	ERROR, 
	/*Pruebas*/
	ENVIAR
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





/*Funciones que agregan a paquete*/
	void agregar_a_paquete_uint8(t_paquete * paquete, uint8_t dato); 
	void agregar_a_paquete_uint32(t_paquete* paquete, uint32_t dato);
	void agregar_a_paquete_string(t_paquete* paquete, uint32_t length_string, char *string);
	void agregar_a_paquete_string_v2(t_paquete *paquete, char *string); 

/* Funciones que leen del paquete previamente armado*/
	uint8_t leer_de_paquete_uint8(t_paquete* paquete);
	uint32_t leer_de_paquete_uint32(t_paquete* paquete); 

	/// @brief Lee de un paquete el contenido de un String reservando memoria. 
	/// @param paquete 
	/// @return contenido del string reservado en memoria. Liberarlo una vez que ya no sea útil. 
	char * leer_de_paquete_string (t_paquete* paquete); 

/* Para Pruebas */
	void enviar_proceso (int socket, t_proceso *proceso); 
	t_proceso * recibir_proceso (int socket); 

/* Firmas auxiliares para el envio y recibimiento de estructuras/paquetes  */
	typedef t_paquete * (*serializador_t) (void *);
	typedef  void *   (*descerializador_t ) (t_paquete *);

/* Envío y recibimiento de estructuras */
	int enviar_estructura ( void * estructura, serializador_t serializador, int socket);
	op_code recibir_operacion (int socket); 
	uint32_t recibir_size_buffer(int socket);

	void * recibir_estructura (op_code code, descerializador_t descerializador, int socket);


/* Serializadores de estructuras */
	t_paquete * serializar_entero_booleano (void *dato);
	t_paquete * serializar_entero (void *dato);
	t_paquete * serializar_string (void *dato);

/* Descerializadores de estructuras */
	void * descerializar_entero_booleano (t_paquete* paquete);
	void * descerializar_entero (t_paquete * paquete);
	void * descerializar_string (t_paquete * paquete);


#endif /* GUTILS_H_ */
