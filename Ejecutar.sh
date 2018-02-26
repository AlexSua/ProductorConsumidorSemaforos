#!/bin/bash
echo "Introduce N (tamaño buffer)"
read numero
echo "Introduce numero elementos a producir"
read elem
echo "Introduce numero de productores"
read prod
echo "Introduce numero de consumidores"
read cons
let floor=elem/prod #calcula floor de los elementos a producir
sed -i "s|#define N 10|#define N $numero|" iniciador.c #edita buffer para
#que tamaño sea = a $numero en iniciador.c
sed -i "s|#define N 10|#define N $numero|" productor.c # = que el anterior para productor
sed -i "s|for (i=0;i<10;i++)|for (i=0;i<$floor;i++)|" productor.c #edita elementos
#a producir para productores donde el numero será el suelo de elem/prod.
sed -i "s|#define N 10|#define N $numero|" consumidor.c # = que para productor para consumidor
sed -i "s|for (i=0;i<10;i++)|for (i=0;i<$floor;i++)|" consumidor.c #lo mismo que para el
#productor para consumidor
if [ -f iniciador.c ];then
gcc iniciador.c -lpthread -o iniciador #compila iniciador si existe archivo
else
echo "No existe consumidor"
sleep 2
exit 1
fi;
if [ -f productor.c ];then
gcc productor.c -lpthread -o productor #compila productor si existe archivo
else
echo "No existe productor"
sleep 2
exit 1
fi;
if [ -f consumidor.c ];then
gcc consumidor.c -lpthread -o consumer #compila consumidor si existe archivo
else
echo "No existe consumidor"
sleep 2
exit 1
fi;
#Vuelve a editar los archivos al valor inicial
sed -i "s|#define N $numero|#define N 10|" iniciador.c
sed -i "s|#define N $numero|#define N 10|" productor.c
sed -i "s|#define N $numero|#define N 10|" consumidor.c
sed -i "s|for (i=0;i<$floor;i++)|for (i=0;i<10;i++)|" consumidor.c
sed -i "s|for (i=0;i<$floor;i++)|for (i=0;i<10;i++)|" productor.c
./iniciador
a=0
let b=($prod-1)
until [ "$a" -ge $prod ]
do
#El ultimo productor produce el techo de elementos entre productores.
#Si no es el ultimo productor, cada productor produce el suelo de elem entre productores.
  if [ "$a" ==  "$b"  ];then
  let ceil=($elem+$prod-1)/$prod
  echo "$ceil"
  sed -i "s|for (i=0;i<10;i++)|for (i=0;i<$ceil;i++)|" productor.c
  gcc productor.c -lpthread -o productor
  gnome-terminal --tab -e "/bin/bash -c './productor; exec /bin/bash -i'"
  sed -i "s|for (i=0;i<$ceil;i++)|for (i=0;i<10;i++)|" productor.c
  else
  gnome-terminal --tab -e "/bin/bash -c './productor; exec /bin/bash -i'"
  fi
  sleep 2
let "a+=1"
done
#gnome-terminal iniciara una ventana nueva para cada productor/consumidor.
read -s -n1 -p "Pulsa una tecla para continuar" #Pulsar una tecla para continuar ejecucion
a=0
let b=($cons-1)
until [ "$a" -ge $cons ]
do
#El ultimo consumidor consume el techo de elementos entre consumidores.
#Si no es el ultimo consumidor, cada consumidor pconsume el suelo de elem entre consumidores.
  if [ "$a" ==  "$b"  ];then
  let ceil=($elem+$cons-1)/$cons
  echo "$ceil"
  sed -i "s|for (i=0;i<10;i++)|for (i=0;i<$ceil;i++)|" consumer.c
  gcc productor.c -lpthread -o productor
  gnome-terminal --tab -e "/bin/bash -c './consumer; exec /bin/bash -i'"
  sed -i "s|for (i=0;i<$ceil;i++)|for (i=0;i<10;i++)|" consumer.c
  else
  gnome-terminal --tab -e "/bin/bash -c './consumer; exec /bin/bash -i'"
  fi

let "a+=1" #incrementa en 1 el valor de a
done

