-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

PRACTICA DE SEGURIDAD EN REDES DE ORDENADORES.

USO DE GPG Y COMANDOS DE OPENSSL

Esta práctica consiste en aprender a utilizar GPG y los comandos de
OpenSSL para cifrar y descifrar ficheros, y gestionar las claves
públicas del resto de usuarios.

ENTREGA: el usuario debe responder a las preguntas en este mismo
fichero, en el hueco indicado. Las respuestas deben estar escritas en
texto plano UTF-8 **CORRECTAMENTE FORMATEADO** , respondiendo a las
preguntas, resumiendo los pasos que ha tenido que seguir para realizar
cada apartado de la práctica y especificando los comandos que ha
ejecutado. SE DEBEN ESPECIFICAR TODOS LOS COMANDOS PARA REALIZAR CADA
PARTE DE CADA APARTADO DEL ENUNCIADO.

Entregue la memoria como indica el último apartado del enunciado.


1. Cree su par de claves GPG, eligiendo como algoritmo RSA de 4096
bits. Elija una passphrase segura.

COMANDO:
====================================================
Para crear el par de claves: "gpg --gen-key"

Para asegurarnos de que lo hemos hecho bien, podemos listar las claves que
tenemos en el anillo publico, con el comando: "gpg --list-keys"(si es la
primera vez que se utiliza gpg, deberia solo aparecer la que hemos creado)

====================================================

2. Descargue la clave pública del profesor y guárdela en
su anillo de claves. Puede descargar la clave del profesor Enrique Soriano
de:

	http://lsub.org/who/esoriano/index.html

O del servidor de claves de RedIris (pgp.rediris.es). Compruebe que en
ambos sitios la clave pública de Enrique Soriano es la misma.

COMANDOS:
====================================================

Para comparar las claves de un sitio y otro, lo que podemos hacer es guardar
cada clave en un fichero y con ayuda de la shell, y el comando 'cmp', podemos
asegurar que son iguales las claves. comando para añadir claves publicas a nue-
stro anillo: "gpg --import public.asc"

====================================================

3. ¿Puede estar seguro de que esas claves descargadas son auténticas y
pertenecen a Enrique Soriano? ¿Por qué?

RESPUESTA:
====================================================

De primeras, no podemos asegurarlo. Cualqueira podría haberle suplantado la identidad
y se estaría pasando por él.

====================================================

4. Compruebe la autenticidad de la clave del profesor Enrique Soriano
comprobando su  fingerprint con el que ha dado el profesor en
persona.

COMANDOS:
====================================================

Lo primero es comprobar (en nuestro terminal) el fingerprint de la clave de
enrique.soriano@urjc.es, con el comando: "gpg --edit-key enrique.soriano@urjc.es"
El profesor mostro su fingerprint en su ordenador y se comprobo su identidad.

====================================================

4. ¿Puede estar seguro de que la clave descargada es auténtica
y pertenece al profesor?

RESPUESTA:
====================================================

Si, podemos estar seguros, ya que nos ha mostrado su fingerprint y hemos visto
que coincidian tanto la de su ordenador como la del mio.

====================================================

5. Si es así, firme la clave del profesor y suba la firma al servidor
de Rediris.

COMANDOS:
====================================================

Con el comando: "gpg --keyserver pgp.rediris.es --send-keys 7B617F70", subimos nues-
tra clave publica al servidor pgp.rediris.es.
y con : "gpg --edit-key enrique.soriano@urjc.es" firmamos la clave del profesor con nu-
estra clave privada.
Por ultimo, subimos la clave del profesor para actualizar:
"gpg --keyserver pgp.rediris.es --send-keys F90A5363"

====================================================

6. Comparta su clave pública con otras personas de la clase (por ejemplo
por correo electrónico). ¿Cómo exporta su clave pública? Resuma todos los
pasos para compartir su clave pública de forma segura:

COMANDO y RESPUESTA:
====================================================

"gpg --output ~/public-key-drodriguez.asc --armor --export d.rodriguezalon@alumnos.urjc.es"
De esta forma obtengo un fichero .asc que contiene mi clave publica.
A la hora de enviarselo a alguno de mis compañeros, lo que deberiamos hacer es comprobar,
en persona, el fingerprint de cada uno, tal y como hicimos con el profesor.

====================================================

7. Añada las claves de dos compañeros en su anillo. Asigne a cada compañero
el nivel de confianza que desee.

COMANDOS:
====================================================

Al igual que se hizo con anterioridad, se ha usado el comando: "gpg --import clave.asm"
He añadido dos claves a mi anillo de claves. Las he descargado de la pagina pgp.rediris.es
En el caso de querer asegurarme de que son realmente sus claves publicas, deberia comprobarlo
en clase y me deberian enseñar su fingerprint.
Para asignar el nivel de confianza: "gpg --edit-key pepe@gmail.com", y luego "trust"

====================================================


8. Compruebe la autenticidad y la integridad de las tres versiones
del enunciado que están disponibles (1, 2, 3). ¿Puede asegurar que
alguna de las versiones fue publicada por el profesor? ¿Cuál o cuáles?
¿Por qué?

COMANDO y RESPUESTA:
====================================================

Vemos que al hacer: "gpg --verify archivo.sig", nos sale que el primer archivo
si ha sido firmado correctamente (o con la firma correcta).
Sale un campo que nos indica que la firma es buena:
'Good signature from "Enrique Soriano-Salvador <enrique.soriano@urjc.es>"'
Las otras dos tambien han sido firmadas pero al parecer con una clave que no corresponde
con la del primer caso. Salta el siguiente mensaje:
'BAD signature from "Enrique Soriano-Salvador <enrique.soriano@urjc.es>"'

====================================================

9. Descargue del foro de la asignatura el fichero p1secret.gpg,
cifrado con una clave simétrica y descífrelo. El secreto para
descifrar el fichero se comunicará en clase.

COMANDOS:
====================================================

Para descifrar el archivo se ha utilizado el
comando: " gpg --output p1secret1 --decrypt p1secret.gpg "
La palabra secreta es "_-_practica-gpg_-_"

====================================================

10. Descargue la firma del mensaje en claro, p1secret.sig.
¿Puede estar seguro de que el mensaje es integro (no ha sido
modificado por un atacante)? ¿Puede el profesor repudiar dicho
mensaje?

COMANDO Y RESPUESTA:
====================================================

EL comando utilizado es: "gpg --verify p1secret.sig p1secret1" donde p1secret.sig
es la firma de Enrique, y p1secret1 es la imagen que hemos descifrado en el apar-
tado anterior.
EL profesor cifro el archivo con una palabra secreta que el mismo no dijo en clase,
luego, el mensaje parece ser integro. En principio no, ya que cuando lo descifras,
te avisa de que ha sido cifrado por el profesor.

====================================================

11. Cifre esa misma imagen con el algoritmo de clave simétrica AES, con
una clave de 256 bits, usando el comando gpg.

COMANDO:
====================================================

comando utilizado:
"gpg --output fichero.gpg --symmetric \
--cipher-algo AES256 fichero"
la contraseña que me pide no es la misma que mi passphrase.

====================================================

12. Haga lo mismo que en el apartado anterior usando el comando
openssl. Tiene que usar el modo CBC.

COMANDO:
====================================================

comando: "openssl aes-256-cbc -in fichero -out fichero.ssl"
He utilizado la misma palabra secreta que en el caso anterior.

====================================================

13. Calcule el resumen hash SHA-1 para los dos ficheros anteriores.
¿Coinciden? ¿Deberían coincidir? ¿Por qué?

COMANDO Y RESPUESTA:
====================================================

comando para sacar hash del .ssl:"openssl dgst -sha1 fdracif2.ssl"
resultado: SHA1(fdracif2.ssl)= 4e0b2191b588f92cd3c9f0f12409260e76404486
comando para sacar hash del .pgp:"gpg --print-md SHA1 fichero"
resultado: fdracif.gpg: 54AD A46A A902 BD27 7193  F7B2 5525 7DF1 E892 25F0

No coinciden debido a que los cifradores por bloques usados, no son los mismos.
Para el comando gpg se usa CAST5, y para openssl se usa CBC.

====================================================

14. Descifre ambos ficheros y compruebe que los datos descifrados son
exactamente los mismos que los originales.

COMANDOS:
====================================================

comando: " gpg --output p1secret1 --decrypt p1secret.gpg " para descifrar
el archivo .gpg, introduciendo la passfrase anterior.

comando: "openssl aes-256-cbc -d -in fichero.ssl -out fichero" para descifrar
el archivo .ssl introduciendo la passfrase anterior.

Al descifrar los archivos cifrados, volvemos a obtener los archivos originales.

====================================================

15. Genere un certificado X.509 en texto plano para la compañía ACME,
con correo electrónico pepe@acme.jp. La clave RSA tiene que ser de
4096 bits y el certificado debe ser válido por un año desde su
creación. Indique su número de serie:

COMANDOS:
====================================================

para generar el certificado:
"openssl req -new -nodes -x509 -newkey 4096 -out cert.pem -keyout privkey.pem -days 365 \
 -subj "/C=ESP/ST=MAD/L=FUENLABRADA/O=ACME/OU=IT/CN=lsub.org/emailAddress=pepe@acme.jp""

para aplanarlo en texto plano:
"openssl x509 -in cert.pem -inform PEM -text"

====================================================

16. ¿Cómo puede enviar la clave privada del certificado anterior como
cuerpo de un correo electrónico, en texto plano y aplanado en PEM a un
compañero (de los del ejercicio 7)? ¿Puede el profesor descifrar dicho
mensaje si se hace con el correo electrónico enviado? ¿Y si le roba
sus anillos de claves de su directorio $HOME/.gnugp?

COMANDO Y RESPUESTAS:
====================================================

Cifrando con la clave publica del receptor, solamente podria descifrar el mensaje el receptor
con su clave privada. Comando: "gpg --output fichero.gpg \
--encrypt [-r recipient] fichero"

No, el profesor no podria descifrarlo.
Aunque te roben el archivo .gnugp tampoco ya que deben saber la passfrase correspondiente.

====================================================

17. ¿Cómo tendría que descifrar y verificar su compañero el mensaje
del punto anterior?

RESPUESTA:
====================================================

Con su clave privada: "gpg --output fichero --decrypt fichero.gpg"
he introduciendo su passfrase.

====================================================

18. ¿Cuál es el número de serie y el fingerprint del certificado
X509 que usa www.urjc.es?

COMANDOS:
====================================================

me descargo el certificado de la urjc: "openssl x509 -in wwwurjces.crt -inform PEM -text"

Serial Number: 08:10:bf:75:0b:b1:40:9d:fc:1f:13:51:6d:cb:86:d5

para ver el fingerprint: "openssl x509 -in wwwurjces.crt -noout -sha1 -fingerprint"

SHA1 Fingerprint=5F:95:FA:FC:E0:6F:DF:82:0B:F1:AB:F2:81:D2:49:69:6C:87:62:AF

====================================================


19. Verifique la cadena de confianza del certificado del punto anterior
con el comando openssl. En un sistema GNU/Linux, los certificados raíz
del sistema se encuentran en el directorio /etc/ssl/certs.

COMANDOS:
====================================================

comando usado: "openssl verify -verbose -CAfile <(cat DigiCert_High_Assurance_EV_Root_CA.pem
 TERENASSLHighAssuranceCA3.crt) wwwurjces.crt"
Respuesta al ejecutarlo en la shell: wwwurjces.crt: OK



====================================================


20. Entregue este fichero con las respuestas, firmado y cifrado
para que sólo Enrique Soriano pueda leerlo. El fichero se tiene
que llamar practica.gpg. También debe entregar un fichero con su
clave pública, llamado clave.gpg.

Especifique los comandos que va a ejecutar para crear el fichero
practica.gpg:

RESPUESTA:
====================================================

para cifrar y firmar: gpg -o practica.gpg --sign --encrypt -r enrique.soriano@urjc.es \
1-practica.txt.1.sig

para mi clave publica hacemos igual que arriba, ciframos la clave del alumno
con la clave publica del profesor.




====================================================
-----BEGIN PGP SIGNATURE-----
Comment: GPGTools - https://gpgtools.org

iQIcBAEBCgAGBQJYkgGJAAoJEKjW2fUAVLumNB8P+gIdihByzmf/xl8T40EW7VEE
go73kMkdAK3/gVyYwdeCHd+ZR8IDoRPFW2Yi4CuE8EvZbWxFYItUuwUSk4yWRbZz
YvXSg5PoaK7vTrPW0XYj0f9cBWaXVV+OkcQ6OnQbRjc/bmzBS0SKKHy6qpbwlvVm
gHrIDQKuq7nXPIJSxKRbTG9UmLK3wn+25CK+x7rWYcJFsrDNQipDXmipE397ZQBR
XQpMc6JVSeJDpUTmGa9nQeBcHxxZ6/KQ9MwWAkVxfu3UM6l5fZpxp8OBe+y9szgq
kXUUgAk9PjpmSYJjxzeL8cNUEHN/cDu/RxloKr/JFU4Z5YqnIw3VLNiVD9oLmcGQ
b0lBLs8abhJdhT+Yq6gYY+1otrDXq+mfYKqD5N7S/rI5K3CS800C49LwXKfD+dzn
gEdhOqD0e/+RKBscVeQEpuFTtEslakwIzcN66CapfSBqFtseWJvHOY2mUeH2aev6
oEIpPKUJKsSetenOndyldUjIygdc2oXbJqGDOqP2Qudn6VTcyD89nQmWM4t8z0Sb
aza2cc9vtHUv8xYDFeZaUW2Lnpq1wuSE9pHbVKu+trmUEiyyqe3oEYxYesQzWKE2
NKNq8I0I2Db90fD7WPsg0ivkvJ24oBXUaS96Ti7rxxlveawJp8KCBEkVZYm/Tuut
N27YoqR8R7bdcnfU/GMJ
=N3t2
-----END PGP SIGNATURE-----
