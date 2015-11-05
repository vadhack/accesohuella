#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <EEPROM.h>
/*
Conexiones Sensor Huella Digital:
Sensor ---> Arduino Mega
Cable Negro --> GND
Cable Blanco --> TX 
Cable Verde --> RX 
Cable Rojo --> 5V
Cable Amarillo --> No se conecta
Cable Azul --> No se conecta
Aquí hay un vídeo que explica paso a paso como conectarlo.
https://www.youtube.com/watch?v=1diFaa5OsFg#t=114
*/

#include <LiquidCrystal.h>
#include <Keypad.h>

uint8_t getFingerprintEnroll(int id);


int fingp_pverde = 12,
    fingp_pblanco = 13;
     
SoftwareSerial mySerial(fingp_pverde , fingp_pblanco);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char hexaKeys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'},
};
byte rowPins[ROWS] = {31, 33, 35, 37}; 
byte colPins[COLS] = {39, 41, 43, 45};

Keypad teclado = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

int lcd_RS_p04  =   30,
    //lcd_RW_p05=   GND,
    lcd_E_p06   =   32,
    lcd_D4_p11  =   40,
    lcd_D5_p12  =   38,
    lcd_D6_p13  =   36,
    lcd_D7_p14  =   34;
LiquidCrystal lcd(lcd_RS_p04 , lcd_E_p06, lcd_D4_p11, lcd_D5_p12, lcd_D6_p13, lcd_D7_p14);



char tecla;
String SECCION = "MENU";
String REGISTRADOS[100];
char REGISTRO_NUEVO[8] = "";
boolean REGHUELLA = false;
int numReg = 0, 
	_cursor = 0,
	chipSelect = 53,
	LEDVERDE = 49,
	LEDROJO = 47; 
int address_numreg = 0;
File dataFile;


String usuarios[20];
char *pch;

void setup(){
  	pinMode (LEDVERDE, OUTPUT);
  	pinMode (LEDROJO, OUTPUT);
  	lcd.begin(16, 2);
    Serial.begin(9600);
	  lcdat(0,0,"Cargando...");

	//SD
	pinMode(10, OUTPUT);
	if (!SD.begin(chipSelect)) {
		lcd.clear();
		lcdat(0,0, "Tarjeta SD");
        lcdat(0,1, "No detectada");
    	Serial.println("Card failed, or not present");
    	return;
  	}
  	Serial.println("card initialized.");

  	Serial.println("fingertest");
  	finger.begin(57600);
  	if (finger.verifyPassword()) {
    	Serial.println("Sensor encontrado.");
  	}else {
        lcdat(0,0, "Sensor Huella");
        lcdat(0,1, "No detectado");
    	Serial.println("Sensor no reconocido");
        while (1);
  	}
  	
  	loadNumreg();
	irSeccion("MENU");
}

void loop(){
	tecla = teclado.getKey();
	checkGeneral();
	if(SECCION == "MENU"){
  		checkMenuCMD();
	}
	if(SECCION == "REGISTRO"){
		checkRegistro();
		registrarHuella();
	}
	if(SECCION == "ACCESO"){
		checkMenuAcceso();
	}
}



//SEccion ACCESO
void printMenuAcceso(){
	lcdat(0,0,"Ingrese Huella");
	lcdat(0,1,"");
}
void MenuAccesoPre(){

}
void checkMenuAcceso(){
	getFingerprintIDez();
}
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  AccessOk(finger.fingerID);
  return finger.fingerID; 
}


void AccessOk(int id){
	lcd.clear();
	lcdat(0,0,"Acceso permitido");
	ok();
	String datareaded;
	char *pch;
	char Ruta[12] = {'d', 'a', 't', 'a', 'l', 'o', 'g', '.', 't', 'x', 't', '\0'};
  	datareaded = ReadFile(id,Ruta);
	irSeccion("MENU");
}
 
String ReadFile(int id,char Ruta[]){
	int Lin=0;
	String Resultado;
	File myFile;
	byte bin;
	
	int dir = 0;
	myFile = SD.open(Ruta);
	if (myFile) {
		while (myFile.available()) {
			Serial.write(dataFile.read());
			bin = myFile.read();
			if(bin == 13){
				if(id == atoi(&Resultado[0])){
					Serial.println("Usuario ok");
					saveLista(Resultado);
					myFile.close();
					return Resultado;
				}
				Resultado = "";
				Lin++;
			}else Resultado = Resultado + (char(bin));
		}
		myFile.close();
		return Resultado;
	}
}



//SEccion REGISTRO
void printMenuRegistro(){
	lcdat(0,0, "Ingrese DNI:");
	lcdat(0,1,"");
}
void registrarHuella(){
	if(REGHUELLA){
		getFingerprintEnroll(numReg);
	}
}

void checkRegistro(){
	if(tecla != NO_KEY){//si se presionó una tecla
    	switch(tecla){
    		case 'A':
    			break;
    		case 'B':
    			break;
    		case 'C':
    			break;
    		case 'D':
    			_cursor = 0;
    			irSeccion("MENU");
    			break;
    		case '#':	//Reinicia
    			irSeccion("REGISTRO");
    			break;
    		case '*':
    			if(String(REGISTRO_NUEVO).length()< 8) {
    				return;
    				break;
    			}
    			lcd.clear();
    			lcdat(0,0, "Registrando DNI...");
    			REGISTRADOS[numReg] = String(REGISTRO_NUEVO);//convierte array char a string
    			REGISTRADOS[numReg].remove(REGISTRADOS[numReg].length()-1, 1);//elimina ultimo caracter
    			lcdat(0,1, REGISTRADOS[numReg]);
    			delay(2000);
    			lcd.clear();
				lcdat(0,0,"Ingrese Huella");
    			REGHUELLA = true;
    			break;
    		default:
    			if(!REGHUELLA) printDNI();
    			break;
    	}
    }
}
void terminarRegistro(String text, String text2){
	REGHUELLA = false;
	saveData( String(numReg) + "-" + REGISTRO_NUEVO);
	numReg += 1;
	saveNumreg();
    ok();
    lcd.clear();
    lcdat(0,0, text);
    lcdat(0,1, text2);
    delay(3000);
   	irSeccion("MENU");
}
void terminarRegistroError(String text, String text2){
	REGHUELLA = false;
    alerta();
    lcd.clear();
    lcdat(0,0, text);
    lcdat(0,1, text2);
    delay(3000);
   	irSeccion("MENU");
}
void printDNI(){
	//esto se ejecuta cada vez q se presiona una tecla en seccion REgistro
	if(_cursor < 8){
		lcd.setCursor(_cursor,1);
		lcd.write(tecla);
		setDNIRegistro(tecla);
	}else alerta();
	_cursor += 1;
}
void setDNIRegistro(char charreg){
	REGISTRO_NUEVO[_cursor] = charreg;
}
void MenuRegistroPre(){
	_cursor = 0;
	REGHUELLA = false;
	memset(REGISTRO_NUEVO, 0, sizeof(REGISTRO_NUEVO));
}
uint8_t getFingerprintEnroll(int id) {
	REGHUELLA = false; 
  	int p = -1;
  	Serial.println("Waiting for valid finger to enroll");
  	while (p != FINGERPRINT_OK) {
  	  	p = finger.getImage();
  	  	switch (p) {
  	  	case FINGERPRINT_OK:
  	  	  Serial.println("Image taken");
  	  	  break;
  	  	case FINGERPRINT_NOFINGER:
  	  	  Serial.println(".");
  	  	  break;
  	  	case FINGERPRINT_PACKETRECIEVEERR:
  	  	  Serial.println("Communication error");
  	  	  break;
  	  	case FINGERPRINT_IMAGEFAIL:
  	  	  Serial.println("Imaging error");
  	  	  break;
  	  	default:
  	  	  Serial.println("Unknown error");
  	 	 	  break;
   		}
  	}

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted1");
      break;
    case FINGERPRINT_IMAGEMESS:
    	terminarRegistroError("Imagen","desordenada");
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
    	terminarRegistroError("Error","Communicacion");
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
    	terminarRegistroError("Imagen","desconocida");
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
    	terminarRegistroError("Imagen","desconocida");
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      terminarRegistroError("Error","desconocido");
      return p;
  }
  
    Serial.println("Remove finger");
    delay(2000);
    p = 0;
    while (p != FINGERPRINT_NOFINGER) {
        p = finger.getImage();
    }

    p = -1;
    Serial.println("Place same finger again");
    lcdat(0,0,"Ingrese huella");
    lcdat(0,1, "Otra vez");
    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
        switch (p) {
        case FINGERPRINT_OK:
          Serial.println("Image taken");
          break;
        case FINGERPRINT_NOFINGER:
          Serial.print(".");
          break;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            break;
        default:
            Serial.println("Unknown error");
            break;
        }
    }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
    	terminarRegistroError("Huella","desordenada");
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
    	terminarRegistroError("Error","Communicacion");
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
    	terminarRegistroError("No reconoce","huella");
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
    	terminarRegistroError("No reconoce","huella");
      Serial.println("Could not find fingerprint features");
      return p;
    default:
    	terminarRegistroError("Error","desconocido");
      	Serial.println("Unknown error");
      return p;
  }
  
  
  // OK converted!
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    terminarRegistroError("Error","Communicacion");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    terminarRegistroError("Error lectura","huella");
    return p;
  } else {
  	terminarRegistroError("Error","desconocido");
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    terminarRegistro("Huella","guardada");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
  	terminarRegistroError("Error","comunic.");
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
  	terminarRegistroError("Error","direccion");
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
  	terminarRegistroError("Error","mem. flash");
    Serial.println("Error writing to flash");
    return p;
  } else {
  	terminarRegistroError("Error","desconocido");
    Serial.println("Unknown error");
    return p;
  } 

}

//SEccion MENU
void checkMenuCMD(){
    if(tecla != NO_KEY){//si se presionó una tecla
    	switch(tecla){
    		case 'A':
    			irSeccion("REGISTRO");
    			break;
    		case 'B':
    			irSeccion("ACCESO");
    			break;
    		case '*':
    			irSeccion("MENU");
    			break;
    		case 'D':
    			break;
    		default:
    			lcd.clear();
    			lcdat(0,0,"Comando inválido");
                lcd.setCursor(0,1);
                lcd.write(tecla);
    			alerta();
    			delay(1000);
    			irSeccion("MENU");
    			break;
    	}
    }
}
void printMenuPrincipal(){
  	lcdat(3,0,"Menu Listas");
  	lcdat(0,1, "(A)Reg - (B)Acc");
}
void MenuPrincialPre(){
	_cursor = 0;
	REGHUELLA = false;
	memset(REGISTRO_NUEVO, 0, sizeof(REGISTRO_NUEVO));
}



void checkGeneral(){
	if(tecla != NO_KEY){
		if(tecla == 'D') {
			irSeccion("MENU");
		}
		if(tecla == 'C'){
			borrarRegistros();
		}
	}
}


void borrarRegistros(){
	lcd.clear();
	lcdat(0,0,"Borrando");
	lcdat(0,1, "Huellas...");
	for(int i = 0; i<254; i++){
  	  deleteFingerprint(i);
  	}
  	lcdat(0,0,"Huellas borradas");
  	delay(3000);
  	SD.remove("datalog.txt");
  	SD.remove("lista.txt");
  	lcd.clear();
  	lcdat(0,0,"Archivo log y lista");
  	lcdat(0,1, "Borrandos");
  	EEPROM.write(address_numreg, 0);
  	numReg = 0;
  	delay(1000);
  	irSeccion("MENU");
}


void irSeccion(String seccion){
	lcd.clear();
	if(seccion == "MENU") {
		printMenuPrincipal();
		MenuPrincialPre();
	}
	if(seccion == "REGISTRO") {
		printMenuRegistro();
		MenuRegistroPre();
	}
	if(seccion == "ACCESO"){
		printMenuAcceso();
		MenuAccesoPre();
	}
	SECCION = seccion;
}


void lcdat(int col, int row, String text){
  	lcd.setCursor(col, row);
  	lcd.print(text);
}
void log(String text){
	lcdat(0,0,text);
	Serial.println(text);
}
void alerta(){
	digitalWrite(LEDVERDE, LOW);
	digitalWrite(LEDROJO, HIGH);
	delay(300);
	digitalWrite(LEDROJO, LOW);
	delay(300);
	digitalWrite(LEDROJO, HIGH);
	delay(300);
	digitalWrite(LEDROJO, LOW);
}

void ok(){
	digitalWrite(LEDROJO, LOW);
	digitalWrite(LEDVERDE, HIGH);
	delay(500);
	digitalWrite(LEDVERDE, LOW);
	delay(500);
	digitalWrite(LEDVERDE, HIGH);
	delay(500);
	digitalWrite(LEDVERDE, LOW);
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    return p;
  }   
}

void saveData(String dataString){
	dataFile = SD.open("datalog.txt", FILE_WRITE);
  	if (dataFile) {
  	  	dataFile.println(dataString);
  	  	dataFile.close();
  	  	Serial.println(dataString);
  	} 
}

void saveLista(String dataString){
	dataFile = SD.open("lista.txt", FILE_WRITE);
  	if (dataFile) {
  	  	dataFile.println(dataString);
  	  	dataFile.close();
  	  	Serial.println(dataString +" Saved in lista");
  	} 
}

void loadNumreg(){
	numReg = EEPROM.read(address_numreg);
}

void saveNumreg(){
	EEPROM.write(address_numreg, numReg);
}