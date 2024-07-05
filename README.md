# Asynchronous IO in Arduino Scheduler

Progetto per il corso di Sistemi Operativi tenuto dal docente Giorgio Grisetti.

## Panoramica del Progetto

Questo progetto aggiunge funzionalità per la scrittura bufferizzata al scheduler preemptivo di Arduino. Il sistema operativo utilizza due buffer: uno per la lettura e uno per la scrittura.

### Funzioni Implementate

1. **char getChar()**
   - Questa funzione legge un carattere dal buffer di input, se disponibile.
   - Se il buffer di input è vuoto, il thread che richiede il carattere viene messo in una coda di attesa.
   - Non appena un carattere diventa disponibile nel buffer di input, il thread viene riportato in esecuzione, il carattere viene restituito e rimosso dal buffer.

2. **void putChar(char)**
   - Questa funzione scrive un carattere nel buffer di output, se c'è abbastanza spazio.
   - Se il buffer di output è pieno, il thread viene messo in attesa.
   - Il thread viene ripreso non appena c'è spazio sufficiente nel buffer per scrivere il carattere.

## Prerequisiti

- Board con ATMega 2560
- Arduino IDE
- GCC (GNU Compiler Collection) per compilare il codice C su Linux
- Make

## Istruzioni per l'avvio

### Impostare la seriale
Come prima cosa, vedere a quale seriale è collegato l'Arduino con il comando:
```bash
sudo dmesg | grep tty
```
Se non si è sicuri di quale sia la porta seriale, scollegare l'Arduino e rilanciare il comando, poi ricollegare l'Arduino e rilanciare il comando. La porta seriale che è apparsa è quella dell'Arduino.

Una volta trovata la porta seriale, se è uguale a `/dev/ttyUSB0`, sei fortunato e non devi fare nulla.

Se la porta seriale è diversa da `/dev/ttyUSB0`, modificare i file:

- Makefile all'interno della cartella IO_Arduino_Scheduler
- lettura.sh e scrittura.sh all'interno della cartella serial_linux
  
### Compilazione e avvio
Come prima cosa, aprire il terminale nella cartella principale del progetto e lanciare il comando:
```bash
./run.sh --make
```
Questo comando permetterà di compilare il codice, caricarlo sull'Arduino e avviare il programma di lettura e scrittura seriale.

### Utilizzo
Una volta compilato il codice, per eseguirlo basta lanciare il comando:
```bash
./run.sh
```
Ovviamente, si possono eseguire e compilare manualmente i singoli componenti da terminale.
