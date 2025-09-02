#ifndef UTILS_H_
#define UTILS_H_

/*el include con ruta absoluta solo sirve para esta máquina, cambiar a 
una relativa si hay error*/

#include "/home/utnso/tp0_SistemasOperativos_limpio/gutils/gutils.h"
#include<sys/socket.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<assert.h>

#define PUERTO "4444"


extern t_log* logger;

void* recibir_buffer(int*, int);

int iniciar_servidor(void);
int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);

t_paquete * recv_paquete( int socket); 



#endif /* UTILS_H_ */
