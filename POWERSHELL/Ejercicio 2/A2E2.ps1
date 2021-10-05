<#
.SYNOPSIS
       	Script que renombra archivos.

.DESCRIPTION
	Un influencer que sube fotos de comida a las redes sociales quiere ordenarlas. Para ello, se decide a
	desarrollar un script que le permita renombrar los archivos de su colección de fotos. Los nombres de
	sus archivos tienen el siguiente formato: “yyyyMMdd_HHmmss.jpg” y desea renombrarlos siguiendo
	este formato: “dd-MM-yyyy (almuerzo|cena) del NombreDia.jpg”. Se considera como cena cualquier
	foto de comida hecha después de las 19hs.

.INPUTS 
        Posee los siguientes parametros
        -Directorio: Carpeta de archivos a procesar.
	-Dia: (Opcional) Es el nombre de un día para el cual no se quieren renombrar los archivos. Los valores posibles para este parámetro son los nombres de los días de la semana (sin tildes). El script tiene que funcionar correctamente sin importar si el nombre del día está en minúsculas o mayúsculas.

#>

param(
	[Parameter(Mandatory=$False)][String]$Directorio,
	[Parameter(Mandatory=$False)][String]$Dia,
)

$global:Hugin=$Directorio
$global:Munin=$Dia

$DB="Sábado Domingo Lunes Martes Miercoles Jueves Viernes"

function Confirm-Directorio{
	if(!(Test-Path $Directorio)){
		Write-Host "Directorio inválido" -ForegroundColor red
		exit 1
	}
	$global:Hugin=Resolve-Path -path $global:Hugin
}

function Confirm-Dia{
	if(!$Dia -or $Dia -eq ""){
		return
	}
	if(!($DB -Match $Dia)){
		Write-Host "Día invalido" -ForegroundColor red
		exit 1
	}
}

function OCD($D1,$D2){
	Get-ChildItem -Path $D1 -Exclude *del* -Recurse -File | Foreach-Object {
		$time = "Almuerzo"
		$name = $_.Name
		$ext = $name.Substring($name.LastIndexOf(".")+1)
		$day = $name.Substring(6, 2);
		$month = $name.Substring(4, 2);
		$year = $name.Substring(0, 4);
		$hour = $name.Substring(9, 2);
		$minute = $name.Substring(11, 2);
		$second = $name.Substring(13, 2);
		$value= (($hour -as [int]) * 3600) + (($minute  -as [int]) * 60) + ($second  -as [int])
	
		if($value -lt 39600){
			$time = "Desayuno"
		}
		if($value -gt 68400){
			$time = "Cena"
		}
		$dayofweek= (get-date -year $year -month $month -day $day).DayOfWeek
		switch ( $dayofweek )
		{
		    'Sunday' { $dayofweek= 'domingo'    }
		    'Monday' { $dayofweek= 'lunes'    }
		    'Tuesday' { $dayofweek= 'martes'   }
		    'Wednesday' { $dayofweek= 'miercoles' }
		    'Thursday' { $dayofweek= 'jueves'  }
		    'Friday' { $dayofweek= 'viernes'    }
		    'Saturday' { $dayofweek= 'sabado'  }
		}
		if($D2 -ne "" -and $dayofweek.ToUpper() -eq $D2.ToUpper() ){
			return
		}
		$dayofweek= "$($day)-$($month)-$($year) $($time) del $($dayofweek)"
		if(Test-Path "$($D1)/$($dayofweek).$($ext)"){
			$i=1;
			While(Test-Path "$($D1)/$($dayofweek)($($i)).$($ext)"){
				$i++;
			}
			$dayofweek="$($dayofweek)($($i)).$($ext)"	
		}else{
			$dayofweek="$($dayofweek).$($ext)"	
		}
		Rename-Item -Path $_ -NewName $dayofweek	
	}
}

Confirm-Directorio 
Confirm-Dia
OCD $global:Hugin $global:Munin
