#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

int 
main(int argc, char const *argv[])
{
	// a la hora de compilar se genera un ejecutable llamado a.out
	SHA_CTX context;
	unsigned char digest[SHA_DIGEST_LENGTH];

	SHA1_Init(&context);
	SHA1_Update(&context, "hola", 4);
	SHA1_Final(digest,&context);

	//hacerme una funcion que me lo pase a hexadecimal %02x

	for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		printf("%02x",digest[i]);
	}

	//printf("SHA_DIGEST_LENGTH: %d\n", SHA_DIGEST_LENGTH);

}
