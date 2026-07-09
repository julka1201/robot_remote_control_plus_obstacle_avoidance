#include <Arduino.h>
#include <Servo.h>
#include <NewPing.h>
#define INFRATED_RECEIVER 12
#define ENA 6
#define IN1 2
#define IN2 4
#define IN3 7
#define IN4 8
#define ENB 5

int distance_right=0;
int distance_left=0;
Servo mojeSerwo;
NewPing mojCzujnik(A1, A0, 200);

unsigned long aktualne_sekundy_od_uruchomienia = 0;
unsigned long poprzednie_sekundy_od_uruchomienia = 0;
unsigned long kod_hex_przycisku = 0;
int poprzedni_stan = HIGH;
bool sygnal_startowy = false;
bool przerwa_po_starcie = false;
int ktory_z_kolei_bit = 0;
unsigned long milisekundy_od_jakiegokolwiek_wcisniecia_przycisku=0;
bool czy_w_ruchu=false;
int tryb=0; //0-sterowanie pilotem, 1-automat

unsigned long przycisk_up=0xB946FF00;
unsigned long przycisk_down=0xEA15FF00;
unsigned long przycisk_left=0xBB44FF00;
unsigned long przycisk_right=0xBC43FF00;
unsigned long przycisk_ok=0xBF40FF00;
unsigned long przycisk_0=0xAD52FF00;
unsigned long przycisk_1=0xE916FF00;

void setup() {
  mojeSerwo.attach(A2);
  pinMode(INFRATED_RECEIVER, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  analogWrite(ENA, 255);
  analogWrite(ENB, 255);
  mojeSerwo.write(90);
}
void jedzProsto(){
//lewe
digitalWrite(IN1, HIGH);
digitalWrite(IN2, LOW);
//prawe
digitalWrite(IN3, HIGH);
digitalWrite(IN4, LOW);
return;
}

void jedzDoTylu(){
//lewe
digitalWrite(IN1, LOW);
digitalWrite(IN2, HIGH);
//prawe
digitalWrite(IN3, LOW);
digitalWrite(IN4, HIGH);
return;
}

void skrecWLewo(){
//prawe do przodu, lewe do tyłu
//prawe
digitalWrite(IN3, HIGH);
digitalWrite(IN4, LOW);
//lewe
digitalWrite(IN1, LOW);
digitalWrite(IN2, HIGH);
return;
}

void skrecWPrawo(){
//prawe do tyłu, lewe do przodu
//prawe
digitalWrite(IN3, LOW);
digitalWrite(IN4, HIGH);
//lewe
digitalWrite(IN1, HIGH);
digitalWrite(IN2, LOW);
return;
}

void zatrzymaj(){
//lewe
digitalWrite(IN1, LOW);
digitalWrite(IN2, LOW);
//prawe
digitalWrite(IN3, LOW);
digitalWrite(IN4, LOW);
return;
}

unsigned long roznica_czasow_stanow(unsigned long aktualne_sekundy_od_uruchomienia, unsigned long poprzednie_sekundy_od_uruchomienia) {
  return aktualne_sekundy_od_uruchomienia - poprzednie_sekundy_od_uruchomienia;
}

void loop() {
  if (digitalRead(INFRATED_RECEIVER) == LOW && poprzedni_stan == HIGH) {
    poprzedni_stan = LOW;
    if (sygnal_startowy) {
      aktualne_sekundy_od_uruchomienia = micros();
      unsigned long roznica = roznica_czasow_stanow(aktualne_sekundy_od_uruchomienia, poprzednie_sekundy_od_uruchomienia);
      
      if (4000 < roznica && roznica < 5000 && przerwa_po_starcie == false) {
        przerwa_po_starcie = true;
      }
      
      else if (2000 < roznica && roznica < 2500 && przerwa_po_starcie == false) {
        milisekundy_od_jakiegokolwiek_wcisniecia_przycisku=millis();
        aktualne_sekundy_od_uruchomienia = 0;
        poprzednie_sekundy_od_uruchomienia = 0;
        sygnal_startowy = false;
        ktory_z_kolei_bit = 0; 
      }
      
      else if (przerwa_po_starcie)
      {
        if (1000 < roznica) {
        // logiczne 1
        bitSet(kod_hex_przycisku, ktory_z_kolei_bit);
        }
        ktory_z_kolei_bit++;
      }
      poprzednie_sekundy_od_uruchomienia = aktualne_sekundy_od_uruchomienia;
    } 

    else 
    {
      poprzednie_sekundy_od_uruchomienia = micros();
    }
  }

  if (digitalRead(INFRATED_RECEIVER) == HIGH && poprzedni_stan == LOW) {
    aktualne_sekundy_od_uruchomienia = micros();
    unsigned long roznica = roznica_czasow_stanow(aktualne_sekundy_od_uruchomienia, poprzednie_sekundy_od_uruchomienia);
    
    if (8000 < roznica && roznica < 10000) {
      sygnal_startowy = true;

    }
    
    poprzedni_stan = HIGH;
    poprzednie_sekundy_od_uruchomienia = aktualne_sekundy_od_uruchomienia;
  }

  if (ktory_z_kolei_bit == 32) {
    if (kod_hex_przycisku == przycisk_1){
      tryb=1;
    }
    if (kod_hex_przycisku==przycisk_0){
      tryb=0;
    }
    if (tryb==0){
    if (kod_hex_przycisku == przycisk_up){
      czy_w_ruchu=true;
      jedzProsto();
    }
    if (kod_hex_przycisku == przycisk_down){
      czy_w_ruchu=true;
      jedzDoTylu();
    }
    if (kod_hex_przycisku == przycisk_left){
      czy_w_ruchu=true;
      skrecWLewo();
    }
    if (kod_hex_przycisku == przycisk_right){
      czy_w_ruchu=true;
      skrecWPrawo();
    }
    if (kod_hex_przycisku == przycisk_ok){
      czy_w_ruchu=false;
      zatrzymaj();
    }
  }
  
  milisekundy_od_jakiegokolwiek_wcisniecia_przycisku=millis();
  aktualne_sekundy_od_uruchomienia = 0;
  poprzednie_sekundy_od_uruchomienia = 0;
  kod_hex_przycisku = 0;
  sygnal_startowy = false;
  przerwa_po_starcie = false;
  ktory_z_kolei_bit = 0;
}
  unsigned long roznica_ms=millis()-milisekundy_od_jakiegokolwiek_wcisniecia_przycisku;

  if (tryb==0 && roznica_ms>300 && czy_w_ruchu){
    //Serial.println(roznica_ms);
    zatrzymaj();
    czy_w_ruchu=false;
  }


  
  if (tryb==1){
    int kat=0;
    jedzProsto();
    if (sygnal_startowy==false){
    int odl= mojCzujnik.ping_cm();
    //jak nic nie ma przed robotem to odl=0
    if (odl>0 && odl<20){
      zatrzymaj();
      for (kat; kat<180; kat++){
        mojeSerwo.write(kat);
        delay(15);
      }
      distance_right=mojCzujnik.ping_cm();
      for (kat; kat>0; kat--){
        mojeSerwo.write(kat);
        delay(15);  
      }
      distance_left=mojCzujnik.ping_cm();
      //powrot
      mojeSerwo.write(90);
        delay(300);  
      
      if (distance_right>=distance_left){
        skrecWPrawo();
        delay(200);
      }
      else {
        skrecWLewo();
        delay(200);
      }
    }
    }
  }
}



