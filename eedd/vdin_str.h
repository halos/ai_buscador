#ifndef __VDIN_STR_H_
#define __VDIN_STR_H_

#define __VDIN_DEBUG_ // Para la activación de la depuración

typedef struct vdin_str_rep *vdin_str;

vdin_str vdin_str_crea();
void vdin_str_destruye(vdin_str *vdin);
vdin_str vdin_str_copia(vdin_str vdin);
void vdin_str_aumd(vdin_str vdin, char* dato);
char* vdin_str_disd(vdin_str vdin);
void vdin_str_asigna(vdin_str vdin, char* dato, int pos);
char* vdin_str_obtiene(vdin_str vdin, int pos);
int vdin_str_tama(vdin_str vdin);

#endif /* __VDIN_STR_H_ */
