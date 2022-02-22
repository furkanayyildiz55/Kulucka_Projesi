#include <LiquidCrystal.h>  //LCD kütüphanesi
#include <EEPROM.h>
#include <dht11.h> // dht11 kütüphanesini kodlarımıza dahil ediyoruz.
#include <time.h>

//*****
#include <DS3232RTC.h>
DS3232RTC myRTC(false);


//lcd pin tanımlaması
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

int DHT11_pin=13;
dht11 DHT11_sensor;

 #define onay_butonu 2 
 #define artirma_butonu 3
 #define azaltma_butonu 0
 byte yok[8]=
{
   B00100,
   B01110,
   B11111,
   B01110,
   B01110,
   B01110,
   B01110,
   B01110,
};
byte aok[8]=
{
   B01110,
   B01110,
   B01110,
   B01110,
   B01110,
   B11111,
   B01110,
   B00100,
};


//MOTOR
void motor_start(){
  digitalWrite(4,0);
}
void motor_stop(){
  digitalWrite(4,1);
//NEM
}
void nem_start(){
  digitalWrite(5,0);
}
void nem_stop(){
  digitalWrite(5,1);
 
}
//ISITICI
void isitici_stop(){
  digitalWrite(6,1);
}
void isitici_start(){
  digitalWrite(6,0);
}




time_t tmConvert(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss)
{
  tmElements_t tmSet;
  tmSet.Year = YYYY - 1970;
  tmSet.Month = MM;
  tmSet.Day = DD;
  tmSet.Hour = hh;
  tmSet.Minute = mm;
  tmSet.Second = ss;
  return makeTime(tmSet);         //convert to time_t
}

//saat menüsü için ve lcd ye saat yazdırmak için değişken tanımlamaları
int saatmenu=0, yil=2020;
byte saat,dakika,saniye,ay,gun,vericekme_kontrol=1;

void setup() {
  
//BUTTON TANIMLAMALARI
pinMode(onay_butonu,INPUT_PULLUP);
pinMode(artirma_butonu,INPUT_PULLUP);
pinMode(azaltma_butonu,INPUT_PULLUP);

pinMode(4,OUTPUT); //motor
pinMode(5,OUTPUT); //nem
pinMode(6,OUTPUT);  //isitici

pinMode(A3,OUTPUT); //fan

digitalWrite(4,1);//MOTOR 
digitalWrite(5,1);//NEM
digitalWrite(A3,1);//FAN
digitalWrite(6,1);//ISITICI


Serial.begin(9600);
Serial.print("asdsad");
lcd.begin(16,2); 
lcd.setCursor(0, 0); // İmleci sütun 0'a, satır 2'ye ayarlıyoruz
lcd.print("Furkan AYYILDIZ");
lcd.setCursor(0, 1); // İmleci sütun 0'a, satır 2'ye ayarlıyoruz
lcd.print("16008119035");
delay(200);
//lcd.clear();

lcd.clear();

myRTC.begin(); //myRTC başlatılıyor

//setTime(12, 55, 80, 26, 1, 2020);   //ilk açılış sistem saati ayarlaması
//RTC.set(now()); 
//RTC.set(tmConvert( 2020,  12,  30,  13,  55,  30)); //default saat veriyoruz

time_t myTime;
myTime = RTC.get();


saat=hour(myTime);
dakika=minute(myTime);
saniye=second(myTime);

yil=year(myTime);
ay=month(myTime);
gun=day(myTime);


}//SETUP BİTİŞ
bool ilkAcilis=false;
bool ilkCevrim=false;

int KalanGun=0;
int kulucka_suresi=21,cikis_suresi=2;
float gelisme_sicaklik=37.6,cikis_sicaklik=37.2;
float gelisme_nem=50,cikis_nem=70;
String yumurta_turu="Tavuk";

float anlik_nem,anlik_sicaklik;
float nem_degeri=60,sicaklik_degeri=30;
int menu=0,yumurta=0,gerisayac=0,deger,buton_suresi=0;

int dondurme_degeri=45;

int kulucka_artma=0,dondurme_artma=0;
float sicaklik_artma=0,nem_artma=0,isi_artma=0;

time_t myTime;
time_t BitisZamani;
time_t CevirmeZamani; 


time_t DakikaEkle(int eklenecekDakika , time_t zaman){
  int saniyeCevrim= eklenecekDakika * 60;
  zaman=zaman+saniyeCevrim;
  return zaman;
  }


void yumurta_turu_degis(int kulucka_suresi1 ,int cikis_suresi1,float gelisme_sicaklik1 , float cikis_sicaklik1,float gelisme_nem1,float cikis_nem1, String yumurta_turu1   ){
 // kulucka_suresi ,cikis_suresi,gelisme_sicaklik,cikis_sicaklik,gelisme_nem,cikis_nem,yumurta_turu 

    kulucka_suresi=kulucka_suresi1;
    cikis_suresi=cikis_suresi1;
    gelisme_sicaklik=gelisme_sicaklik1;
    cikis_sicaklik=cikis_sicaklik1;
    gelisme_nem=gelisme_nem1;
    cikis_nem=cikis_nem1;
    yumurta_turu=yumurta_turu1;
  }
////////////////////////////////////////////////////
//İKİ TARİH ARASI GÜN FARKINI VERİR
int dateDiff(int year1, int mon1, int day1, int year2, int mon2, int day2)
{
    int ref,dd1,dd2,i;
    ref = year1;
    if(year2<year1)
    ref = year2;
    dd1=0;
    dd1=dater(mon1);
    for(i=ref;i<year1;i++)
    {
        if(i%4==0)
        dd1+=1;
    }
    dd1=dd1+day1+(year1-ref)*365;
    dd2=0;
    for(i=ref;i<year2;i++)
    {
        if(i%4==0)
        dd2+=1;
    }
    dd2=dater(mon2)+dd2+day2+((year2-ref)*365);
    return dd2-dd1;
}
//DATEDİFF FONK ÇALIŞABİLMESİ İÇİN
int dater(int x)
{ const int dr[]= { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  return dr[x-1];
}
//TARİHE GÜN DEGER EKLİYOR
time_t zaman_ekle(time_t baslangicZamani , int eklenecekGun){
    struct tm* tm = localtime(&baslangicZamani);
    tm->tm_mday +=eklenecekGun ;
    time_t next = mktime(tm);
    return next;
  }




///////////////////////////////////////////////////////////////////////
void yonlendirme(int gidilecekMenu , int geriGelinecekMenu ){
  deger = digitalRead(onay_butonu);
  while (deger == LOW)
  {
    delay(100);
    gerisayac++; 
    deger = digitalRead(onay_butonu); 
  }
  if(gerisayac >0 && gerisayac <= 4){//kısa basma durumu
     delay(100);
     lcd.clear();
     //kısa basmada gidilecek menü
     gerisayac=0;
     menu = gidilecekMenu;
  }
  else if(gerisayac >=5 ){
     //uzun basma durumunda geri geinecek menü
     lcd.clear();
     lcd.print("GERİ");
     delay(100); 
     gerisayac=0;
     menu=geriGelinecekMenu;
             } 
  }

void  saatYazdir(){//lcd ye saat değerlerini yazdırmak için kullanılan fonk
  
lcd.setCursor(0, 0);
lcd.print("Saat:");
lcd.setCursor(6, 0);
lcd.print(saat);
lcd.setCursor(9, 0);
lcd.print(":");
lcd.setCursor(10, 0);
lcd.print(dakika);
lcd.setCursor(12, 0);
lcd.print(":");
lcd.setCursor(13, 0);
lcd.print(saniye);

lcd.setCursor(0, 1);
lcd.print("Tarh:");
lcd.setCursor(5, 1);
lcd.print(yil);
lcd.setCursor(9, 1);
lcd.print("/");
lcd.setCursor(10, 1);
lcd.print(ay);
lcd.setCursor(12, 1);
lcd.print("/");
lcd.setCursor(13, 1);
lcd.print(gun);
}

void Kontrol(){

  if(KalanGun>kulucka_suresi-cikis_suresi){
    //GELİSME EVRESİNDE
     //NEM KONTROL
    if(anlik_nem<gelisme_nem){
      nem_start();
    }
    else{
      nem_stop();
    }

    //SICAKLIK KONTROL
    if(anlik_sicaklik<gelisme_sicaklik){
      isitici_start();
    }
    else{
    isitici_stop();
    }
    
    }
    else{
       //ÇIKIŞ EVRESİNDE
       //NEM KONTROL
     if(anlik_nem<cikis_nem){
     nem_start();
    }
    else{
      nem_stop();
    }

    //SICAKLIK KONTROL
    if(anlik_sicaklik<gelisme_sicaklik){
      isitici_start();
    }
    else{
    isitici_stop();
    }
    
    }


    //MOTOR KONTROL
    if(myTime>CevirmeZamani){
      motor_start();
      lcd.clear();
      lcd.print("CEVRILIYOR");
      delay(200);
      //motor çevrildi tekarar yeni döngü başlatılıyor
      CevirmeZamani=DakikaEkle( dondurme_degeri , myTime);
      
      }
      else{
        motor_stop();
        }

      }

void loop() {//LOOP


//DHT11 DEN NEM VE  SICAKLIK ÇEKİLİYOR
int chk = DHT11_sensor.read(DHT11_pin);
anlik_nem=(float)DHT11_sensor.humidity;
anlik_sicaklik=(float)DHT11_sensor.temperature;

//SAAT VERİLERİ ÇEKİLİYOR
if(vericekme_kontrol==1){
myTime = RTC.get();
saat=hour(myTime);
dakika=minute(myTime);
saniye=second(myTime);
yil=year(myTime);
ay=month(myTime);
gun=day(myTime);
}


if(ilkCevrim==false){//İLK ÇEVRİM DAKİKASI EKLENİYOR
    CevirmeZamani=DakikaEkle( dondurme_degeri , myTime);
    ilkCevrim=true;
    delay(200);
  }


//KULUÇKA BİTİMİNE KALAN GÜN HESAPLANIYOR
KalanGun= dateDiff(year(myTime),month(myTime),day(myTime), year(BitisZamani),month(BitisZamani),day(BitisZamani));


Kontrol();

if(menu==0){
  lcd.setCursor(0,0);
  lcd.print(" MAKINA          ");
  lcd.setCursor(0,1);
  lcd.print(" BASLATILIYOR.... ");
  delay(200);
  lcd.clear();
  
  if(ilkAcilis==false){
  menu=9;
  ilkAcilis=true;
  }
  else{
      menu=1;
    }



    
  }//menu0
else if(menu==1){//ANA EKRAN
    
    
    
    char saatchr[17];
    sprintf(saatchr, " %02d/%02d-%02d:%02d:%02d ",gun,ay, saat, dakika, saniye);

    if(saniye%4<2){
    lcd.setCursor(0,0);
    lcd.print(saatchr);
    lcd.setCursor(0,1);
    lcd.print(" KALAN :");
    lcd.print(KalanGun);
    lcd.print(" GUN");
    
      if(digitalRead(onay_butonu)==LOW){
          menu=2;
          lcd.clear();
          }
      if(digitalRead(artirma_butonu)==LOW){
          menu=10;
          lcd.clear();
          }
      if(digitalRead(azaltma_butonu)==LOW){
          menu=11;
          lcd.clear();
          }        
      }
    else{
      
      lcd.setCursor(0,0);
      lcd.print(" SICAKLIK : ");
      lcd.setCursor(12,0);
      lcd.print(anlik_sicaklik);
      lcd.setCursor(0,1);
      lcd.print(" NEM: ");
      lcd.setCursor(6,1);
      lcd.print(anlik_nem);
      lcd.setCursor(10,1);
      lcd.print("        ");
      if(digitalRead(onay_butonu)==LOW){
        menu=2;
        lcd.clear();
        }
      if(digitalRead(artirma_butonu)==LOW){
          menu=10;
          lcd.clear();
          }  
      if(digitalRead(azaltma_butonu)==LOW){
          menu=11;
          lcd.clear();
          }       
      }
      
  if(digitalRead(onay_butonu)==LOW){
    menu=2;
    lcd.clear();
    }
  }//MENU1 ANA EKRAN BİTİŞ
  
else if(menu==2){//MENU2 HAYVAN TÜRÜ SEÇME 
        lcd.setCursor(0, 0);
        lcd.print("YUMURTA TURU ");
        lcd.setCursor(0, 1);
        lcd.print("OTOMATIK       ");
        lcd.createChar(1, aok);
        lcd.setCursor(8, 1);
        lcd.write(1);
        delay(250);
        
        if (digitalRead(onay_butonu) == LOW)
        {
            yonlendirme(12,1);
        }
        if (digitalRead(azaltma_butonu) == LOW)
        {
            while (digitalRead(azaltma_butonu) == LOW)
                delay(10);
            lcd.clear();
            menu = 3;
        }
  }//MENU2 HAYVAN TÜRÜ SEÇME BİTİŞ
  
 else if(menu==3){//MENU3 MANUEL DÖNDÜRME ZAMAN AYARI

        lcd.setCursor(0, 0);
        lcd.print("DONDURME AYARI");
        lcd.setCursor(0, 1);
        lcd.print("MANUEL         ");
        lcd.createChar(1, aok);
        lcd.setCursor(8, 1);
        lcd.write(1);
        delay(100);
        if (digitalRead(onay_butonu) == LOW)
        {
          yonlendirme(13,1);
        }
        if (digitalRead(azaltma_butonu) == LOW)
        {
            while (digitalRead(azaltma_butonu) == LOW)
                delay(10);
            lcd.clear();
            menu = 4;
        }
      }//MENU3 DÖNDÜRME ZAMAN AYARI BİTİŞ
 else if(menu==4){//MENU4 NEM AYARİ
      lcd.setCursor(0, 0);
        lcd.print("NEM AYARI ");
        lcd.setCursor(0, 1);
        lcd.print("MANUEL         ");
        lcd.createChar(1, aok);
        lcd.setCursor(8, 1);
        lcd.write(1);
        delay(100);
        if (digitalRead(onay_butonu) == LOW)
        {
          yonlendirme(14,1);
        }
        if (digitalRead(azaltma_butonu) == LOW)
        {
            while (digitalRead(azaltma_butonu) == LOW)
                delay(10);
            lcd.clear();
            menu = 5;
        }
  }//MENU4 NEM AYARİ BİTİŞ
else if(menu==5){//MENU 5 SICAKLIK AYARİ
        lcd.setCursor(0, 0);
        lcd.print("SICAKLIK AYARI ");
        lcd.setCursor(0, 1);
        lcd.print("MANUEL         ");
        lcd.createChar(1, aok);
        lcd.setCursor(8, 1);
        lcd.write(1);
        delay(100);
        if (digitalRead(onay_butonu) == LOW)
        {
          yonlendirme(15,1);
        }
        if (digitalRead(azaltma_butonu) == LOW)
        {
            while (digitalRead(azaltma_butonu) == LOW)
                delay(10);
            lcd.clear();
            menu = 6;
        }
  }//MENU 5 SICAKLIK AYARİ BİTİŞ
else if(menu==6){//MENU6 MANUEL KULUÇKA SÜRESİ AYARI
              lcd.setCursor(0, 0);
        lcd.print("KULUCKA SURESI");
        lcd.setCursor(0, 1);
        lcd.print("MANUEL         ");
        lcd.createChar(1, aok);
        lcd.setCursor(8, 1);
        lcd.write(1);
        delay(100);
        if (digitalRead(onay_butonu) == LOW)
        {
          yonlendirme(16,1);
        }
        if (digitalRead(azaltma_butonu) == LOW)
        {
            while (digitalRead(azaltma_butonu) == LOW)
                delay(10);
            lcd.clear();
            menu = 7;
        }
  }//MENU6 MANUEL KULUÇKA SÜRESİ AYARI BİTİŞ
else if(menu==7){//MENU6 MANUEL SAAT AYARI
              lcd.setCursor(0, 0);
        lcd.print("SAAT AYARI");
        lcd.setCursor(0, 1);
        lcd.print("MANUEL         ");
        lcd.createChar(1, aok);
        lcd.setCursor(8, 1);
        lcd.write(1);
        delay(100);
        if (digitalRead(onay_butonu) == LOW)
        {
          vericekme_kontrol=0;
          yonlendirme(17,1);
        }
        if (digitalRead(azaltma_butonu) == LOW)
        {
            while (digitalRead(azaltma_butonu) == LOW)
                delay(10);
            lcd.clear();
            menu = 8;
        }
  }//MENU6 MANUEL SAAT AYARI BİTİŞ
else if(menu==8){//MENU8 KULUÇKA BAŞLAT
        lcd.setCursor(0, 0);
        lcd.print("KULUCKA BASLAT !");
        lcd.setCursor(0, 1);
        lcd.print("MANUEL         ");
        lcd.createChar(1, aok);
        lcd.setCursor(8, 1);
        lcd.write(1);
        delay(100);
        if (digitalRead(onay_butonu) == LOW)
        {
          yonlendirme(9,1);
        }
        if (digitalRead(azaltma_butonu) == LOW)
        {
            while (digitalRead(azaltma_butonu) == LOW)
                delay(10);
            lcd.clear();
            menu = 2;
        }
  }//MENU8 KULUÇKA BAŞLAT
else if(menu==9)//KULUÇKA BAŞLAT KODLARI
{
  //KULUÇKA BAŞLAT DÜĞMESİNE EKLE
BitisZamani= zaman_ekle(myTime , kulucka_suresi );
lcd.setCursor(0,0);
lcd.print("KULUCKA BASLADI");
lcd.setCursor(0,1);
KalanGun= dateDiff(year(myTime),month(myTime),day(myTime), year(BitisZamani),month(BitisZamani),day(BitisZamani));
lcd.print(KalanGun);
delay(700);
menu=1;
}
else if(menu==10){//AYARLARI GOSTEREN MENU
    
    lcd.setCursor(0,0);
    lcd.print("Tur:");
    lcd.setCursor(5,0);
    lcd.print(yumurta_turu);
    lcd.setCursor(0,1);
    lcd.print(gelisme_sicaklik);
    lcd.setCursor(6,1);
    lcd.print(gelisme_nem);
    delay(2000);
    lcd.clear();
    menu=1;
  }
else if(menu==11){
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CEVIRME KALAN DK");
  lcd.setCursor(0,1);
  int saniye=myTime-CevirmeZamani;
  int dakika=saniye/60;
  lcd.print(dakika);
  delay(2000);
  menu=1;
  lcd.clear();
  }
  
else if(menu==12){//---HAYVAN TÜRÜ SEÇME MENÜSÜ

  if(yumurta==0){//TAVUK
        lcd.setCursor(0, 0);
        lcd.print("1-TAVUK  ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.createChar(1, aok);
        lcd.setCursor(8, 1);
        lcd.write(1);
        delay(100);
         //*****
        if(digitalRead(onay_butonu)==0){
        deger = digitalRead(onay_butonu);
        while (deger == LOW)
        {
          delay(100);
          buton_suresi++;
          deger = digitalRead(onay_butonu); 
        }
        if(buton_suresi >0 && buton_suresi <= 4){//kısa basma durumu iptal
          delay(100);
          lcd.clear();
          lcd.print("IPTAL");
          delay(200);
          menu = 1;
          yumurta=0;
                              

        }
        else if(buton_suresi >=5 ){ //uzun basma durumunda islem onaylanacak 
          lcd.clear();
          lcd.print("KAYDEDILIYOR");
          //TAVUK MENÜSÜ İŞLEMLER
          yumurta_turu_degis(21 ,2,37.6 ,37.2,55,75,"Tavuk");
          delay(200); 
          buton_suresi=0;
          menu=1;
          yumurta=0;
          

        } 
        }
        ///****
        if (digitalRead(azaltma_butonu) == LOW)
        {
            while (digitalRead(azaltma_butonu) == LOW)
                delay(10);
            lcd.clear();
            yumurta = 1;
        }
    }//TAVUK BİTİŞ

     if(yumurta==1){//KAZ
        lcd.setCursor(0, 0);
        lcd.print("2-KAZ  ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.createChar(1, aok);
        lcd.setCursor(8, 1);
        lcd.write(1);
        delay(100);
        if(digitalRead(onay_butonu)==0){
        deger = digitalRead(onay_butonu);
        while (deger == LOW)
        {
          delay(100);
          buton_suresi++;
          deger = digitalRead(onay_butonu); 
        }
        if(buton_suresi >0 && buton_suresi <= 4){//kısa basma durumu iptal
          delay(100);
          lcd.clear();
          lcd.print("IPTAL");
          delay(200);
          menu = 1;
          yumurta=0;
                   
          
          
        }
        else if(buton_suresi >=5 ){
          //uzun basma durumunda islem onaylanacak
          lcd.clear();
          lcd.print("KAYDEDILIYOR");
          //KAZ MENÜSÜ İŞLEMLER
          yumurta_turu_degis(28 ,3,37.4 ,39.9,55,75,"Kaz");
          delay(200); 
          buton_suresi=0; 
          menu=1;
          yumurta=0;
        } 
        }
        if (digitalRead(azaltma_butonu) == LOW)
        {
            while (digitalRead(azaltma_butonu) == LOW)
                delay(10);
            lcd.clear();
            yumurta = 2;
        }
    }//KAZ BİTİŞ

         if(yumurta==2){//BILDIRCIN
        lcd.setCursor(0, 0);
        lcd.print("3-BILDIRCIN  ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.createChar(1, aok);
        lcd.setCursor(8, 1);
        lcd.write(1);
        delay(100);
        if(digitalRead(onay_butonu)==0){
        deger = digitalRead(onay_butonu);
        while (deger == LOW)
        {
          delay(100);
          buton_suresi++;
          deger = digitalRead(onay_butonu); 
        }
        if(buton_suresi >0 && buton_suresi <= 4){//kısa basma durumu iptal
          delay(100);
          lcd.clear();
          lcd.print("IPTAL");
          delay(200);
          menu = 1;
          yumurta=0;     
             

          

        }
        else if(buton_suresi >=5 ){
          //uzun basma durumunda islem onaylanacak
          lcd.clear();
          lcd.print("KAYDEDILIYOR");
          //BILDIRCIN MENÜSÜ İŞLEMLER
          yumurta_turu_degis(17 ,2,37.4 ,37.2,55,75,"Bldrcn");
          delay(200); 
          buton_suresi=0; 
          menu=1;
          yumurta=0;
        } 
        }
        if (digitalRead(azaltma_butonu) == LOW)
        {
            while (digitalRead(azaltma_butonu) == LOW)
                delay(10);
            lcd.clear();
            yumurta = 0;
        }
    }//BILDIRCIN BİTİŞ
  
  
  }//---HAYVAN TÜRÜ SEÇME MENÜSÜ BİTİŞ


else if(menu==13){//--DÖNDÜRME SAAT AYARI DEĞİŞTİRME

    lcd.setCursor(1,0);
  lcd.print("DONDURME ZMN AYARI");
  lcd.setCursor(8,1);
  lcd.print(dondurme_degeri+dondurme_artma);
  lcd.createChar(1,yok); 
  lcd.setCursor(0,0);   
  lcd.write(1);
  lcd.createChar(2,aok); 
  lcd.setCursor(0,1);   
  lcd.write(2);
  delay(10);
   
         if(digitalRead(onay_butonu)==0){
        deger = digitalRead(onay_butonu);
        while (deger == LOW)
        {
          delay(100);
          buton_suresi++;
          deger = digitalRead(onay_butonu); 
        }
        if(buton_suresi >0 && buton_suresi <= 4){//kısa basma durumu
          delay(100);
          lcd.clear();
          lcd.print("IPTAL");
          delay(200);
          //kısa basmada islem iptal
          buton_suresi=0;
          dondurme_artma=0;
          menu = 3;
        }
        else if(buton_suresi >=5 ){
          //uzun basma durumunda islem onaylanacak
          lcd.clear();
          lcd.print("KAYDEDILIYOR");
          delay(200); 
          buton_suresi=0;
          dondurme_degeri=dondurme_degeri+dondurme_artma;
          CevirmeZamani=myTime;
          CevirmeZamani=DakikaEkle( dondurme_degeri , myTime);
          dondurme_artma=0;
          menu=3;
        } 
       lcd.setCursor(0,0);
       lcd.print("                 ");
    }
      else if(digitalRead(artirma_butonu)==0){
        while(digitalRead(artirma_butonu)==0) delay(10);
      dondurme_artma=dondurme_artma+1;
    }
    else if(digitalRead(azaltma_butonu)==0){
      while(digitalRead(azaltma_butonu)==0) delay(10);
      dondurme_artma=dondurme_artma-1;
    }
    lcd.setCursor(8,1);
    lcd.print(dondurme_degeri+dondurme_artma);
  
  }//--DÖNDÜRME SAAT AYARI DEĞİŞTİRME BİTİŞ

else if(menu==14){//--NEM AYARI DEĞİŞTİRME
  lcd.setCursor(1,0);
  lcd.print(" NEM AYARI ");
  lcd.setCursor(8,1);
  lcd.print(nem_degeri+nem_artma);
  lcd.createChar(1,yok); 
  lcd.setCursor(0,0);   
  lcd.write(1);
  lcd.createChar(2,aok); 
  lcd.setCursor(0,1);   
  lcd.write(2);
  delay(10);
   
         if(digitalRead(onay_butonu)==0){
        deger = digitalRead(onay_butonu);
        while (deger == LOW)
        {
          delay(100);
          buton_suresi++;
          deger = digitalRead(onay_butonu); 
        }
        if(buton_suresi >0 && buton_suresi <= 4){//kısa basma durumu
          delay(100);
          lcd.clear();
          lcd.print("IPTAL");
          delay(200);
          //kısa basmada islem iptal
          buton_suresi=0;
          nem_artma=0;
          menu = 4;
        }
        else if(buton_suresi >=5 ){
          //uzun basma durumunda islem onaylanacak
          lcd.clear();
          lcd.print("KAYDEDILIYOR");
          delay(200); 
          buton_suresi=0;
          gelisme_nem=gelisme_nem+nem_artma;
          nem_artma=0;
          menu=4;
        } 
       lcd.setCursor(0,0);
       lcd.print("                 ");
    }
      else if(digitalRead(artirma_butonu)==0){
        while(digitalRead(artirma_butonu)==0) delay(10);
      nem_artma=nem_artma+0.5;
    }
    else if(digitalRead(azaltma_butonu)==0){
      while(digitalRead(azaltma_butonu)==0) delay(10);
      nem_artma=nem_artma-0.5;
    }
    lcd.setCursor(8,1);
    lcd.print(nem_degeri+nem_artma);  
  
  }//--NEM AYARI DEĞİŞTİRME BİTİŞ
  
else if(menu==15){//--SICAKLIK AYARI DEĞİŞTİRME
  lcd.setCursor(1,0);
  lcd.print(" SICAKLIK AYARI ");
  lcd.setCursor(8,1);
  lcd.print(gelisme_sicaklik+sicaklik_artma);
  lcd.createChar(1,yok); 
  lcd.setCursor(0,0);   
  lcd.write(1);
  lcd.createChar(2,aok); 
  lcd.setCursor(0,1);   
  lcd.write(2);
  delay(10);
   
         if(digitalRead(onay_butonu)==0){
        deger = digitalRead(onay_butonu);
        while (deger == LOW)
        {
          delay(100);
          buton_suresi++;
          deger = digitalRead(onay_butonu); 
        }
        if(buton_suresi >0 && buton_suresi <= 4){//kısa basma durumu
          delay(100);
          lcd.clear();
          lcd.print("IPTAL");
          delay(200);
          //kısa basmada islem iptal
          buton_suresi=0;
          sicaklik_artma=0;
          menu = 5;
        }
        else if(buton_suresi >=5 ){
          //uzun basma durumunda islem onaylanacak
          lcd.clear();
          lcd.print("KAYDEDILIYOR");
          delay(200); 
          buton_suresi=0;
          gelisme_sicaklik=gelisme_sicaklik+sicaklik_artma;
          sicaklik_artma=0;
          menu=5;
        } 
       lcd.setCursor(0,0);
       lcd.print("                 ");
    }
      else if(digitalRead(artirma_butonu)==0){
        while(digitalRead(artirma_butonu)==0) delay(10);
      sicaklik_artma=sicaklik_artma+0.5;
    }
    else if(digitalRead(azaltma_butonu)==0){
      while(digitalRead(azaltma_butonu)==0) delay(10);
      sicaklik_artma=sicaklik_artma-0.5;
    }
    lcd.setCursor(8,1);
    lcd.print(gelisme_sicaklik+sicaklik_artma);
  }//--SICAKLIK AYARI DEĞİŞTİRME BİTİŞ


else if(menu==16){//--KULUÇKA SURESİ AYARI DEĞİŞTİRME
  lcd.setCursor(1,0);
  lcd.print(" KULUCKA AYARI ");
  lcd.setCursor(8,1);
  lcd.print(kulucka_suresi+kulucka_artma);
  lcd.createChar(1,yok); 
  lcd.setCursor(0,0);   
  lcd.write(1);
  lcd.createChar(2,aok); 
  lcd.setCursor(0,1);   
  lcd.write(2);
  delay(10);
   
        if(digitalRead(onay_butonu)==0){
        deger = digitalRead(onay_butonu);
        while (deger == LOW)
        {
          delay(100);
          buton_suresi++;
          deger = digitalRead(onay_butonu); 
        }
        if(buton_suresi >0 && buton_suresi <= 4){//kısa basma durumu
          delay(100);
          lcd.clear();
          lcd.print("IPTAL");
          delay(200);
          //kısa basmada islem iptal
          buton_suresi=0;
          kulucka_artma=0;
          menu = 6;
        }
        else if(buton_suresi >=5 ){
          //uzun basma durumunda islem onaylanacak
          lcd.clear();
          lcd.print("KAYDEDILIYOR");
          delay(200); 
          buton_suresi=0;
          kulucka_suresi=kulucka_suresi+kulucka_artma;
          BitisZamani= zaman_ekle(myTime , kulucka_suresi );
          kulucka_artma=0;
          menu=6;
        } 
       lcd.setCursor(0,0);
       lcd.print("                 ");
    }
      else if(digitalRead(artirma_butonu)==0){
        while(digitalRead(artirma_butonu)==0) delay(10);
      kulucka_artma=kulucka_artma+1;
    }
    else if(digitalRead(azaltma_butonu)==0){
      while(digitalRead(azaltma_butonu)==0) delay(10);
      kulucka_artma=kulucka_artma-1;
    }
    lcd.setCursor(8,1);
    lcd.print(kulucka_suresi+kulucka_artma);
  
  }//--KULUÇKA SURESİ AYARI DEĞİŞTİRME
  
else if(menu==17){//--SAAT AYARI DEĞİŞTİRME
    
    if(saatmenu!=6)saatYazdir();
    
    if(saatmenu==0){//SAAT DEĞİŞTİRİLİYOR
          
        if (digitalRead(artirma_butonu) == LOW)
        {
            saat++;
            if(saat>24)saat=0;
            delay(200);
            lcd.clear();
        }
        else if (digitalRead(azaltma_butonu) == LOW)
        { 
            saat--;
            if(saat>24)saat=24;
            delay(200);
            lcd.clear();
        }
        else if (digitalRead(onay_butonu) == LOW)
        { 
            saatmenu=1;
            while (digitalRead(onay_butonu) == LOW)
               delay(10);
        }
      }//SAAT BİTİŞ

      
     else if(saatmenu==1){//DAKİKA 
        if (digitalRead(artirma_butonu) == LOW)
        {
            dakika++;
            if(dakika>60)dakika=0;
            delay(100);
            lcd.clear();
        }
        else if (digitalRead(azaltma_butonu) == LOW)
        { 
            dakika--;
            if(dakika>60)dakika=60;
            delay(100);
            lcd.clear();
        }
        else if (digitalRead(onay_butonu) == LOW)
        { 
            saatmenu=2;
            while (digitalRead(onay_butonu) == LOW)
               delay(10);
        }
     }
             else if(saatmenu==2){//SANİYE 
        if (digitalRead(artirma_butonu) == LOW)
        {
            saniye++;
            if(saniye>60)saniye=0;
            delay(100);
            lcd.clear();
        }
        else if (digitalRead(azaltma_butonu) == LOW)
        { 
            saniye--;
            if(saniye>60)saniye=60;
            delay(100);
            lcd.clear();
        }
        else if (digitalRead(onay_butonu) == LOW)
        { 
            saatmenu=3;
            while (digitalRead(onay_butonu) == LOW)
               delay(10);
        }
      }//SANİYE BİTİŞ

      else if(saatmenu==3){//YIL 
        if (digitalRead(artirma_butonu) == LOW)
        {
            yil++;
            if(yil>2050)yil=2022;
            delay(200);
            lcd.clear();
        }
        else if (digitalRead(azaltma_butonu) == LOW)
        { 
            yil--;
            if(yil<0 || yil>2050)yil=60;
            delay(200);
            lcd.clear();
        }
        else if (digitalRead(onay_butonu) == LOW)
        { 
            saatmenu=4;
            while (digitalRead(onay_butonu) == LOW)
               delay(10);
        }
      }//YİL BİTİŞ

        else if(saatmenu==4){//AY 
        if (digitalRead(artirma_butonu) == LOW)
        {
            ay++;
            if(ay>12)ay=0;
            delay(200);
            lcd.clear();
        }
        else if (digitalRead(azaltma_butonu) == LOW)
        { 
            ay--;
            if(ay>12)ay=12;
            delay(200);
            lcd.clear();
        }
        else if (digitalRead(onay_butonu) == LOW)
        { 
            saatmenu=5;
            while (digitalRead(onay_butonu) == LOW)
               delay(10);
        }
      }//AY BİTİŞ

            else if(saatmenu==5){//GÜN 
        if (digitalRead(artirma_butonu) == LOW)
        {
            gun++;
            if(gun>30)gun=0;
            delay(200);
            lcd.clear();
        }
        else if (digitalRead(azaltma_butonu) == LOW)
        { 
            ay--;
            if(ay>30)ay=30;
            delay(200);
            lcd.clear();
        }
        else if (digitalRead(onay_butonu) == LOW)
        { 
            saatmenu=6;
            lcd.clear();
            while (digitalRead(onay_butonu) == LOW)
               delay(10);
        }
      }//GÜN BİTİŞ
      
         else if(saatmenu==6){//KAYIT İÇİN ONAY ALIMI
          
          lcd.setCursor(0,0);
          lcd.print("kayit icin: +") ;
          lcd.setCursor(0,1);
          lcd.print("iptal icin: -") ;
        if (digitalRead(artirma_butonu) == LOW)
        {
            lcd.clear();
            lcd.print("KAYDEDILIYOR");
            RTC.set(tmConvert( yil,  ay,  gun,  saat,  dakika,  saniye));
            vericekme_kontrol=1;
            BitisZamani= zaman_ekle(tmConvert( yil,  ay,  gun,  saat,  dakika,  saniye) , kulucka_suresi );
            KalanGun= dateDiff(year(myTime),month(myTime),day(myTime), year(BitisZamani),month(BitisZamani),day(BitisZamani));
            delay(300);
            menu=0;
            saatmenu=0;
        }
        else if (digitalRead(azaltma_butonu) == LOW)
        { 
            lcd.clear();
            lcd.print("IPTAL EDILIYOR");
            delay(300);
            vericekme_kontrol=1;
            menu=0;
            saatmenu=0;
        }

      }//AY BİTİŞ
  
  }//--SAAT AYARI DEĞİŞTİRME BİTİŞ



else if(menu==55){
  
  
  }
}
 
