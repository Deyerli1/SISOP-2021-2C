#!/bin/bash

# Script: Ejercicio1.sh
# Actividad Pactica de Laboratorio Nro. 1 - Ejercicio 1 - Primera Entrega

### Integrantes ###
# Fierro, Agustin Gabriel- 42.427.695
# Albanesi, Matias - 39.770.388
# Rodriguez, Ezequiel Nicolás - 40.135.570
# Jimenez Vitale, Matias - 34.799.834
# Cambiasso, Tomas - 41.471.465

funcA() {
    # Rutina de ayuda para el usuario del script
    echo "Error. La sintaxis del script es la siguiente:"

    echo "Para listar los primeros 5 directorios dentro de 'directorio' ordenados por cantidad de elementos usar: $0 'directorio' 5"
}
funcB() {
    echo "Error. '$1' no es un directorio "

    #Mejora para punto 5
    #funcA
    #exit
}
# Se valida que el primer parametro sea un directorio.
funcC() {
    if [[ ! -d $2 ]]; then
        funcB
    fi
}

# Se llama a la funcion c para validar los parametros pasados 
funcC $# $1 $2 # $3 $4 $5 No son utilizados por el script

# Se guardan todos los sub-directorios del directorio parametro.
LIST=$(ls -d $1*/)
ITEMS=()

# Por cada directorio guardado, creo un array con la cantidad de elementos y el nombre de cada subdirectorio
for d in $LIST; do
    ITEM="`ls $d | wc -l`-$d"
    ITEMS+=($ITEM)
done

IFS=$'\n'
# Se ordenan los sub-directorios por cantidad de subelementos
sorted=($(sort -rV -t '-' -k 1 <<<${ITEMS[*]}))

# Se reduce el array solo a la cantidad deseada pasada por parametro.
CANDIDATES="${sorted[*]:0:$2}"
unset IFS

echo "Listado de directorios ordenado por cantidad de elementos que tienen dentro"
# Imprimo el nombre y luego la cantidad de elementos
printf "%s\n" "$(cut -d '-' -f 2 <<<${CANDIDATES[*]})"

#### Respuestas ####

# 1. ¿Cuál es el objetivo de este script?, ¿Qué parámetros recibe?
    # El objetivo de este script es tomar todos los sub-directorios del directorio
    # pasado mediante el primer parametro y conseguir la cantididad de elementos que hay dentro de
    # cada uno de esos directorios para luego mostrarlos en forma ordenada.
    # Recibe también un segundo parametro con la cantidad de elementos que se desea mostrar por pantalla.

# 4. ¿Qué nombre debería tener las funciones funcA, funcB, funcC?
    # funcA() deberia ser help() ya que muestra la posible sintaxis o formas de ejecutar el script.
    # funcB() podria ser noDirectorio() ya que muestra un mensaje cuando el primer parametro no es un directorio.
    # funcC() podria ser validar() ya que verifica si el primer parametro del script (segundo de la funcion) es un directorio.

# 5. ¿Agregaría alguna otra validación a los parámetros?, ¿existe algún error en el script?

    # Podría validarse que el segundo parametro pasado sea un numero e indicar un error.
    # Así también como validar la cantidad de parametros pasados.
    # El script envia a la funcion C mas parametros que los necesarios, lo cual puede ser visto como un error.
    # La funcion A, que sirve como ayuda para el usuario, nunca es invocada.


# 6. ¿Qué información brinda la variable $#? ¿Qué otras variables similares conocen? Explíquelas.
    # La variable $# brinda la cantidad de parametros que recibe el script
    # Otras variables similares son:
        # $0 brinda el nombre del script ejecutado.
        # $? brinda el valor de retorno del ultimo comando ejecutado
        # $@ brinda una string con todos los parametros
        # $* brinda una palabra con todos los parametros
        # $$ brinda el Process ID o identificador unico de proceso

#  7. Explique las diferencias entre los distintos tipos de comillas que se pueden utilizar en Shell scripts.
    # Comilla Simple: si se utilizan, Shell no realiza el reemplazo de variables (texto fuerte)
    # Comilla Doble: si se utilizan, Shell realiza el reemplazo de variables (texto debil)
    # Comilla Francesa: si se utilizan, Shell realiza el reemplazo de variables, ejecuta el comando encerrado por ellas y guarda el resultado en la variable.
                         # es equivalente a usar $()

# 8. ¿Qué sucede si se ejecuta el script sin ningún parámetro?
    # Al ejecutar el script sin ningun parametro lo que ocurre es que funcC va a validar que '' no es un directorio
    # por lo que va a arrojar un error. Y muestra un listado vacío.