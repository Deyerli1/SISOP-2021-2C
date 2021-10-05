<#
.SYNOPSIS

Script basico para realizar las operaciones aritmeticas basicas
(Suma, Resta, Division, Multiplicacion)

.DESCRIPTION
Se realizaran las operaciones aritmeticas basicas con los numeros -n1 y -n2
el numero

.PARAMETER n1
Parametro de tipo numerico obligatorio (Operando A)
.PARAMETER n2
Parametro de tipo numerico obligatorio (Operando B)
.PARAMETER suma
Parametro de tipo Switch (Indica que se realzara una suma)
.PARAMETER resta
Parametro de tipo Switch (Indica que se realzara una resta)
.PARAMETER division
Parametro de tipo Switch (Indica que se realzara una division)
.PARAMETER multiplicacion
Parametro de tipo Switch (Indica que se realzara una multiplicacion)

.EXAMPLE
.\EJ6.ps1 -n1 5 -n2 10 -suma

.EXAMPLE
.\EJ6.ps1 -n1 20 -n2 10 -resta

.EXAMPLE

.\EJ6.ps1 -n1 10 -n2 2.5 -division

.EXAMPLE
.\EJ6.ps1 -n1 -5 -n2 -5 -multiplicacion
#>


#---------------------------Parametros----------------------------------------
param(
    [Parameter(Mandatory=$true,HelpMessage = "Parametro tipo numerico obligatorio")][Double]$n1,
    [Parameter(Mandatory=$true,HelpMessage = "Parametro tipo numerico obligatorio")][Double]$n2,
    [Parameter(Mandatory=$false,ParameterSetName="suma")][Switch]$suma,
    [Parameter(Mandatory=$false,ParameterSetName="resta")][Switch]$resta,
    [Parameter(Mandatory=$false,ParameterSetName="division")][Switch]$division,
    [Parameter(Mandatory=$false,ParameterSetName="multiplicacion")][Switch]$multiplicacion
)
#----------------------------FUNCIONES----------------------------------------
function sumar([Double]$num1,[Double]$num2){
    Write-Host ($num1+$num2)
    exit
}

function restar([Double]$num1,[Double]$num2){
    Write-Host ($num1-$num2)
    exit
}

function multiplicar([Double]$num1,[Double]$num2){
    Write-Host ($num1*$num2)
    exit
}

function dividir([Double]$num1,[Double]$num2){
    if($num2 -eq 0){
        throw "No es posible dividir por 0"
        exit
    }
    Write-Host ($num1/$num2)
    exit
}
#------------------------------MAIN---------------------------------------------
if($suma){sumar $n1 $n2;}
if($resta){restar $n1 $n2;}
if($division){dividir $n1 $n2;}
if($multiplicacion){multiplicar $n1 $n2;}