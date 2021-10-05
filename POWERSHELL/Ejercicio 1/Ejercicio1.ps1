
# Script: Ejercicio1.ps1
# Actividad Pactica de Laboratorio Nro. 2 - Ejercicio 1 - Primera Entrega

### Integrantes ###
# Fierro, Agustin Gabriel- 42.427.695
# Albanesi, Matias - 39.770.388
# Rodriguez, Ezequiel Nicolás - 40.135.570
# Jimenez Vitale, Matias - 34.799.834
# Cambiasso, Tomas - 41.471.465

[CmdletBinding()]
Param (
 [Parameter(Position = 1, Mandatory = $false)]
 # Valida que el Path pasado sea un directorio
 [ValidateScript( { Test-Path -PathType Container $_ } )]
 [String] $param1,
 [int] $param2 = 0
)
# Guarda todos los subdirectorios del directorio recibido
$LIST = Get-ChildItem -Path $param1 -Directory

# Por cada uno de estos subdirectorios se guarda el nombre y la cantidad de elementos que tiene en un nuevo objeto
$ITEMS = ForEach ($ITEM in $LIST) {
 $COUNT = (Get-ChildItem -Path $ITEM).Length
 $props = @{
 name = $ITEM
 count = $COUNT
 }
 New-Object psobject -Property $props
}

# Se ordenan los subdirectorios por cantidad de elementos en forma descendente, luego se seleccionan los primeros X y se guarda
# su nombre
$CANDIDATES = $ITEMS | Sort-Object -Property count -Descending | Select-Object -First $param2 | Select-Object -Property name

Write-Output "Los primeros $param2 subdirectorios con mas elementos son:" 
# Muestra los subdirectorios ordenados en formato de tabla sin los header
$CANDIDATES | Format-Table -HideTableHeaders

    # 1. ¿Cuál es el objetivo de este script?, ¿Qué parámetros recibe? Renombre los parámetros con un nombre adecuado.
        # El objetivo de este script es tomar todos los sub-directorios del directorio
        # pasado mediante el primer parametro y conseguir la cantididad de elementos que hay dentro de
        # cada uno de estos para luego mostrar (una cantadidad determinada por el segundo parametro)
        # estos en forma ordenada descendente.
        # param1 -> Path o Directorio
        # param2 -> Cantidad
    
    # 4. ¿Agregaría alguna otra validación a los parámetros? ¿Existe algún error en el script?
        # Validaria que $Cantidad sea mayor a 0 para que realmente muestre algo el script. Tambien la validacion de $Path
        # se hace pero no tira ningun mensaje de warning, error, ni termina el script, por lo que el script no hace nada si el $Path
        # es invalido y sigue su ejecucion como si lo fuera, lo que podria causar errores. 

    # 5. ¿Para qué se utiliza [CmdletBinding()]?
        # [CmdletBinding()] se usa para transformar una funcion simple en una funcion avanzada. Esto agrega automaticamente   
        # algunos parametros comunes para todas las funciones avanzadas. Algunos de estos parametros son Verbose y Debug
        # entre muchos otros

    # 6. Explique las diferencias entre los distintos tipos de comillas que se pueden utilizar en Powershell.
       # Comilla Simple: si se utilizan, PS no realiza la expansion de variables (texto fuerte)
       # Comilla Doble: si se utilizan, PS realiza la expansion de variables (texto debil)

    # 7. ¿Qué sucede si se ejecuta el script sin ningún parámetro?
        # Si se ejecuta el script sin ningun parametro, este no tira ningun error e imprime el mensaje de Write-Output, sin
        # embargo el script no hace y no muestra nada mas alla de eso. 
        


