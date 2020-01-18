He lanzado una captura en la interfaz de r1:  tcpdump -i r1-eth1 -s 0 -w ipsec-hola.cap
luego he lanzado un server udp: nc -l -u -p 7777
ahora lanzo el cliente: nc -u 10.18.1.10 7777

desde el cliente escribo cosas, como hola o adios.
paramos la captura y vemos que aparecen los datos cifrados.
si la hash es debil, con meterla en google directamente te descifra que palabra genera dicha hash. Por ejemplo, para la palabra hola, genera la hash: 686f6c610a.
