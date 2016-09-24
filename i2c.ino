const int lee_sda = 2;
const int lee_scl = 3;
const int esc_sda = 4;
const int esc_scl = 5;

void setup(){
  Serial.begin(9600);
  pinMode(lee_sda, INPUT);
  pinMode(lee_scl, INPUT);
  pinMode(esc_sda, OUTPUT);
  pinMode(esc_scl, OUTPUT);
  digitalWrite(esc_scl, HIGH);
  digitalWrite(esc_sda, HIGH);
}

void loop(){
  Serial.println("OPERACIONES");
  Serial.println("1. Guardar un valor (de 0 a 255) en una posicion del M24C01.");
  Serial.println("2. Leer una posicion (de 0 a 127) del M24C01.");
  Serial.println("3. Inicializar toda la memoria del M24C01 a un valor.");
  Serial.println("4. Mostrar el contenido de los 128 bytes del M24C01.");
  Serial.println("Nota: Introduzca solamente numeros enteros.");
  Serial.print("\n");
  Serial.println("Que operacion desea realizar?");
  int op = Serial.read(); 
  while (op > 53 || op < 49){
    op = Serial.read();
  }
  if (op == 49){
    Serial.println("Opcion elegida 'Escritura'.");
    Serial.println("Indica la direccion."); 
    byte dir = pedirdir();
    if (dir > B01111111){
      Serial.println("Direccion no valida.");
      Serial.print("\n");
    } else {
      Serial.println("Indica el valor.");
      byte num = pedirdir();
      escribe(dir,num);
      Serial.print("\n");
    }
    op = 54;
  }
  delay(50);
  if (op == 50){
    Serial.println("Opcion elegida 'Lectura'.");
    Serial.println("Indica la direccion."); 
    byte dir = pedirdir();
    if (dir > 127){
      Serial.println("Direccion no valida.");
      Serial.print("\n");
    }else{
      Serial.print("Se ha leido: ");
      leer(dir);
      Serial.print("\n");
    }
    op = 54;
  }
  
  if (op == 51){
    Serial.println("Opcion elegida 'Inicializar memoria a valor' :");
    Serial.println("Indica el valor."); 
    byte num = pedirdir();
    byte dir = B00000000;
    Serial.println("Escribiendo..."); 
    for (int i = 0; i < 128; i++){
      escribe(dir,num);
      dir = dir + B1;
    }
    Serial.println("Operacion realizada con exito.");
    Serial.print("\n");
  }
  
  if (op == 52){
    Serial.println("Opcion elegida 'Visualizar todos los valores guardados:'");
    byte dir = B00000000;
    for (int i = 0; i < 128; i++){
      Serial.print ("Posicion: ");
      Serial.print (i);
      Serial.print (" = ");
      leer(dir);
      dir = dir + B1;
    }
    Serial.println("Operacion realizada con exito.");
    Serial.print("\n");
  }
}

void leer(byte dir){
  byte buffer;
  buffer = B00000000;
  byte direccion;
  //  Envia direccion
NoACK:  start();
  direccion = dir;
  ebit1();
  ebit0();
  ebit1();  
  ebit0();
  ebit0();
  ebit0();
  ebit0();
  // Orden de lectura
  ebit0();
  if (!rbit()){      
    //  Envia direccion de lectura
    for (int i = 0; i < 8; i++){
      if ((direccion & B10000000) == B10000000){
        ebit1();
      } else {
        ebit0();
      }
      direccion = direccion * B10;
    }
  } else {
    goto NoACK;
  }
  if (!rbit()){
    start();
    ebit1();
    ebit0();
    ebit1();  
    ebit0();
    ebit0();
    ebit0();
    ebit0();
    // Orden de lectura
    ebit1();
  } else {
    goto NoACK;
  }
  if (!rbit()){
    //  Leer dato
    for (int i = 0; i < 8; i++){
      if (rbit()){
        buffer = buffer * B10;
        buffer = buffer + B1;
      } else {
        buffer = buffer * B10;
      }
    }
    ebit0();
    mystop();
    Serial.println(buffer);
  } else {
    goto NoACK;
  }   
}

void escribe (byte dir, byte num){
  byte direccion;
  byte numero;
fallo: direccion = dir;
  numero = num;
  start();
  //  Envia direccion del dispositivo
  ebit1();
  ebit0();
  ebit1();  
  ebit0();
  ebit0();
  ebit0();
  ebit0();
  // Orden de escritura
  ebit0();
  //  Envia 8 bits a 0
  if (!rbit()){
    //  Envia direccion
    for (int i = 0; i < 8; i++){
      if ((direccion & B10000000) == B10000000){
        ebit1();
      } else {
        ebit0();
      }
      direccion = direccion * B10;
    }
  } else {
    goto fallo;
  }
  if (!rbit()){
    //  Envia dato
    for (int i = 0; i < 8; i++){
      if ((numero & B10000000) == B10000000){
        ebit1();
      } else {
        ebit0();
      }
      numero = numero * B10;
    }
  } else {
    goto fallo;
}
  if (!rbit()){
    mystop(); 
  } else {
    goto fallo;
  }
}

// Señal de Start
void start(){
  digitalWrite(esc_sda, HIGH);
  digitalWrite(esc_scl, HIGH);
  while (digitalRead(lee_sda) != HIGH && digitalRead(lee_scl) != HIGH){};
  digitalWrite(esc_sda, LOW);
  digitalWrite(esc_scl, HIGH);
  digitalWrite(esc_sda, LOW);
  digitalWrite(esc_scl, LOW);
}

// Señal de Stop
void mystop(){
  digitalWrite(esc_sda, LOW);
  digitalWrite(esc_scl, LOW);
  digitalWrite(esc_sda, LOW);
  digitalWrite(esc_scl, HIGH);
  digitalWrite(esc_sda, HIGH);
  digitalWrite(esc_scl, HIGH);
  digitalWrite(esc_sda, HIGH);
  digitalWrite(esc_scl, HIGH);
}

// Señal de Enviar un bit a 1
void ebit1(){
  digitalWrite(esc_sda, HIGH);
  digitalWrite(esc_scl, LOW);
  digitalWrite(esc_sda, HIGH);
  digitalWrite(esc_scl, HIGH);
  digitalWrite(esc_sda, HIGH);
  digitalWrite(esc_scl, HIGH);
  digitalWrite(esc_sda, HIGH);
  digitalWrite(esc_scl, LOW);
}

// Señales de Enviar un bit a 0 y Enviar el ACK
void ebit0(){
  digitalWrite(esc_sda, LOW);
  digitalWrite(esc_scl, LOW);
  digitalWrite(esc_sda, LOW);
  digitalWrite(esc_scl, HIGH);
  digitalWrite(esc_sda, LOW);
  digitalWrite(esc_scl, HIGH);
  digitalWrite(esc_sda, LOW);
  digitalWrite(esc_scl, LOW);
}

// Señal Enviar un bit a 1 y Recibir el ACK
boolean rbit(){
  boolean b;
  digitalWrite(esc_sda, HIGH);
  digitalWrite(esc_scl, LOW);
  digitalWrite(esc_scl, HIGH);
  digitalWrite(esc_sda, HIGH);  
  if (digitalRead(lee_scl) == HIGH){
    if (digitalRead(lee_sda) == HIGH){
      b = true;
    } else {
      b = false;
    }
    digitalWrite(esc_sda, HIGH);
    digitalWrite(esc_scl, LOW);
  }
  return b;
}

byte pedirdir(){
  while(Serial.read() != -1);
  byte escbin = B00000000;
  int val = 0;
  int centena = -1;
  delay(20);
  // Leemos centenas
  while (centena < 0) {
    centena = Serial.read() - 48;
  }
  val = val + (centena * 100);
  int decena = -1;
  delay(20);
  // Leemos decenas
  while (decena < 0) {
    decena = Serial.read() - 48;
  }
  val = val + (decena * 10);
  int unidad = -1;
  // Leemos unidades
  while (unidad < 0) {
    unidad = Serial.read() - 48;  
  }
  val = val + unidad;
  escbin = val;
  Serial.print("El numero introducido es ");
  Serial.println(escbin);
  return escbin;
}