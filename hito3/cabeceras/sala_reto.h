int reserva_asiento (int id, int sala);
int libera_asiento (int asiento, int sala);
int estado_asiento(int asiento, int sala);
int asientos_libres (int sala);
int asientos_ocupados (int sala);
int capacidad (int sala);
int total_salas();
void crea_sala (int nsalas, int* capacidadSalas);
void elimina_salas();
