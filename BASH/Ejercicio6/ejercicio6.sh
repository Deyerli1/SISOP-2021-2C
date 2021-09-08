#!/bin/bash


# Actividad Pactica de Laboratorio Nro. 1 - Ejercicio 6
# Fierro, Agustin Gabriel- 42.427.695
# Albanesi, Matias - 39.770.388
# Rodriguez, Ezequiel Nicolás - 40.135.570
# Jimenez Vitale, Matias - 34.799.834
# Cambiasso, Tomas - 41.471.465


# inicio declaracion de funciones
function sumar() {
	echo "" | awk -v num_awk=$1 -v num2_awk=$2 '
	{
		print num_awk+num2_awk
	}'
}

function restar() {
	echo "" | awk -v num_awk=$1 -v num2_awk=$2 '
	{
		print num_awk-num2_awk
	}'
}

function dividir() {
	echo "" | awk -v num_awk=$1 -v num2_awk=$2 '
	{
		if(num2_awk == 0) {
			print "no es posible dividir por 0"
			exit 1
		}
		print num_awk/num2_awk
	}'
}

function multiplicar() {
	echo "" | awk -v num_awk=$1 -v num2_awk=$2 '
	{
		print num_awk*num2_awk
	}'
}

function ayuda() {
	echo "***********************************************************************"
	echo "* Este script sirve para realizar las operaciones aritmeticas basicas *"
	echo "* Con -n1 indicamos que el siguiente valor se trata del operando A    *"
	echo "* Con -n2 indicamos que el siguiente valor se trata del operando B    *"
	echo "* Operaciones(-suma, -resta, -division, -multiplicacion)              *"
	echo "* En caso de hacer una division -n2 es el 'divisor'                   *"
	echo "* En caso de hacer una resta n2 es el sustraendo                      *"
	echo "* ej: ./ejercicio6.sh -n1 4 -n2 6 -suma                               *"
	echo "* ej: ./ejercicio6.sh -n1 4 -n2 6 -division                           *"
	echo "* ej: ./ejercicio6.sh -multiplicacion -n2 4 -n1 4                     *"	
	echo "***********************************************************************"
}
# fin de declaracion de funciones

#valido que no se pasen mas de los argumentos solicitados
if [[ $# > 5 ]] ; then
	echo "Exceso de argumentos"
	exit 1
fi

#lectura de argumentos
tipoOperacion=""
while true; do
	case $1 in
		-h | -? | -help) ayuda; exit 0 ;;
		-n1) num1=$2; shift; shift ;;
		-n2) num2=$2; shift; shift ;;
		-suma) tipoOperacion="-suma"; shift ;;
		-resta) tipoOperacion="-resta"; shift ;;
		-multiplicacion) tipoOperacion="-multiplicacion"; shift ;;
		-division) tipoOperacion="-division"; shift ;;
		* ) break;;
	esac
done

#Validacion de numeros
isDataOk=$(echo "" | awk -v num_awk=$num1 -v num2_awk=$num2 '
	{
		isnum = num_awk+0; isnum2 = num2_awk+0;
		if( isnum != num_awk || isnum2 != num2_awk ){ print 0 } else { print 1 }
	}')
	
if [[ isDataOk -eq 0 ]]; then
	echo "Argumentos invalidos"
	exit 1
fi

#Validacion de la operacion a realizar
case $tipoOperacion in
	-suma) sumar $num1 $num2 ;;
	-resta) restar $num1 $num2 ;;
	-multiplicacion) multiplicar $num1 $num2 ;;
	-division) dividir $num1 $num2 ;;
	*) echo "Codigo de operacion invalido"; exit 1 ;;
esac
