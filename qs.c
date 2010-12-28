
void swap(float *v, int *d, int i, int j){

	float aux_f = v[i];
	v[i] = v[j];
	v[j] = aux_f;

        int aux_i = d[i];
	d[i] = d[j];
	d[j] = aux_i;

}

int pivotar (float *v, int *d, int izq, int der)
{ 
    int posicionPivote = (izq + der) / 2;
    float valorPivote = v[posicionPivote];
    int indiceAlmacenamiento;
    int indiceLectura;
    
    swap (v, d, posicionPivote, der);//Se situal pivote al final del vector
    indiceAlmacenamiento = izq;
 
    for (indiceLectura = izq; indiceLectura < der; indiceLectura++){
        if (v[indiceLectura] <= valorPivote){
           swap (v, d, indiceLectura, indiceAlmacenamiento);
            indiceAlmacenamiento++;
        }
    }
 
    swap (v, d, indiceAlmacenamiento, der); //Se coloca el pivote en su lugar.
 
    return indiceAlmacenamiento;
}
 
void quicksort (float *v, int *d, int izq, int der)
{
    int pivote;
     if(izq < der){
        pivote = pivotar (v, d, izq, der); //Esta operación coloca el pivote en su lugar.
        quicksort(v, d, izq, pivote-1);
        quicksort(v, d, pivote+1, der);
    } 
}
