cd "IO_Arduino_Scheduler" || exit

# Esegui i comandi richiesti
make clean
make
make main.hex

cd "../serial linux" || exit
make clean
make