//          prueba de sha512 sobre los datos de un fichero
//              gcc -Wall -g sign.c -lssl -lcrypto

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

int L_SHA512 = SHA512_DIGEST_LENGTH; // long = 64
int TEXT_SIZE = 50;

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

    if (close(fd) < 0){
    	err(1, "close");
    }

    //cuando no queda mas por leer...
    SHA512_Final(md, &c);

    converthexa(md);

    return stt;
}


int
main(int argc, char const *argv[])
{
    printf("%d\n", L_SHA512);
    int i;
    char * path_fichero = (char*)argv[1];
    unsigned char text[TEXT_SIZE];
	unsigned char md[L_SHA512];

    memset(text, 0 , TEXT_SIZE);
	memset(md, 0, L_SHA512);

    i = sha512(path_fichero, text, TEXT_SIZE, md);
    if (i < 0) {
        exit(1);
    }
    exit(0);
}
