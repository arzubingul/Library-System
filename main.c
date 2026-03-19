#include <stdio.h>
#include <stdlib.h>
#include<string.h>

//SABITLER
#define MAX_KITAP 500
#define MAX_KULLANICI 100
#define MAX_AD 50
#define MAX_SIFRE 20
#define MAX_ODUNC_KITAP 3
#define ODUNC_SURESI 15
#define KULLANICI_DOSYA "kullanicilar.dat"
#define KITAP_DOSYA "kitaplar.dat"

//Odunc Alma Tarihi
typedef struct {
    int gun;
    int ay;
    int yil;
} Tarih;


//Kitap yapisi
typedef struct{
    int kitapId;
    char ad[100];
    char yazar[100];
    char kategori[50];
    int durum; // 0: raflarda, 1: oduncte
    int oduncAlanId;//�d�n� alan kullan�c�n�n id si
    Tarih oduncAlmaTarihi;
}Kitap;


//Odunc Bilgisi
typedef struct {
    int kitapId;
    Tarih oduncTarihi;
} OduncBilgisi;


// Kullanici Yapisi
typedef struct{
    int kullaniciId;
    char kullaniciAdi[50];
    char sifre[50];
    int tip; // 0:ogrenci, 1:personel, 2:yonetici(admin)
    OduncBilgisi oduncKitaplar[MAX_ODUNC_KITAP];
    int oduncKitapSayisi;
}Kullanici;


// Global degiskenler
Kitap kitaplar[MAX_KITAP];
Kullanici kullanicilar[MAX_KULLANICI];
int kitapSayisi=0;
int kullaniciSayisi=0;


// Dosya islemleri
void kitaplariDosyayaYukle();
void kullanicilariDosyayaYukle();
void kitaplariDosyayaKaydet();
void kullanicilariDosyayaKaydet();

// Ana menu fonksiyonlari
void anaMenu();
void yoneticiMenu();
void kullaniciMenu(Kullanici* aktifKullanici);

//Fonksiyon Prototipleri
void kitapEkle();
void kitapSil();
void kitapGuncelle();
void kitapAra();
void kitaplariListele();
void kitapOduncAl(Kullanici* aktifKullanici);
void kitapIadeEt(Kullanici* aktifKullanici);
void oduncKitaplariListele(Kullanici* aktifKullanici);
int gunFarki(Tarih t1, Tarih t2);
void kullaniciEkle();
void kullanicilariListele();
void gecikenKitaplariKontrolEt();
int enBuyukKitapId();//son kitap�n id si
int enBuyukKullaniciId();//son kullanicinin id si


int gunFarki(Tarih t1, Tarih t2) {
    // Her ayin gun sayisi (subat 28 gun )
    const int aylar[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Tarihleri gun cinsinden hesapla
    int days1 = t1.yil * 365 + t1.gun;
    for (int i = 0; i < t1.ay - 1; i++) {
        days1 += aylar[i];
    }

    int days2 = t2.yil * 365 + t2.gun;
    for (int i = 0; i < t2.ay - 1; i++) {
        days2 += aylar[i];
    }

    // Farki dondur (negatif olmamali)
    return (days2 > days1) ? (days2 - days1) : 0;
}


int main() {
    // Dosyalardan verileri yukle
    kitaplariDosyayaYukle();
    kullanicilariDosyayaYukle();


    // Yonetici hesabi olustur
    if (kullaniciSayisi == 0) {
        kullanicilar[0].kullaniciId = 1;
        strcpy(kullanicilar[0].kullaniciAdi, "yonetici");
        strcpy(kullanicilar[0].sifre, "yonetici123");
        kullanicilar[0].tip = 2;
        kullanicilar[0].oduncKitapSayisi = 0;
        kullaniciSayisi = 1;

        // Yonetici hesabini kaydet
        kullanicilariDosyayaKaydet();
    }

    anaMenu();
    return 0;
}

//Yardimci fonksiyon
void bekle() {
    printf("\nDevam etmek icin bir tusa basin:");
        while(getchar()!='\n');//Buffer da kalan \n i bekler
        getchar();//Asil beklemeyi yapar
}


//Dosyalara Yazma Islemleri
void kullanicilariDosyayaYukle(){
     FILE *kullaniciDosya=fopen(KULLANICI_DOSYA,"rb");
     if(kullaniciDosya==NULL){
         printf("Kullanici dosyasi bulunamadi!\n");
         return;
     }else{
        fread(&kullaniciSayisi,sizeof(int),1,kullaniciDosya);
        fread(kullanicilar,sizeof(kullanicilar),kullaniciSayisi,kullaniciDosya);
        fclose(kullaniciDosya);
     }
}

void kitaplariDosyayaYukle(){
     FILE *kitapDosya=fopen(KITAP_DOSYA,"rb");
     if(kitapDosya==NULL){
         printf("Kullanici dosyasi bulunamadi!\n");
         return;
     }else{
        fread(&kitapSayisi,sizeof(int),1,kitapDosya);
        fread(kitaplar,sizeof(Kitap),kitapSayisi,kitapDosya);
        fclose(kitapDosya);
     }
}

//Dosyalara Kaydetme Islemleri
void kullanicilariDosyayaKaydet(){
     FILE *kullaniciDosya=fopen(KULLANICI_DOSYA,"wb");
     if(kullaniciDosya==NULL){
         printf("Kullanici dosyasi bulunamadi!\n");
         return;
     }else{
        fwrite(&kullaniciSayisi,sizeof(int),1,kullaniciDosya);
        fwrite(kullanicilar, sizeof(Kullanici), kullaniciSayisi, kullaniciDosya);
        fclose(kullaniciDosya);
     }
}

void kitaplariDosyayaKaydet(){
     FILE *kitapDosya=fopen(KITAP_DOSYA,"wb");
     if(kitapDosya==NULL){
         printf("Kullanici dosyasi bulunamadi!\n");
         return;
     }else{
        fwrite(&kitapSayisi,sizeof(int),1,kitapDosya);
        fwrite(kitaplar,sizeof(Kitap),kitapSayisi,kitapDosya);
        fclose(kitapDosya);
     }
}


void anaMenu(){
    int secim;

    printf("\n\n~KUTUPHANE SISTEMI~\n\n");
    printf("1- Yonetici Girisi\n");
    printf("2- Kullanici Girisi\n");
    printf("0- Cikis\n");
    printf("Seciminiz: ");
    scanf("%d", &secim);

     if (secim==1) {
        char yoneticiAdi[MAX_AD], yoneticiSifre[MAX_SIFRE];
        printf("\nYonetici Adi: ");
        scanf("%49s",yoneticiAdi);//MAX_AD=50 oldugu icin 50. '0' isareti
        printf("Sifre: ");
        scanf("%19s",yoneticiSifre);//MAX_SIFRE=20 oldugu icin 20. '0' isareti

        if (strcmp(yoneticiAdi,"yonetici")==0 && strcmp(yoneticiSifre,"yonetici123")==0) {
            yoneticiMenu();
        } else {
            printf("Gecersiz yonetici girisi!\n");
        }
    } else if (secim == 2) {
        char kullaniciAdi[MAX_AD],sifre[MAX_SIFRE];
        printf("\nKullanici Adi: ");
        scanf("%49s",kullaniciAdi);
        printf("Sifre: ");
        scanf("%19s",sifre);

        int kullaniciBulundu=0; //girilen kullanici ve sifrelerin sistemdekilerle eslesip eslesmedigini kontrol etmek icin
        for (int i=0; i<kullaniciSayisi; i++) {
            if (strcmp(kullanicilar[i].kullaniciAdi,kullaniciAdi)==0 && strcmp(kullanicilar[i].sifre,sifre)==0) {
                kullaniciBulundu=1;
                kullaniciMenu(&kullanicilar[i]);
                break;
            }
        }
        if (kullaniciBulundu==0) {
            printf("Kullanici bulunamadi veya sifre hatali...\n");
        }
    }else if (secim == 0) {
        printf("Program sonlandiriliyor...\n");
        exit(0);
    } else {
        printf("Gecersiz secim!\n");
    }
    anaMenu();
}


// Yonetici Menusu
void yoneticiMenu() {
    int secim;
    do {
        printf("\n\n~YONETICI MENUSU~\n\n");
        printf("1- Kitap Ekle\n");
        printf("2- Kitap Sil\n");
        printf("3- Kitap Guncelle\n");
        printf("4- Kitaplari Listele\n");
        printf("5- Kullanici Ekle\n");
        printf("6- Kullanicilari Listele\n");
        printf("7- Geciken Kitaplari Kontrol Et\n");
        printf("0- Cikis\n");
        printf("Seciminiz: ");
        scanf("%d",&secim);

        switch(secim){
            case 1: kitapEkle(); break;
            case 2: kitapSil(); break;
            case 3: kitapGuncelle(); break;
            case 4: kitaplariListele(); break;
            case 5: kullaniciEkle(); break;
            case 6: kullanicilariListele();break;
            case 7: gecikenKitaplariKontrolEt(); break;
            case 0: printf("Yonetici menusunden cikiliyor...\n"); break;
            default: printf("Gecersiz secim!\n");
        }
    } while (secim!=0);
}


// Kullanici Menusu
void kullaniciMenu(Kullanici* aktifKullanici) {
    int secim;
    do {
        printf("\n\n~KULLANICI MENUSU~\n\n");
        printf("1- Kitap Ara\n");
        printf("2- Kitap Odunc Al\n");
        printf("3- Kitap Iade Et\n");
        printf("4- Odunc Alinan Kitaplari Listele\n");
        printf("0- Cikis\n");
        printf("Seciminiz: ");
        scanf("%d", &secim);

        switch(secim) {
            case 1: kitapAra(); break;
            case 2: kitapOduncAl(aktifKullanici); break;
            case 3: kitapIadeEt(aktifKullanici); break;
            case 4: oduncKitaplariListele(aktifKullanici); break;
            case 0: printf("Kullanici menusunden cikiliyor...\n"); break;
            default: printf("Gecersiz secim!\n");
        }
    } while (secim!=0);
}

// Kitap Ekleme
void kitapEkle() {
    if (kitapSayisi >= MAX_KITAP) {
        printf("Kitap kapasitesi dolu!\n");
        return;
    }

    Kitap yeniKitap;
    yeniKitap.kitapId = enBuyukKitapId()+1;
    yeniKitap.durum=0;
    yeniKitap.oduncAlanId=-1;

    printf("\nKitap Adi: (kucuk harf kullanin)");
    getchar();  // Onceki girislerden kalan newline karakterini temizliyor
    fgets(yeniKitap.ad, sizeof(yeniKitap.ad), stdin);
    yeniKitap.ad[strcspn(yeniKitap.ad, "\n")] = '\0';  // Sonundaki '\n' karakterini kaldirir

    printf("\nYazar Adi: (kucuk harf kullanin) ");
    fgets(yeniKitap.yazar, sizeof(yeniKitap.yazar), stdin);
    yeniKitap.yazar[strcspn(yeniKitap.yazar, "\n")] = '\0';

    printf("\nKategori: (kucuk harf kullanin) ");
    fgets(yeniKitap.kategori, sizeof(yeniKitap.kategori), stdin);
    yeniKitap.kategori[strcspn(yeniKitap.kategori, "\n")] = '\0';

    kitaplar[kitapSayisi++] = yeniKitap;
    printf("\nKitap eklendi! (ID:%d)\n", yeniKitap.kitapId);

    kitaplariDosyayaKaydet();//Dosya kaydetme
}

// Kitap Silme
void kitapSil(){

    if(kitapSayisi==0){
    printf("Kutuphane bos! \n");
    return;
    }

    // Mevcut kitaplari listele
    printf("\nMevcut Kitaplar:\n");
    kitaplariListele();


    int id, kitapBulundu=0;//Basta kitap bulunmadi olarak 0 atiyoruz
    printf("Silinecek Kitap ID: ");
    scanf("%d",&id);

    for (int i=0; i<kitapSayisi; i++) {
        if (kitaplar[i].kitapId == id) {
            kitapBulundu=1;
            for (int j=i; j<kitapSayisi-1; j++) {
                kitaplar[j] = kitaplar[j+1];
            }
            kitapSayisi--;
            printf("Kitap silindi.\n");

            //Dosya kaydetme islemleri
            kitaplariDosyayaKaydet();
            kullanicilariDosyayaKaydet();

            break;
        }
    }
    if (kitapBulundu==0) {
        printf("Kitap bulunamadi.\n");
    }
}

// Kitap Guncelleme
void kitapGuncelle() {
    if (kitapSayisi == 0) {
        printf("Kutuphane bos! Guncellenecek kitap yok.\n");
        return;
    }

    // Mevcut kitaplari listele
    printf("\nMevcut Kitaplar:\n");
    kitaplariListele();

    int id;
    printf("\nGuncellenecek kitabin ID numarasini girin: ");
    scanf("%d", &id);

    // Kitabi bul
    int kitapIndex = -1;//Basta kitap bulunmadi olarak -1 atiyoruz
    for (int i = 0; i < kitapSayisi; i++) {
        if (kitaplar[i].kitapId == id) {
            kitapIndex = i;
            break;
        }
    }

    if (kitapIndex == -1) {
        printf("%d ID'li kitap bulunamadi!\n", id);
        return;
    }

    // Yeni bilgileri al
    printf("\nYeni bilgileri girin (kucuk harf kullanin) (degistirmek istemiyorsaniz bos birakin):\n");

    // Kitap adi
    printf("Kitap Adi (Eski: %s): ", kitaplar[kitapIndex].ad);
    getchar(); // Buffer temizleme
    char yeniAd[100];
    fgets(yeniAd, sizeof(yeniAd), stdin);
    yeniAd[strcspn(yeniAd, "\n")] = '\0'; // Newline karakterini kaldir
    if (strlen(yeniAd) > 0) {
        strcpy(kitaplar[kitapIndex].ad, yeniAd);
    }

    // Yazar
    printf("Yazar (Eski: %s): ", kitaplar[kitapIndex].yazar);
    char yeniYazar[100];
    fgets(yeniYazar, sizeof(yeniYazar), stdin);
    yeniYazar[strcspn(yeniYazar, "\n")] = '\0';
    if (strlen(yeniYazar) > 0) {
        strcpy(kitaplar[kitapIndex].yazar, yeniYazar);
    }

    // Kategori
    printf("Kategori (Eski: %s): ", kitaplar[kitapIndex].kategori);
    char yeniKategori[50];
    fgets(yeniKategori, sizeof(yeniKategori), stdin);
    yeniKategori[strcspn(yeniKategori, "\n")] = '\0';
    if (strlen(yeniKategori) > 0) {
        strcpy(kitaplar[kitapIndex].kategori, yeniKategori);
    }

    // Durumu guncelle (sadece admin yapabilir)
    printf("Durumu degistirmek istiyor musunuz? (0: Hayir, 1: Evet): ");
    int durumSecim;
    scanf("%d", &durumSecim);
    if (durumSecim == 1) {
        printf("Yeni durum (0: Rafta, 1: Oduncte): ");
        scanf("%d", &kitaplar[kitapIndex].durum);
        if (kitaplar[kitapIndex].durum == 1) {
            printf("Odunc alan kullanici ID: ");
            scanf("%d", &kitaplar[kitapIndex].oduncAlanId);
        } else {
            kitaplar[kitapIndex].oduncAlanId = -1;
        }
    }

    printf("\nKitap bilgileri guncellendi!\n");

    // Degisiklikleri dosyaya kaydet
    kitaplariDosyayaKaydet();
    kullanicilariDosyayaKaydet();
}


// Kitaplari Listeleme
void kitaplariListele() {
    printf("\n~Kitaplar~\n");
    for (int i=0; i<kitapSayisi; i++) {
        char* durum = kitaplar[i].durum ? "Odunc" : "Rafta";
        printf("ID %d - %s (Yazar: %s, Kategori: %s, Durum: %s)\n",
               kitaplar[i].kitapId, kitaplar[i].ad, kitaplar[i].yazar,
               kitaplar[i].kategori, durum);
    }
}

//Kullanici Ekleme
void kullaniciEkle(){
    //Max kullaniciya ulasilmis mi diye kontrol
    if(kullaniciSayisi>=MAX_KULLANICI){
        printf("Kullanici kapasitesi dolu!\n");
        return;
    }

    Kullanici yeniK;
    yeniK.kullaniciId=enBuyukKullaniciId()+1;

    printf("\nKullanici Adi: ");
    getchar();
    fgets(yeniK.kullaniciAdi, sizeof(yeniK.kullaniciAdi), stdin);
    yeniK.kullaniciAdi[strcspn(yeniK.kullaniciAdi, "\n")] = 0;  // '\n' karakterini temizle

    printf("Sifre: ");
    fgets(yeniK.sifre, sizeof(yeniK.sifre), stdin);
    yeniK.sifre[strcspn(yeniK.sifre, "\n")] = 0;

    printf("Kullanici Tipi (0: Ogrenci, 1: Personel): ");
    scanf("%d", &yeniK.tip);
    getchar();  // '\n' karakterini temizle

    yeniK.oduncKitapSayisi = 0;
    kullanicilar[kullaniciSayisi++] = yeniK;

    printf("\nKullanici eklendi! (ID: %d)\n", yeniK.kullaniciId);

    //Dosya kaydetme islemi
    kullanicilariDosyayaKaydet();
}

//Kullanicilari Listeleme
void kullanicilariListele(){
    printf("\n~Kullanicilar~\n");

    // Sadece gercek kullanicilari listele (kullaniciSayisi kadar)
    for(int i = 0; i < kullaniciSayisi; i++) {
       // Sadece gecerli ID'ye sahip kullanicilari goster
        if(kullanicilar[i].kullaniciId > 0) {
            printf("ID %d - %s (Tip: %d)\n",
                  kullanicilar[i].kullaniciId,
                  kullanicilar[i].kullaniciAdi,
                  kullanicilar[i].tip);
         }
     }
}

//Kitap Arama
void kitapAra(){
    getchar();  //Onceki '\n' karakterini temizliyor

    char arananAd[MAX_AD];
    char arananYazar[MAX_AD];
    char arananKategori[MAX_AD];

    printf("\nKitap adi girin (kucuk harf kullanin)(bos birakabilirsiniz): ");
    fgets(arananAd, sizeof(arananAd), stdin);
    arananAd[strcspn(arananAd, "\n")] = 0;

    printf("\nYazar adi girin (kucuk harf kullanin) (bos birakabilirsiniz): ");
    fgets(arananYazar, sizeof(arananYazar), stdin);
    arananYazar[strcspn(arananYazar, "\n")] = 0;

    printf("\nKategori girin (kucuk harf kullanin) (bos birakabilirsiniz): ");
    fgets(arananKategori, sizeof(arananKategori), stdin);
    arananKategori[strcspn(arananKategori, "\n")] = 0;

    printf("\n~Arama Sonuclari~\n");
    int kitapBulundu=0;//Basta bulunmadi olarak 0 atiyoruz

    for (int i=0; i<kitapSayisi; i++) {
        int eslesiyor=1;

        // Sadece girilmis olan alanlara gore karsilastirma yapilir
        if (strlen(arananAd)>0 && strstr(kitaplar[i].ad, arananAd)==NULL){
            eslesiyor=0;
        }
        if (strlen(arananYazar)>0 && strstr(kitaplar[i].yazar, arananYazar) == NULL) {
            eslesiyor=0;
        }
        if (strlen(arananKategori)>0 && strstr(kitaplar[i].kategori, arananKategori) == NULL) {
            eslesiyor=0;
        }
        if (eslesiyor){
            printf("ID %d - %s (Yazar: %s, Kategori: %s)\n",
                   kitaplar[i].kitapId, kitaplar[i].ad,
                   kitaplar[i].yazar, kitaplar[i].kategori);
            kitapBulundu = 1;
        }
    }

    if (kitapBulundu==0) {
        printf("Eslesen kitap bulunamadi.\n");
    }
}

void kitapOduncAl(Kullanici* aktifKullanici) {
    printf("\n~Kitap Odunc Alma Islemi~\n");

    //Odunc alma limit kontrolu
    if ((*aktifKullanici).oduncKitapSayisi >= MAX_ODUNC_KITAP) {
        printf("Uyari! En fazla %d kitap odunc alabilirsiniz. Limitiniz doldu.\n", MAX_ODUNC_KITAP);
        return;
    }

    //Kitaplari listele
    printf("\nMevcut Kitaplar:\n");
    kitaplariListele();

    //Kitap secimi
    int kitapId;
    printf("\nOdunc almak istediginiz kitabin ID numarasini girin: ");
    scanf("%d", &kitapId);

    //Kitabi pointer ile bul
    Kitap* secilenKitap = NULL; //Basta null atiyoruz
    for (int i = 0; i < kitapSayisi; i++) {
        if (kitaplar[i].kitapId == kitapId) {
            secilenKitap = &kitaplar[i]; //Kitap bulununca adresini atiyoruz
            break;
        }
    }

    //Girilen id kutuphanedeki herhangi bir kitapla eslesiyor mu kontrolu
    if (secilenKitap == NULL) {
        printf("Hata! %d ID numarali kitap bulunamadi.\n", kitapId);
        return;
    }
    //Kitap rafta mi oduncte mi kontrolu
    if ((*secilenKitap).durum == 1) {
        printf("Uyari! Bu kitap zaten odunc alinmis durumda.\n");
        return;
    }

    //Kullanicidan tarih bilgisi al
    Tarih oduncTarihi;
    printf("\nOdunc alma tarihini giriniz (Gun Ay Yil): ");
    scanf("%d %d %d", &oduncTarihi.gun, &oduncTarihi.ay, &oduncTarihi.yil);

    //Kitap durumunu guncelle (pointer kullanarak)
    (*secilenKitap).durum = 1;
    (*secilenKitap).oduncAlanId = (*aktifKullanici).kullaniciId;
    (*secilenKitap).oduncAlmaTarihi = oduncTarihi;

    //Kullanicinin odunc listesine ekle
    (*aktifKullanici).oduncKitaplar[(*aktifKullanici).oduncKitapSayisi].kitapId = kitapId;
    (*aktifKullanici).oduncKitaplar[(*aktifKullanici).oduncKitapSayisi].oduncTarihi = oduncTarihi;
    (*aktifKullanici).oduncKitapSayisi++;

    //Iade tarihini hesapla
    Tarih iadeTarihi = oduncTarihi;
    const int aylar[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    iadeTarihi.gun += ODUNC_SURESI;

    // Gercek aylara gore tarih hesaplama
    while (iadeTarihi.gun > aylar[iadeTarihi.ay - 1]) {
    iadeTarihi.gun -= aylar[iadeTarihi.ay - 1];
    iadeTarihi.ay++;
    if (iadeTarihi.ay > 12) {
        iadeTarihi.ay = 1;
        iadeTarihi.yil++;
    }
}

    printf("\nIslem Basarili.\n");
    printf("'%s' (ID: %d) adli kitap odunc alindi.\n", (*secilenKitap).ad, kitapId);
    printf("Iade tarihi: %d/%d/%d\n", iadeTarihi.gun, iadeTarihi.ay, iadeTarihi.yil);

    //Degisiklikleri kaydet
    kitaplariDosyayaKaydet();
}

//Odunc Kitaplari Listele
void oduncKitaplariListele(Kullanici* aktifKullanici) {
    printf("\n~Odunc Alinan Kitaplar~\n");

    if ((*aktifKullanici).oduncKitapSayisi == 0) {
        printf("Odunc alinan kitap bulunmamaktadir.\n");
        return;
    }

    // Bugunun tarihini al(gecikmeyi hesaplamak icin aliyoruz)
    Tarih bugun;
    printf("\nBugunun tarihini girin (gg aa yyyy): ");
    scanf("%d %d %d", &bugun.gun, &bugun.ay, &bugun.yil);
    getchar(); // Buffer temizleme

    for (int i = 0; i < (*aktifKullanici).oduncKitapSayisi; i++) {
        int kitapId = (*aktifKullanici).oduncKitaplar[i].kitapId;

        // Kitap bilgilerini bul
        for (int j = 0; j < kitapSayisi; j++) {
            if (kitaplar[j].kitapId == kitapId) {
                // Gecikme hesapla
                int gecikme = gunFarki((*aktifKullanici).oduncKitaplar[i].oduncTarihi, bugun) - ODUNC_SURESI;

                // Kitap bilgilerini yazdir
                printf("ID %d - %s (Yazar: %s, Odunc Tarihi: %02d/%02d/%d",
                       kitaplar[j].kitapId,
                       kitaplar[j].ad,
                       kitaplar[j].yazar,
                       (*aktifKullanici).oduncKitaplar[i].oduncTarihi.gun,
                       (*aktifKullanici).oduncKitaplar[i].oduncTarihi.ay,
                       (*aktifKullanici).oduncKitaplar[i].oduncTarihi.yil);

                // Gecikme varsa goster
                if (gecikme > 0) {
                    printf(" - Gecikme: %d gun", gecikme);
                }
                printf("\n");
                break;
            }
        }
    }
}

//Geciken Kitaplari Kontrol Etme
void gecikenKitaplariKontrolEt() {
    Tarih bugun;
    printf("Bugunun tarihini giriniz (gg aa yyyy): ");
    scanf("%d %d %d", &bugun.gun, &bugun.ay, &bugun.yil);

    printf("\n~Geciken Kitaplar~\n");
    for (int i = 0; i < kullaniciSayisi; i++) {
        for (int j = 0; j < kullanicilar[i].oduncKitapSayisi; j++) {
            Tarih oduncTarihi = kullanicilar[i].oduncKitaplar[j].oduncTarihi;
            int fark = gunFarki(oduncTarihi, bugun);
            if (fark > ODUNC_SURESI) {
                printf("Kullanici: %s | Kitap ID: %d | Gecikme: %d gun\n",
                    kullanicilar[i].kullaniciAdi,
                    kullanicilar[i].oduncKitaplar[j].kitapId,
                    fark - ODUNC_SURESI);
            }
        }
    }
}


//Kitap Iade Etme
void kitapIadeEt(Kullanici* aktifKullanici){

    printf("\n~Kitap iade islemi~\n");

    if ((*aktifKullanici).oduncKitapSayisi == 0) {
        printf("Odunc aldiginiz kitap bulunmamaktadir.\n");
        return;
    }

     // Kullanicinin odunc kitaplarini listele
    printf("\nOdunc Aldiginiz Kitaplar:\n");
    oduncKitaplariListele(aktifKullanici);


    int id;
    printf("\nIade etmek istediginiz kitabin ID numarasini girin: ");
    scanf("%d", &id);

    int kitapIndex = -1;
    // Kullanicinin odunc kitaplarinda ara
    for (int i = 0; i < (*aktifKullanici).oduncKitapSayisi; i++) {
        if ((*aktifKullanici).oduncKitaplar[i].kitapId == id) {
            kitapIndex = i;
            break;
        }
    }
     if (kitapIndex == -1) {
        printf("Hata! Bu kitap sizin odunc aldiginiz kitaplar arasinda degil.\n");
        return;
    }

   // Kitabi bul ve hem ID hem de odunc alan kullaniciyi kontrol et
    int kitapBulundu=0;// Baslangicta "kitap bulunamadi" varsayiyoruz
      for (int j = 0; j < kitapSayisi; j++) {
         if (kitaplar[j].kitapId == id && kitaplar[j].oduncAlanId == (*aktifKullanici).kullaniciId) {
             kitaplar[j].durum = 0;//Kitap artik raflarda
             kitaplar[j].oduncAlanId=-1;//OdUnC alan kullanici yok
             kitapBulundu=1;//Kitap bulundu olarak isaretliyoruz

              // Kullanicinin odunc listesinden cikartiyoruz ve kitaplari kaydirip duzenliyoruz
            for (int k = kitapIndex; k < (*aktifKullanici).oduncKitapSayisi - 1; k++) {
                (*aktifKullanici).oduncKitaplar[k] = (*aktifKullanici).oduncKitaplar[k + 1];
            }
            (*aktifKullanici).oduncKitapSayisi--;

             // Kitap bilgilerini goster
            printf("\n'%s' (ID: %d) kitabi basariyla iade edildi.\n",
                   kitaplar[j].ad, kitaplar[j].kitapId);

            // Degisiklikleri kaydet
            kitaplariDosyayaKaydet();
            kullanicilariDosyayaKaydet();
            break;
        }
   }
   if(!kitapBulundu){
     printf("Hata! Kitap bulunamadi.\n");
  }
}

//Her Yeni Kitap Eklendiginde Duzenlemek Icin
int enBuyukKitapId() {
    int maxId = 0;
    for (int i = 0; i < kitapSayisi; i++) {
        if (kitaplar[i].kitapId > maxId) {
            maxId = kitaplar[i].kitapId;
        }
    }
    return maxId;
}

//Her Yeni Kullanici Eklendiginde Duzenlemek Icin
int enBuyukKullaniciId() {
    int maxId = 0;
    for (int i = 0; i < kullaniciSayisi; i++) {
        if (kullanicilar[i].kullaniciId > maxId) {
            maxId = kullanicilar[i].kullaniciId;
        }
    }
    return maxId;
}

