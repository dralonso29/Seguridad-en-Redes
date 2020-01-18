//         genera la firma digital de unos datos, cifrados con la clave privada
//              gcc -Wall -g final.c -lssl -lcrypto

#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/engine.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

unsigned char EMSASHA512ID[] = {0x30, 0x51, 0x30, 0x0d,
								0x06, 0x09, 0x60, 0x86,
								0x48, 0x01, 0x65, 0x03,
								0x04, 0x02, 0x03, 0x05,
								0x00, 0x04, 0x40};
char verb_v[]= "-v";

enum {
	L_SHA512 = SHA512_DIGEST_LENGTH, // len = 64
	TEXT_SIZE = 50,
	KEY_SIZE = 4096,
	BLOCK_SIZE = KEY_SIZE/8,	// len = 512
	LEN_ID = sizeof(EMSASHA512ID), // len = 19
	LEN_T = LEN_ID + L_SHA512, // len = 19 + 64 = 83 (esto es ID||Hash donde Hash es Sha512(Mensaje||nombrefichero))
	PS_SIZE = BLOCK_SIZE - LEN_T - 3,// len = 512 - 83 - 3 = 426
	PADDING_SIZE = BLOCK_SIZE - L_SHA512 - LEN_ID // len = 512 - 64 - 19 = 429
};

void
converthexa(unsigned char *md)
{
	for (int i = 0; i < L_SHA512; i++)
	{
		printf("%02x",md[i]);
	}
	printf("\n");
}

int
sha512(char *path_fichero, unsigned char *text, int text_len, unsigned char *md)
{
    SHA512_CTX c;
    int fd, nr;
    int stt = 0;

    fd = open(path_fichero, O_RDONLY); //abrimos el path del fichero que contiene el mensaje
    if (fd < 0){
        stt = fd;
    	err(1, "open %s", path_fichero);
    }

    SHA512_Init(&c);
    for (;;){
    	nr = read(fd, text, text_len);
        stt = nr;

    	if (nr < 0){
    		close(fd);
    		err(1, "read");
    	}

    	if (nr == 0){	//fin de fichero
    		break;
        }
    	SHA512_Update(&c, text, nr); //alimentamos a la HASH con lo que hemos leido del fichero ////nr es el numero de bytes que leemos por cada pasada/////
    }
	// Concateno lo que leo del fichero con el nombre del fichero.
	SHA512_Update(&c, path_fichero, strlen(path_fichero)); // Para comprobar si lo estamos haciendo bien (en el shell): (cat a.txt ; echo -n 'a.txt') | sha512sum

    if (close(fd) < 0){
    	err(1, "close");
    }

    //cuando no queda mas por leer...
    SHA512_Final(md, &c);

    //converthexa(md);

    return stt;
}

void
concatenaIDsha512(unsigned char *md, unsigned char *IDsha512)
{

	for (size_t i = 0; i < LEN_ID; i++) { //primero metemos ID en la variable IDsha512
		IDsha512[i] = EMSASHA512ID[i];
	}

	for (size_t i = 0 ; i < L_SHA512; i++) { // y a continuacion metemos Hash
		IDsha512[i + LEN_ID] = md[i]; // seguimos introduciendo por donde nos habiamos quedado (en LEN_ID)
	}
}

void
createPadding(unsigned char *PD)
{
	PD[0] = 0x00;
	PD[1] = 0x01;
	for (int i = 2; i < PADDING_SIZE -1; i++) {
		PD[i] = 0xFF;
	}
	PD[PADDING_SIZE - 1] = 0x00;
}

void
concatenaPaddingHash(unsigned char *PD, unsigned char *HASH, unsigned char *MSG)
{
	for (int i = 0; i < PADDING_SIZE; i++) {
		MSG[i] = PD[i];
	}

	for (int i = 0; i < LEN_T; i++) {
		MSG[i + PADDING_SIZE] = HASH[i];
	}
}

RSA *
readPrivkey(char * pathPrivateKey)
{
	FILE *fp;
	RSA * RSAKey;

	fp = fopen(pathPrivateKey, "r"); // hay que abrir asi el fichero ya que la funcion PEM_read_RSAPrivateKey necesita que fp sea de tipo FILE
	if (fp < 0){
    	err(1, "fopen %s", pathPrivateKey);
    }

	RSAKey = RSA_new();
	if (RSAKey == NULL) {
		err(1, "readPrivkey: RSA_new(): RSAKey is NULL");
	}

	RSAKey = PEM_read_RSAPrivateKey(fp, &RSAKey, NULL, NULL);
	if (RSAKey == NULL) {
		err(1, "PEM_read_RSAPrivateKey %s", pathPrivateKey);
	}
	return RSAKey;
}

void
generarFirmaDigital(unsigned char * BLOCK, RSA * RSAKey, unsigned char * C_BLOCK)
{
	BIO * Base64;
	BIO * NormalBase;
	int wr = 0;
	int sig_size = 0;

	Base64 = BIO_new(BIO_f_base64()); // Crea una BIO de base 64
	NormalBase = BIO_new_fp(stdout, BIO_NOCLOSE); // Hay que poner ese FLAG obligatoriamente (man 3 BIO_s_file) stdout indica la salida estandar
	BIO_push(Base64, NormalBase); // hace un append de NormalBase a Base64

	sig_size = RSA_private_encrypt(BLOCK_SIZE, BLOCK, C_BLOCK, RSAKey, RSA_NO_PADDING);
	if (sig_size < 0) {
		fprintf(stderr, "%s\n",  ERR_error_string(ERR_get_error(), NULL));
	}

	printf("---BEGIN SRO SIGNATURE---\n");
	wr = BIO_write(Base64, C_BLOCK, BLOCK_SIZE);
	if (wr == -1 || wr == 0) {
		err(1, "BIO_write: No data written");
	}else if (wr == -2) {
		err(1, "BIO_write: the operation is not implemented in the specific BIO type");
	}
	BIO_flush(Base64);
	printf("---END SRO SIGNATURE---\n");

	BIO_free_all(Base64);
}

void
leerFirmaB64(char * pathSignature, unsigned char * BLOCK)
{
	FILE * fp;
	BIO * Base64;
	BIO * NormalBase;
	int leido, t = 0;

	fp = fopen(pathSignature, "r"); // primero intentamos abrir y leer antes de lanzar las BIOS
	if (fp < 0){
    	err(1, "fopen %s", pathSignature);
    }

	Base64 = BIO_new(BIO_f_base64()); // Crea una BIO de base 64
	NormalBase = BIO_new_fp(fp, BIO_NOCLOSE); // Hay que poner ese FLAG obligatoriamente (man 3 BIO_s_file) stdout indica la salida estandar
	BIO_push(Base64, NormalBase); // hace un append de NormalBase a Base64

	for(;;) {
		// vamos leyendo del fichero en base 64 y actualizamos posicion y tamaño del bloque (si en una lectura no nos vale)
		leido = BIO_read(Base64, BLOCK + t, BLOCK_SIZE - t);
		if (leido == -1) {
			err(1, "BIO_read: No data read");
		}else if(leido == -2) {
			err(1, "BIO_read: the operation is not implemented in the specific BIO type");
		}else if (leido == 0 || t > BLOCK_SIZE) {
			 break; // Hemos terminado de leer
		}

		t = t + leido;
	}

	BIO_free_all(Base64);
}

RSA *
readPubkey(char * pathPubkey)
{
	FILE *fp;
	RSA * RSAKey;

	printf("%s\n",pathPubkey );
	fp = fopen(pathPubkey, "r"); // hay que abrir asi el fichero ya que la funcion PEM_read_RSAPublicKey necesita que fp sea de tipo FILE
	if (fp < 0){
    	err(1, "fopen %s", pathPubkey);
    }

	RSAKey = RSA_new();
	if (RSAKey == NULL) {
		err(1, "readPubkey: RSA_new(): RSAKey is NULL");
	}

	RSAKey = PEM_read_RSA_PUBKEY(fp, &RSAKey, NULL, NULL);
	if (RSAKey == NULL) {
		err(1, "PEM_read_RSAPrivateKey %s", pathPubkey);
	}
	return RSAKey;
}

void
desencryptarMSG(unsigned char * BLOCK, RSA * RSAKey, unsigned char * C_BLOCK) {
	int sig_size = 0;

	sig_size = RSA_public_decrypt(BLOCK_SIZE, C_BLOCK, BLOCK, RSAKey, RSA_NO_PADDING);
	if (sig_size < 0) {
		fprintf(stderr, "%s\n",  ERR_error_string(ERR_get_error(), NULL));
	}
}

int
comprobarPadding(unsigned char * BLOCK) {
	int padding = 0; // indicara si el padding es correcto o no

	if (BLOCK[0] != 0x00 || BLOCK[1] != 0x01 || BLOCK[PADDING_SIZE -1] != 0x00) {
		padding = -1;
	}

	for (int i = 2; i < PADDING_SIZE -1; i++) {
		if(BLOCK[i] != 0xFF) {
			padding = -1;
			break;
		}
	}
	return padding; //si padding es 0 todo ok; si padding es -1, ko
}

void
sacarHashFirma(unsigned char * BLOCK, unsigned char * HASH)
{
	for (int i = 0 ; i < L_SHA512; i++) {
		HASH[i] = BLOCK[i + PADDING_SIZE + LEN_ID];
	}
}

int
compararHASHES(unsigned char * md, unsigned char * HASH)
{
	int cmp = 0;

	for (int i = 0; i < L_SHA512; i++) {
		if (md[i] != HASH[i]) {
			cmp = -1;
			break;
		}
	}

	return cmp;
}

int
main(int argc, char const *argv[])
{
    int i, cmp, r_padding, result;
    char * path_fichero;
	char * pathPrivateKey;
	char * pathPubkey;
	char * pathSignature;
    unsigned char text[TEXT_SIZE];
	unsigned char md[L_SHA512];
	unsigned char HASH[L_SHA512];
	unsigned char IDsha512[LEN_T];
	unsigned char PADDING[PADDING_SIZE];
	unsigned char BLOCK[BLOCK_SIZE];
	unsigned char C_BLOCK[BLOCK_SIZE];
	RSA *RSAKeyPrivada;
	RSA *RSAKeyPublica;

    memset(text, 0 , TEXT_SIZE);
	memset(md, 0, L_SHA512);
	memset(IDsha512, 0, LEN_T);
	memset(PADDING, 0, PADDING_SIZE);
	memset(BLOCK, 0, BLOCK_SIZE);
	memset(HASH, 0, L_SHA512);

	if(argc == 3){ // Creacion de la firma digital
		path_fichero = (char*)argv[1];
		pathPrivateKey = (char*)argv[2];

	    i = sha512(path_fichero, text, TEXT_SIZE, md);
	    if (i < 0) {
	        exit(1);
	    }

		concatenaIDsha512(md, IDsha512);

		// for (size_t i = 0; i < LEN_T; i++) {
		// 	printf("%02x", IDsha512[i]);
		// }
		// printf("\n");

		createPadding(PADDING);
		// for (size_t i = 0; i < PADDING_SIZE; i++) {
		// 	printf("%02x", PADDING[i]);
		// }
		// printf("\n");

		concatenaPaddingHash(PADDING, IDsha512, BLOCK);

		// for (size_t i = 0; i < BLOCK_SIZE; i++) {
		// 	printf("%02x", BLOCK[i]);
		// }
		//printf("\n");

		RSAKeyPrivada = readPrivkey(pathPrivateKey); //le pasamos la clave privada y la estructura RSA que vamos a rellenar
		generarFirmaDigital(BLOCK, RSAKeyPrivada, C_BLOCK);

	}else if (argc == 5) { // Verificacion de la firma digital
		path_fichero = (char*)argv[3];
		pathPubkey = (char*)argv[4];

		for (int i = 1; i < argc; i++) { // Miramos en los argumentos que nos pasan y comprobamos que esté el -v
			cmp = strncmp(verb_v, argv[i], sizeof(verb_v));
			if (cmp == 0 && i != argc -1) { // si el -v aparece y no es el ultimo elemento, asignamos a la variable pathSignature la firma que queremos comprobar
				pathSignature = (char*)argv[i+1];
				break;
			}else if (i == argc -1) { // si llegamos al final y no hemos encontrado el -v significa que los argumentos que nos han pasado estan mal
				puts("Usage: [-v signature.pem] myfile.txt privkey.pem");
				exit(1);
			}
		}

		leerFirmaB64(pathSignature, C_BLOCK); // le pasamos la firma en base 64 y el bloque donde vamos a almacenar lo que leamos de ella en "base normal"

		// for (size_t i = 0; i < BLOCK_SIZE; i++) {
		// 	printf("%02x", C_BLOCK[i]);
		// }
		// printf("\n");

		RSAKeyPublica = readPubkey(pathPubkey);
		desencryptarMSG(BLOCK, RSAKeyPublica, C_BLOCK);

		for (size_t i = 0; i < BLOCK_SIZE; i++) {
			printf("%02x", BLOCK[i]);
		}
		printf("\n");

		r_padding = comprobarPadding(BLOCK);
		if (r_padding == 0) {
			printf("padding ok\n");
			sacarHashFirma(BLOCK, HASH);
			// for (size_t i = 0; i < L_SHA512; i++) {
			// 	printf("%02x", HASH[i]);
			// }
			// printf("\n");
			i = sha512(path_fichero, text, TEXT_SIZE, md);
		    if (i < 0) {
		        exit(1);
		    }

			result = compararHASHES(md, HASH);
		}

	}else{
		puts("Usage: [-v signature.pem] myfile.txt privkey.pem");
		exit(1);

	}
    exit(0);
}
