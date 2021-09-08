#!/bin/bash

# Actividad Práctica de Laboratorio Nro. 1 - Ejercicio 4
# Fierro, Agustin Gabriel- 42.427.695
# Albanesi, Matias - 39.770.388
# Rodriguez, Ezequiel Nicolás - 40.135.570
# Jimenez Vitale, Matias - 34.799.834
# Cambiasso, Tomas - 41.471.465

function ayuda() {
	echo "Este script sirve para recopilar la información de las distintas sucursales, generando un resumen en un archivo JSON llamado “salida.json”."
	echo
	echo "Los parametros de entrada son:"
	echo "-d directorio: directorio donde se encuentran los archivos CSV de las sucursales."
    echo "-e sucursal: parámetro opcional que indicará la exclusión de alguna sucursal a la hora de generar el archivo unificado."
    echo "-o directorio: directorio donde se generará el resumen (salida.csv)."
	echo
	echo "ej: ./ejercicio4.sh -d ./ejemplos -e ""Moron"" -o ./dirSalida"
	echo "ej: ./ejercicio4.sh -d ./ejemplos -o ./dirSalida"
}

if [[ $1 == "-h" || $1 == "-help" ]] ; then
	ayuda
	exit 0
fi

if [[ $# -ne 4 ]] && [ $# -ne 6 ] ;then
    echo "La cantidad de parametros ingresada es incorrecta"
    exit 1
fi

destdir=""
filtro=""

if [[ $# -eq 4 ]] ;then
    destdir="$(basename $4)"
elif [[ $# -eq 6 ]] ;then
    destdir="$(basename $6)"
    filtro="$4"
fi

#echo "$destdir"
#echo "$filtro"

if ! test -d "$destdir";then
    mkdir -p $destdir
fi

sucursales="$(basename $2)"
#echo "$sucursales"

result=""
files=`find "$sucursales" -iname '*.csv'`
for filename in $files
do
    #echo $filename
    onlyname=$(basename "$filename" | cut -d. -f1)

    #echo $filtro
    #echo $onlyname
    
    if [[ $filtro != "" ]] && [[ $filtro == $onlyname ]] ;then
        continue
    fi

    if [ -s $filename ]; then
        #echo "The file is not-empty."
        var=$(awk 'BEGIN {FS="," } NR>1 {suma[tolower($1)]+=$2} END { for (name in suma) printf "\n%s %d",name,suma[name] }' $filename)
        result+="$var"
    else
        echo "El archivo $onlyname.csv esta vacio."
        continue
    fi
    
done

order=$(printf "%s\n" "$result" | sort)
#echo "$order"
#printf "\n"

declare -A arrayAsociativo

while IFS= read -r line; do
    stringarray=($line)
    if [[ ${#stringarray[@]} > 0 ]]
    then
        key="${stringarray[0]^}"
        value="${stringarray[1]}"

        #echo $key: $value

        temp_value=0
        if [[ ${arrayAsociativo[${key}]} ]]; then 
            #echo "Exists";
            temp_value=${arrayAsociativo[${key}]}
            #echo $temp_value
            total=$(($value+$temp_value))
            arrayAsociativo[$key]=$total
        else
            #echo "NOT Exists"; 
            arrayAsociativo+=([$key]=$value)
        fi
    fi
done <<< "$order"

# for KEY in "${!arrayAsociativo[@]}";
# do 
#     printf "%s=%s\n" "$KEY" "${arrayAsociativo[$KEY]}";
# done

#printf "\n"
count=1
json="{ "
keys=( $( echo ${!arrayAsociativo[@]} | tr ' ' $'\n' | sort ) )
total_elements=${#keys[@]}
#echo "TOTAL: $total_elements"
for k in ${keys[@]}; do
    #echo "$k=${arrayAsociativo[$k]}"
    if [[ $total_elements != $count ]];then 
        json+=$(printf "\"%s\": %d, " "$k" "${arrayAsociativo[$k]}")
    else
        json+=$(printf "\"%s\": %d" "$k" "${arrayAsociativo[$k]}")
    fi
    ((count=count+1))
done
json+=" }"

destdir=$destdir"/salida.json"
echo $json > $destdir

# assoc2json() {
#     declare -n v=$1
#     printf '%s\0' "${!v[@]}" "${v[@]}" |
#     jq -Rs 'split("\u0000") | . as $v | (length / 2) as $n | reduce range($n) as $idx ({}; .[$v[$idx]]=$v[$idx+$n])'
# }

# destdir=$destdir"/salida.json"

# assoc2json arrayFINAL > $destdir