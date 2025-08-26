#ifndef UTILS_H_
#define UTILS_H_

#include "/home/utnso/tp0_SistemasOperativos/gutils/gutils.h"
#include<signal.h>
#include<sys/socket.h>
#include<commons/log.h>



int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);

void liberar_conexion(int socket_cliente);

#endif /* UTILS_H_ */
