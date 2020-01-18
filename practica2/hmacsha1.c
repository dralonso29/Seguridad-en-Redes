// practica2 Seguridad en Redes de Ordenadores
//gcc -Wall hmacsha1.c -lssl -lcrypto

#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

int K_SIZE = 64;
int TEXT_SIZE = 50;

void
converthexa(unsigned char *md)
{
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		printf("%02x",md[i]);
	}
	printf("\n");
}

void
hmacsha1(char * path_fichero, unsigned char *text, int text_len, unsigned char *key, int key_len, unsigned char *md)
{
	SHA_CTX c;
	unsigned char k_ipad[K_SIZE];
	unsigned char k_opad[K_SIZE];
	int fd, nr;

	memset( k_ipad,0 , K_SIZE );
	memset( k_opad,0 , K_SIZE );
	memcpy( k_ipad, key, key_len );
	memcpy( k_opad, key, key_len );

	for (int i = 0; i < K_SIZE; i++)
	{
		k_ipad[i] ^= 0x36; //XOR con la primera cte
		k_opad[i] ^= 0x5c; //XOR con la segunda cte
	}

	//hacemos la SHA1 de k_ipad
	SHA1_Init(&c);
	SHA1_Update(&c, k_ipad, K_SIZE);

	//ahora procedemos a leer de fichero para obtener el texto he ir alimentando a la HASH

	fd = open(path_fichero, O_RDONLY); //abrimos el path del fichero que contiene el mensaje
	if (fd < 0){
		err(1, "open %s", path_fichero);
	}

	for (;;){
		nr = read(fd, text, text_len);

		if (nr < 0){
			close(fd);
			err(1, "read");
		}

		if (nr == 0){	//fin de fichero
			break;
		}

		SHA1_Update(&c, text, nr); //alimentamos a la HASH con lo que hemos leido del fichero ////nr es el numero de bytes que leemos por cada pasada/////

	}

	if (close(fd) < 0){
		err(1, "close");
	}

	//cuando no queda mas por leer...
	SHA1_Final(md, &c);

	//una vez hecha la SHA1 de k_ipad y el mensaje, procedemos a hacer la SHA1 de k_opad y lo anterior
	SHA1_Init(&c);
	SHA1_Update(&c, k_opad, K_SIZE); //alimentamos a la HASH con el XOR de k_opad
	SHA1_Update(&c, md, SHA_DIGEST_LENGTH);	 //concatenamos lo de la linea anterior con la SHA1 de k_ipad y el mensaje
	SHA1_Final(md, &c);

	converthexa(md);
}

void
readkey(char *path_fich, unsigned char *key, int size_key) //leemos de fichero la key de menos de 64 bytes
{
	int fd, nr;
	int t = 0;
	int s = size_key; //s toma como valor el tamaño de la key

	fd = open(path_fich, O_RDONLY);

	if (fd < 0){
		err(1, "open %s", path_fich);
	}

	for (;;){
		nr = read(fd, key + t, size_key - t);

		if (nr < 0){
			close(fd);
			err(1, "read");
		}

		if (nr == 0){	//fin de fichero
			break;
		}

		t = t + nr;
		s = s - nr;

		if (t > size_key || s == 0){ //miramos si hemos llegado al tamaño maximo del fichero
			break;
		}
	}

	if (close(fd) < 0){
		err(1, "close");
	}
}

int
main(int argc, char const *argv[])
{
	char * path_fichero = (char*)argv[1];
	char * path_key = (char*)argv[2];
	unsigned char key[K_SIZE];
	unsigned char buffer_text[TEXT_SIZE];
	unsigned char md[SHA_DIGEST_LENGTH];

	if (argc < 3){
		puts("Usage: ./nombreprograma myfile.txt key");
		exit(1);
	}

	memset(key, 0, K_SIZE);
	memset(buffer_text, 0 , TEXT_SIZE);
	memset(md, 0, SHA_DIGEST_LENGTH);

	readkey(path_key, key, K_SIZE);
	hmacsha1(path_fichero ,buffer_text, TEXT_SIZE, key, K_SIZE, md);

	exit(0);
}
