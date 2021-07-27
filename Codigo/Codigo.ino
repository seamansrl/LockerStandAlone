// PROYECTO DESARROLLADO POR SEAMAN SRL
// EL PRESENTE CODIGO SE ENTREGA SIN GARANTIA ALGUNA

#include <Keypad.h>
#include "EEPROM.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

typedef struct
{
  int Columna;
  int Fila;  
} PositionMatrix;

PositionMatrix LokerMatrix[] = 
{ 
  {22,23},
  {24,23},
  {26,23},
  {28,23},
  {30,23},
  {22,25},
  {24,25},
  {26,25},
  {28,25},
  {30,25},
  {22,27},
  {24,27},
  {26,27},
  {28,27},
  {30,27},
  {22,29},
  {24,29},
  {26,29},
  {28,29},
  {30,29},
  {22,31},
  {24,31},
  {26,31},
  {28,31},
  {30,31},
  {32,31},
  {22,33},
  {24,33},
  {26,33},
  {28,33},
  {30,33},
  {32,33},
  {22,35},
  {24,35},
  {26,35},
  {28,35},
  {30,35},
  {32,35},
  {22,37},
  {24,37},
  {26,37},
  {28,37},
  {30,37},
  {32,37}
};

const byte rowsCount = 4;
const byte columsCount = 4;
 
char keys[rowsCount][columsCount] = {
   { '1','2','3', 'A' },
   { '4','5','6', 'B' },
   { '7','8','9', 'C' },
   { '*','0','#', 'D' }
};

const byte rowPins[rowsCount] = { 11, 10, 9, 8 };
const byte columnPins[columsCount] = { 7, 6, 5, 4 };
 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, rowsCount, columsCount);

String AllKeys = "";

void SetPassword(int locker, int password)
{
  int addr = 0;

  addr = locker * 4;
 
  EEPROM.put(addr, password);  
}

int GetPassword(int locker)
{
  int password;
  int addr = 0;

  addr = locker * 4;

  EEPROM.get( addr, password );
  
  if (password != NULL)
    return password;
  else
    return -1;
}

bool GetMagnetics()
{
  bool Value = false;

  EEPROM.get(200, Value); 

  return Value;
}

void setMagneics(bool Value = false)
{
  EEPROM.put(200, Value);  
}

int GetFreeLooker()
{
  int locker = random(43);
  int password;
  
  for (int Loops = 0; Loops <= 43;Loops++)
  {
    if (locker > 43)
      locker = 0;
    else
      locker++;

    if (GetPassword(locker) == -1)
      return locker;
  }

  return -1;
}

void ResetEEPROM()
{
  for (int a = 0; a < 1024; a++)
  {
      SetPassword(a, NULL);
  }
}

void Beep(int Ciclos = 1, int tempo = 50)
{
  for (int Ciclo = 0; Ciclo < Ciclos; Ciclo++)
  {
    digitalWrite(3, LOW); 
    delay(tempo);  
    digitalWrite(3, HIGH); 
    delay(tempo);   
  }  
}

int LockersDisponibles()
{
  int Count = 0;

  for (int locker = 0; locker <= 43;locker++)
  {
    if (GetPassword(locker) == -1)
      Count = Count + 1;
  }

  return Count;
}

void MensajeCancel()
{
  // CIERRO CERRADURAS TODAS
  CerrarTodo();
  
  MensajeMarco();
  
  lcd.setCursor(0, 1);
  lcd.print("Operacion Cancelada");  

  Beep(2,100);
}

void MensajeTimeOut()
{
  // CIERRO CERRADURAS TODAS
  CerrarTodo();
                  
  MensajeMarco();
  
  lcd.setCursor(0, 1);
  lcd.print("!! TIEMPO AGOTADO !!"); 

  Beep(2,100);
}

void MensajeDeInicio()
{
  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print("A: GUARDAR"); 
  
  lcd.setCursor(0, 1);
  lcd.print("B: RETIRAR"); 
  
  lcd.setCursor(0, 2);
  lcd.print("C: CANCELAR"); 
  
  lcd.setCursor(0, 3);
  lcd.print("D: DISPONIBILIDAD");   
}

void MensajeSinEspacios()
{
  MensajeMarco();
  
  lcd.setCursor(0, 1);
  lcd.print("No hay lockers"); 

  lcd.setCursor(0, 2);
  lcd.print("disponibles"); 

  Beep(2,100);  
}

void CerrarTodo()
{
  for (int Pin = 22; Pin < 44;Pin++)
    digitalWrite(Pin, HIGH); 
}

void AbrirLocker(int locker = -1)
{
  if (locker >= 0 && locker <= 43)
  {
      digitalWrite(LokerMatrix[locker].Fila, LOW); 
      delay(10);
      digitalWrite(LokerMatrix[locker].Columna, LOW); 
      delay(10);
  }  
}

void MensajeMarco()
{
  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print("------ LOCKER ------"); 

  lcd.setCursor(0, 3);
  lcd.print("--------------------");   
}

void setup() 
{
  Serial.begin(9600);
  
  lcd.begin(20, 4);
  lcd.backlight();

  // LECTURA DE PUERTAS
  pinMode(2, INPUT);

  // ZUMBADOR
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH); 

  // RELES
  for (int Pin = 22; Pin < 44;Pin++)
  {
    pinMode(Pin, OUTPUT);
    digitalWrite(Pin, HIGH); 
  }
  
  MensajeDeInicio();
}
 
void loop() 
{
   char key = keypad.getKey();

   if (Serial.available())
   {
      String Command = Serial.readString();
      Command.trim();
      Command = Command.substring(0);
      
      Serial.println("> " + Command);
      if (Command == "resete")
      {
        Serial.print("Borrando...");
        ResetEEPROM();

        Serial.println(" OK");

        return;
      }

      Serial.println("comando no reconocido");
   }
    
   if (key) 
   {
      if (AllKeys.length() > 20)
        AllKeys = "";
      else
        AllKeys = AllKeys + key;
      
      Beep();

      // LOS CODIGOS DE SERVICIO PERMITEN AL ADMINISTRADOR DE LOCKER TENER ACCESO A FUNCIONES AVANZADAS EN CASO DE 
      // FALLA O MANTENIMIENTO. ESTAS CLAVES NO DEBEN SER PUBLICAS.

      // BEEP DE ENTRADA A MODO DE SERVICIO
      if (AllKeys == "*000")
      {
        delay(600);
        Beep(2,100);
      }

      // ACTIVA O DESACTIVA LOS SENSORES MAGNETICOS, ESTO PERMITE USARLOS O NO SEGUN SI SE QUIERE INSTALARLOS O NO.
      if (AllKeys == "*00025500##")
      {
          MensajeMarco();
          
          lcd.setCursor(0, 1);
          lcd.print("Ajustando sensores"); 
          Beep(1,200);
          
          setMagneics(!GetMagnetics());


          if (GetMagnetics())
          {
            lcd.setCursor(0, 2);
            lcd.print("Activos"); 
          }
          else
          {
            lcd.setCursor(0, 2);
            lcd.print("desactivos"); 
          }

          Beep(2,200);
          delay(2000);
          MensajeDeInicio();

          AllKeys = "";
          
          return;
      }

      // BORRA TODAS LAS CLAVES Y RESETEA EL LOCKER
      if (AllKeys == "*00075569##")
      {
          MensajeMarco();
          
          lcd.setCursor(0, 1);
          lcd.print("Borrando..."); 
          Serial.print("Borrando...");
          Beep(1,200);
          
          ResetEEPROM();

          lcd.setCursor(0, 2);
          lcd.print("OK"); 
          Serial.println(" OK");
          
          Beep(2,200);
          delay(2000);
          MensajeDeInicio();
          
          return;
      }

      // PERMITE REESCRIBIR UNA CLAVE EN CASO QUE EL USUARIO OLVIDARA LA INGRESADA
      if (AllKeys == "*00023434##")
      {
          MensajeMarco();
        
          lcd.setCursor(0, 1);
          lcd.print("Ingrese"); 
          
          lcd.setCursor(0, 2);
          lcd.print("numero de loker");
          
          int locker = Locker();

          if (locker != -1)
          {
            MensajeMarco();
          
            lcd.setCursor(0, 1);
            lcd.print("Ingrese una clave");
    
            lcd.setCursor(0, 2);
            lcd.print("de 4 digitos");
              
            int Clave1 = Password();
  
            if (Clave1 != -1)
            {
              MensajeMarco();
    
              lcd.setCursor(0, 1);
              lcd.print("Repita la clave"); 
    
              Beep(2, 50);
              
              int Clave2 = Password();
    
              if (Clave2 != -1)
              { 
                if (Clave1 == Clave2)
                {
                  SetPassword(locker, Clave1);  
                
                  MensajeMarco();
                  
                  lcd.setCursor(0, 1);
                  lcd.print("Clave cambiada"); 
  
                  Beep(2, 50);
                }
              }
            }
            
            delay(2000);
            MensajeDeInicio();

            AllKeys = "";
            
            return;
          }
      }

      // TECLAS DE FUNCION
      
      if ((String)key == "A")
      {
        AllKeys = "";
        
        int locker = GetFreeLooker();

        if (locker == -1)
        {
          MensajeSinEspacios();
          
          delay(5000);
          MensajeDeInicio();
          
          return;
        }
              
        MensajeMarco();
        
        lcd.setCursor(0, 1);
        lcd.print("Ingrese una clave");

        lcd.setCursor(0, 2);
        lcd.print("de 4 digitos");
          
        int Clave1 = Password();

        if (Clave1 != -1)
        {
          MensajeMarco();

          lcd.setCursor(0, 1);
          lcd.print("Repita la clave"); 

          Beep(2, 50);
          
          int Clave2 = Password();

          if (Clave2 != -1)
          { 
            if (Clave1 == Clave2)
            {
              MensajeMarco();
              
              lcd.setCursor(0, 1);
              lcd.print("Use el locker"); 

              lcd.setCursor(0, 2);
              lcd.print("numero " + (String)locker); 


              // ABRO CERRADURA INDIVIDUAL
              AbrirLocker(locker);
              
              // LOOP DE ESPERA A QUE SE ABRA EL LOCKER
              for (int timer_1 = 0; timer_1 < 100; timer_1++)
              {
                key = keypad.getKey();
                        
                 if (key) 
                 {
                    Beep();
                    if ((String)key == "C")
                    {
                      MensajeCancel();
                      delay(4000);
                      MensajeDeInicio();
                      
                      return;
                    }
                 }
        
                // SI SE ABRE LA PUERTA
                if (digitalRead(2) == LOW || (GetMagnetics() == false && timer_1 > 50))
                {       
                  // CIERRO CERRADURAS TODAS
                  CerrarTodo();

                  Beep(1, 20);
                  
                  MensajeMarco();
                  
                  lcd.setCursor(0, 1);
                  lcd.print("Guarde sus cosas "); 
    
                  lcd.setCursor(0, 2);
                  lcd.print("y cierre la puerta"); 
              
                  for (int timer_2 = 0; timer_2 < 1000; timer_2++)
                  {
                    key = keypad.getKey();
                        
                     if (key) 
                     {
                        Beep();
                        if ((String)key == "C")
                        {
                          MensajeCancel();
                          delay(4000);
                          MensajeDeInicio();
                          
                          return;
                        }
                     }
                   
                    if (digitalRead(2) == HIGH || (GetMagnetics() == false && timer_2 > 50))
                    {
                      if (GetMagnetics() == false)
                      {
                        MensajeMarco();
                      
                        lcd.setCursor(0, 1);
                        lcd.print("A: ASEGURAR"); 
          
                        lcd.setCursor(0, 2);
                        lcd.print("C: CANCELAR"); 
                      
                        for (int timer_1 = 0; timer_1 < 600; timer_1++)
                        {
                           key = keypad.getKey();
                                  
                           if (key) 
                           {
                              Beep();
                              if ((String)key == "C")
                              {
                                MensajeDeInicio();
                          
                                return;
                              }
                                
                              if ((String)key == "A")
                                break;
                           } 

                           delay(100);
                        }
                      }

                      SetPassword(locker, Clave1);
                      
                      MensajeMarco();
                      
                      lcd.setCursor(0, 1);
                      lcd.print("Loker " + (String)locker); 
        
                      lcd.setCursor(0, 2);
                      lcd.print("asegurado"); 

                      delay(5000);
                      
                      Beep(2, 50);
                      MensajeDeInicio();
                  
                      return;
                    }

                    if (timer_2 < 300)
                    {
                      delay(100);  
                    }
                    else if (timer_2 >= 300 && timer_2 < 400)
                    {
                      Beep(1, 60);
                      delay(100);
                    } 
                    else if (timer_2 >= 400 && timer_2 < 600)
                    {
                      Beep(2, 30);
                      delay(100); 
                    }
                    else if (timer_2 >= 600 && timer_2 < 800)
                    {
                      Beep(3, 20); 
                      delay(100); 
                    } 
                    else if (timer_2 >= 800 && timer_2 < 1000)
                    {
                      MensajeMarco();
                      
                      lcd.setCursor(0, 1);
                      lcd.print("ERROR EN PUERTA"); 
        
                      lcd.setCursor(0, 2);
                      lcd.print("LOCKER ABIERTO"); 
                      
                      while (digitalRead(2) == LOW || (GetMagnetics() == false && timer_2 > 50))
                      {
                         key = keypad.getKey();
                        
                         if (key) 
                         {
                            Beep();
                            
                            if ((String)key == "C")
                            {
                              SetPassword(locker, NULL);
                              
                              MensajeCancel();
                              delay(4000);
                              MensajeDeInicio();
                              
                              return;
                            }
                         }
                         delay(100);
                      }
                    }
                  }
                  
                  MensajeTimeOut();
                  delay(5000);
                  MensajeDeInicio();

                  return;
                }
                
                delay(100);        
              }

              MensajeTimeOut();
              delay(5000);
              MensajeDeInicio();

              return;
            }
            else
            {
              MensajeMarco();
              
              lcd.setCursor(0, 1);
              lcd.print("Las claves"); 

              lcd.setCursor(0, 2);
              lcd.print("no coinciden"); 

              Beep(1,200);
              delay(4000);
              MensajeDeInicio();
            }
          }
        }
      }
   
      if ((String)key == "B")
      {
        AllKeys = "";
        
        MensajeMarco();
        
        lcd.setCursor(0, 1);
        lcd.print("Ingrese"); 
        
        lcd.setCursor(0, 2);
        lcd.print("numero de loker");
          
        int locker = Locker();

        if (locker != -1)
        {
            int Clave0 = GetPassword(locker);  
        
            if (Clave0 != -1)
            {
              MensajeMarco();
  
              lcd.setCursor(0, 1);
              lcd.print("Ingrese su clave");
      
              lcd.setCursor(0, 2);
              lcd.print("de 4 digitos");
                
              int Clave1 = Password();
      
              if (Clave1 != -1)
              {
                if (Clave1 == Clave0)
                {
                  // ABRO CERRADURA INDIVIDUAL
                  AbrirLocker(locker);

                  MensajeMarco();
                      
                  lcd.setCursor(0, 1);
                  lcd.print("Abra la puerta"); 
    
                  lcd.setCursor(0, 2);
                  lcd.print("y retire sus cosas"); 
                      
                  // LOOP DE ESPERA A QUE SE ABRA EL LOCKER
                  for (int timer_1 = 0; timer_1 < 100; timer_1++)
                  {
                    key = keypad.getKey();
                            
                     if (key) 
                     {
                        Beep();
                        if ((String)key == "C")
                        {
                          MensajeCancel();
                          delay(4000);
                          MensajeDeInicio();
                          
                          return;
                        }
                     }
            
                    // SI SE ABRE LA PUERTA
                    if (digitalRead(2) == LOW || (GetMagnetics() == false && timer_1 > 50))
                    {       
                      // CIERRO CERRADURAS TODAS
                      CerrarTodo();
                      
                      Beep(1, 20);
                      
                      MensajeMarco();
                      
                      lcd.setCursor(0, 1);
                      lcd.print("Cierre la puerta "); 
        
                      lcd.setCursor(0, 2);
                      lcd.print("para terminar"); 
                  
                      for (int timer_2 = 0; timer_2 < 1000; timer_2++)
                      {
                        key = keypad.getKey();
                            
                         if (key) 
                         {
                            Beep();
                            if ((String)key == "C")
                            {
                              MensajeCancel();
                              delay(4000);
                              MensajeDeInicio();
                              
                              return;
                            }
                         }
                       
                        if (digitalRead(2) == HIGH || (GetMagnetics() == false && timer_2 > 50))
                        {
                          if (GetMagnetics() == false)
                          {
                            MensajeMarco();
                          
                            lcd.setCursor(0, 1);
                            lcd.print("A: LIBERAR"); 
              
                            lcd.setCursor(0, 2);
                            lcd.print("C: CANCELAR"); 
                          
                            for (int timer_1 = 0; timer_1 < 600; timer_1++)
                            {
                               key = keypad.getKey();
                                      
                               if (key) 
                               {
                                  Beep();
                                  if ((String)key == "C")
                                  {
                                    MensajeDeInicio();
                              
                                    return;
                                  }
                                    
                                  if ((String)key == "A")
                                    break;
                               } 
    
                               delay(100);
                            }
                          }

                          SetPassword(locker, NULL);
                          
                          MensajeMarco();
                          
                          lcd.setCursor(0, 1);
                          lcd.print("Loker " + (String)locker); 
            
                          lcd.setCursor(0, 2);
                          lcd.print("liberado"); 
    
                          delay(5000);
                          
                          Beep(2, 50);
                          MensajeDeInicio();
                      
                          return;
                        }
    
                        if (timer_2 < 300)
                        {
                          delay(100);  
                        }
                        else if (timer_2 >= 300 && timer_2 < 400)
                        {
                          Beep(1, 60);
                          delay(100);
                        } 
                        else if (timer_2 >= 400 && timer_2 < 600)
                        {
                          Beep(2, 30);
                          delay(100); 
                        }
                        else if (timer_2 >= 600 && timer_2 < 800)
                        {
                          Beep(3, 20); 
                          delay(100); 
                        } 
                        else if (timer_2 >= 800 && timer_2 < 1000)
                        {
                          MensajeMarco();
                          
                          lcd.setCursor(0, 1);
                          lcd.print("ERROR EN PUERTA"); 
            
                          lcd.setCursor(0, 2);
                          lcd.print("LOCKER ABIERTO"); 
                          
                          while (digitalRead(2) == LOW || (GetMagnetics() == false && timer_2 > 50))
                          {
                             key = keypad.getKey();
                            
                             if (key) 
                             {
                                Beep();
                                
                                if ((String)key == "C")
                                {
                                  SetPassword(locker, NULL);
                                  
                                  MensajeCancel();
                                  delay(4000);
                                  MensajeDeInicio();
                                  
                                  return;
                                }
                             }
                             delay(100);
                          }
                        }
                      }
                      
                      MensajeTimeOut();
                      delay(5000);
                      MensajeDeInicio();
    
                      return;
                    }
                    
                    delay(100);        
                  }
    
                  MensajeTimeOut();
                  delay(5000);
                  MensajeDeInicio();
    
                  return;  
                }
                else
                {
                  MensajeMarco();
                      
                  lcd.setCursor(0, 1);
                  lcd.print("Clave erronea"); 

                  delay(4000);
                  MensajeDeInicio();
    
                  return;  
                }
              }
            }
            else
            {
              MensajeMarco();
            
              lcd.setCursor(0, 1);
              lcd.print("Loker vacio"); 
  
              Beep(2,100);
              delay(4000);
  
            
              MensajeDeInicio();
            }

            Beep(2,50);
          }
      }

      if ((String)key == "C")
      {
          AllKeys = "";

          MensajeMarco();
                      
          lcd.setCursor(0, 1);
          lcd.print("Nada para cencelar"); 
  
          delay(2000);
          MensajeDeInicio();
  
          return; 
      }
      
      if ((String)key == "D")
      {
         AllKeys = "";
         
        MensajeMarco();
        
        lcd.setCursor(0, 1);
        lcd.print("Hay un total de"); 

        lcd.setCursor(0, 2);
        lcd.print((String)LockersDisponibles() + " lockers libres."); 

        
        delay(5000);
        MensajeDeInicio();

        return; 
      }
   }

    delay(1);
}

int Password()
{
  String Clave = "";
  
  for (int timer_1 = 0; timer_1 < 100; timer_1++)
  {
     char key = keypad.getKey();

     if (key)
      Beep();

     if (key && (String)key != "A" && (String)key != "B" && (String)key != "D" && (String)key != "*" && (String)key != "#")
     {
        if ((String)key == "C")
        {
          MensajeCancel();
          delay(4000);
          MensajeDeInicio();
          
          return -1;
        }

        MensajeMarco();
        
        lcd.setCursor(0, 1);
        lcd.print("CLAVE: *"); 
          
        Clave = (String)key;

        for (int timer_1 = 0; timer_1 < 100; timer_1++)
        {
           char key = keypad.getKey();

           if (key)
            Beep();
     
           if (key && (String)key != "A" && (String)key != "B" && (String)key != "D" && (String)key != "*" && (String)key != "#")
           {
              if ((String)key == "C")
              {
                MensajeCancel();
                delay(4000);
                MensajeDeInicio();
              
                return -1;
              }

              MensajeMarco();
              
              lcd.setCursor(0, 1);
              lcd.print("CLAVE: **"); 
          
              Clave = Clave + (String)key;
      
              for (int timer_1 = 0; timer_1 < 100; timer_1++)
              {
                 char key = keypad.getKey();

                 if (key)
                  Beep();
     
                 if (key && (String)key != "A" && (String)key != "B" && (String)key != "D" && (String)key != "*" && (String)key != "#")
                 {
                    if ((String)key == "C")
                    {
                      MensajeCancel();
                      delay(4000);
                      MensajeDeInicio();
              
                      return -1;
                    }

                    MensajeMarco();
                    
                    lcd.setCursor(0, 1);
                    lcd.print("CLAVE: ***"); 
          
                    Clave = Clave + (String)key;

                    for (int timer_1 = 0; timer_1 < 100; timer_1++)
                    {
                       char key = keypad.getKey();

                       if (key)
                        Beep();
     
                       if (key && (String)key != "A" && (String)key != "B" && (String)key != "D" && (String)key != "*" && (String)key != "#")
                       {
                          if ((String)key == "C")
                          {
                            MensajeCancel(); 
                            delay(4000);
                            MensajeDeInicio();
              
                            return -1;
                          }

                          MensajeMarco();
                          
                          lcd.setCursor(0, 1);
                          lcd.print("CLAVE: ****"); 
          
                          Clave = Clave + (String)key;
                    
                          return Clave.toInt();
                       }  
                  
                       delay(100);
                     }

                      MensajeTimeOut();
                      delay(4000);
                      MensajeDeInicio();
              
                     return -1;
                 }  
            
                 delay(100);
               }

                MensajeTimeOut();
                delay(4000);
                MensajeDeInicio();
              
               return -1;
           }  
      
           delay(100);
         }

          MensajeTimeOut();
          delay(4000);
          MensajeDeInicio();
              
          return -1;
     }  

     delay(100);
   }

    MensajeTimeOut(); 
    delay(4000);
    MensajeDeInicio();
    
   return -1;
}

int Locker()
{
  String Posicion = "";
  
  for (int timer_1 = 0; timer_1 < 100; timer_1++)
  {
     char key = keypad.getKey();

     if (key)
      Beep();
     
     if (key && (String)key != "A" && (String)key != "B" && (String)key != "D" && (String)key != "*" && (String)key != "#")
     {
        if ((String)key == "C")
        {
          MensajeCancel();
          delay(4000);
          MensajeDeInicio();
              
          return -1;
        }

        Posicion = (String)key;

        MensajeMarco();
        
        lcd.setCursor(0, 1);
        lcd.print("LOCKER NRO: " + Posicion); 
        
        for (int timer_1 = 0; timer_1 < 100; timer_1++)
        {
           char key = keypad.getKey();

           if (key)
            Beep();
     
           if (key && (String)key != "A" && (String)key != "B" && (String)key != "D" && (String)key != "*" && (String)key != "#")
           {
              if ((String)key == "C")
              {
                MensajeCancel();
                delay(4000);
                MensajeDeInicio();
              
                return -1;
              }

              Posicion = Posicion + (String)key;

              MensajeMarco();
              
              lcd.setCursor(0, 1);
              lcd.print("LOCKER NRO: " + Posicion); 

              delay(600);

              if (Posicion.toInt() <= 43)
                return Posicion.toInt();
              else
              {
                MensajeMarco();
            
                lcd.setCursor(0, 1);
                lcd.print("Numero de loker"); 

                lcd.setCursor(0, 2);
                lcd.print("invalido"); 
                
                Beep(2,100);
                delay(4000);

                MensajeDeInicio();
                
                return -1;
              }
           }  
      
           delay(100);
         }

        MensajeTimeOut();
        delay(4000);
        MensajeDeInicio();
              
        return -1;
     }  

     delay(100);
   }

    MensajeTimeOut();
    delay(4000);
    MensajeDeInicio();
              
   return -1;
}
