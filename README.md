# Control de Listas Acceso con Huella digital 

## Desarrollado por www.vadhack.com

Controla y da acceso a personas mediante la lectura de sus huellas digitales. 

Crea una lista de las personas registradas, guarda su DNI y su ID relacionado con su huella digital, la lista de las personas registradas es guardada en un archivo .txt dentro de la tarjeta SD.

Permite el acceso a todas aquellas personas que esten registradas y crea una lista nueva guardandola en otro archivo .txt

Todo el control de los usuarios se verifica mediante la lectura de huellas digitales.

## Materiales

### Arduino 

Mega 2560

#### Sensor de Huella

-Modelo: 071405
-Voltaje de alimentación: 3.6V – 6V
-Corriente de operación: 100mA – 150mA
-Interfaz: UART TTL
-Modo de paridad de huella: 1:1 1:N
-Baud Rate: 9600*N
-N = 1 a 12 (Por defecto es 6)
-Tiempo de adquisición menor a 1 segundo
-5 Niveles de seguridad
-Dimensión de la ventana: 14x18mm
-Entorno de trabajo: -10ºC a 40ºC (Humedad Relativa 40% a 85%)
-Dimensiones: 5.5 x 2.1 x 2.0 cm
-Peso: 22g

#####https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library

####LCD
16 x 2

####Teclado 4x4
#####http://es.slideshare.net/iihhhii/teclado-matricial-4x4-en-ccs-con

####Lector SD
Módulo lector de tarjeta SD y tarjeta SD
#####http://www.ebay.com/itm/SD-Card-Module-Slot-Socket-Reader-Adapter-For-Arduino-ARM-MCU-NEW-S3-/381340259458?hash=item58c9a69482:g:Ve8AAOSw3ydVsgdg

#### Componentes:

Leds : Verde y Rojo

Resitencias 220 Ohm para los Leds

4 Resistencias 1k ohm

4 Resistencias 2k ohm

Un protoboard para conexiones


##USO

El menu principal muestra 2 opciones: 

Letra A: Ir a Menu de registro de DNI y Huella, Primero se ingresa DNI y luego se ingresa huella 2 veces, el LED verde indica registro correcto.

Letra B: Ir menu Acceso, está a la espera de lectura de huella la cual indica mediante el LED verde si tiene acceso permitido.

Los DNI y IDs para huellas son guardados en un archivo: datalog.txt

Los DNI de personas con acceso permitido son guardados en archivo: lista.txt



##Notas

Deben hacer un partidor de tensión de 5v a 3.3v para las conexiones del módulo lector de SD


Para dudas pueden ponerse en contacto conmigo : valentin.arambulo@gmail.com, www.vadhack.com


#####Vendo cualquiera de los componentes utilizados.



