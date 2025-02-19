# Controlla se è stato passato un parametro
MAKE_COMMANDS=false
if [ "$1" == "--make" ]; then
    MAKE_COMMANDS=true
fi

if [ "$MAKE_COMMANDS" == true ]; then
    #esegue il file make.sh
    ./make.sh
fi


cd "serial linux" || exit

# Apri il secondo terminale e esegui il secondo script
gnome-terminal  --title="Terminale di Output" -- bash -c "stty -echo -icanon; ./scrittura.sh; stty echo icanon; exec bash"

# Apri il primo terminale e esegui il primo script
gnome-terminal  --title="Terminale di Input" -- bash -c "./lettura.sh; exec bash"
