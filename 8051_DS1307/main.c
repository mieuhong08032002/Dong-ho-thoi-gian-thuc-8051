#include <REGX52.H>

//Giao tiep I2c

#define SCL    P3_0
#define SDA    P3_1
//define setup
#define set    P3_3
#define mode   P3_4
#define tang   P3_5
#define giam   P3_6
//Ham lien quan den delay
void delay_us(unsigned int t){
	unsigned int i;
	for(i=0;i<t;i++);
}
void delay_ms(unsigned int t){
	unsigned int i,j;
	for(i=0;i<t;i++)
		for(j=0;j<125;j++);;
}
void I2C_time_start(){
          SCL = 1;
          SDA = 1;
          SDA = 0;
          SCL = 0;
}
void I2C_time_stop(){
          SCL = 1;
          SDA = 0;
          SDA=1;
          SCL=1;
}
void I2C_write(unsigned char dat){
          unsigned char i;
          for (i=0;i<8;i++){
                    SDA = (dat & 0x80); ///? 1:0;
                    SCL=1;
                    SCL=0;
                    dat<<=1;
          }
          SCL = 1;
          SCL = 0;
}
unsigned char I2C_read(void){
          int rd_bit;
          unsigned char i, dat;
          dat = 0x00;
          for(i=0;i<8;i++){ /* For loop read data 1 byte */
                    SCL = 1;
                    rd_bit = SDA; /* Keep for check acknowledge */
                    dat = dat<<1;
                    dat = dat | rd_bit; /* Keep bit data in dat */
                    SCL = 0; /* Clear SCL */
          }
          return dat;
}
//Ham lien quan den ds1307
#define Dia_chi_DS1307 0x68
#define DS1307_Thanh_ghi_Giay 0x00
#define DS1307_Thanh_ghi_Phut 0x01
#define DS1307_Thanh_ghi_Gio 0x02
#define DS1307_Thanh_ghi_Thu 0x03
#define DS1307_Thanh_ghi_Ngay 0x04
#define DS1307_Thanh_ghi_Thang 0x05
#define DS1307_Thanh_ghi_Nam 0x06
#define DS1307_Thanh_ghi_Dieu_khien 0x07
#define DS1307_Nghin_Tram_Nam 0x10
typedef struct {
	unsigned int giay;
	unsigned int phut;
	unsigned int gio;
	unsigned int thu;
	unsigned int ngay;
	unsigned int thang;
	unsigned int nam;
}DS1307_STRUCT;
DS1307_STRUCT ds1307;
//Hàm giai mã tu BCD sang DEC
unsigned int DS1307_DecodeBCD(unsigned int bin) {
return (((bin & 0xf0) >> 4) * 10) + (bin & 0x0f);
}
//Hàm chuyen mã tu DEC sang BCD
unsigned int DS1307_EncodeBCD(unsigned int dec) {
	return (dec % 10 + ((dec / 10) << 4));
}
void DS1307_Ghi_Dulieu(int Dia_chi_thanh_ghi, int Gia_tri){
		
					I2C_time_start(); /* time_start i2c bus */
          I2C_write(Dia_chi_DS1307 << 1); /* Connect to DS1307 */
          I2C_write(Dia_chi_thanh_ghi); /* Requetime_5 RAM address on DS1307 */
          I2C_write(Gia_tri);
          I2C_time_stop();
}
int DS1307_Doc_Dulieu(int Dia_chi_thanh_ghi) {
		unsigned int val;
					I2C_time_start(); /* time_start i2c bus */
          I2C_write(0xD0); /* Connect to DS1307 */
          I2C_write(Dia_chi_thanh_ghi); /* Requetime_5 RAM address on DS1307 */
          I2C_time_start(); /* time_start i2c bus */
          I2C_write(0XD1); /* Connect to DS1307 for Read */
          val = I2C_read(); /* Receive data */
          I2C_time_stop();
		return val;
}
void DS1307_Cai_dat_Thoi_gian(int gio_24,int phut,int
giay,int thu,int ngay,int thang, int nam){
		DS1307_Ghi_Dulieu(DS1307_Thanh_ghi_Giay, DS1307_EncodeBCD(giay));
		DS1307_Ghi_Dulieu(DS1307_Thanh_ghi_Phut, DS1307_EncodeBCD(phut));
		DS1307_Ghi_Dulieu(DS1307_Thanh_ghi_Gio, DS1307_EncodeBCD(gio_24 &
		0x3f));
		DS1307_Ghi_Dulieu(DS1307_Thanh_ghi_Thu,DS1307_EncodeBCD(thu));
		DS1307_Ghi_Dulieu(DS1307_Thanh_ghi_Ngay,DS1307_EncodeBCD(ngay));
		DS1307_Ghi_Dulieu(DS1307_Thanh_ghi_Thang,DS1307_EncodeBCD(thang));
		DS1307_Ghi_Dulieu(DS1307_Nghin_Tram_Nam ,nam / 100);
		DS1307_Ghi_Dulieu(DS1307_Thanh_ghi_Nam,DS1307_EncodeBCD(nam% 100));
}
void DS1307_Doc_Thoi_gian(){
		unsigned int cen; //Lay 2 so hang nghin va tram cua nam cai dat
		ds1307.giay=DS1307_DecodeBCD(DS1307_Doc_Dulieu(DS1307_Thanh_ghi_Giay) & 0x7f);
		ds1307.phut=DS1307_DecodeBCD(DS1307_Doc_Dulieu(DS1307_Thanh_ghi_Phut));
		ds1307.gio=DS1307_DecodeBCD(DS1307_Doc_Dulieu(DS1307_Thanh_ghi_Gio) & 0x3f);
		ds1307.thu=DS1307_DecodeBCD(DS1307_Doc_Dulieu(DS1307_Thanh_ghi_Thu));
		ds1307.ngay=DS1307_DecodeBCD(DS1307_Doc_Dulieu(DS1307_Thanh_ghi_Ngay));
		ds1307.thang=DS1307_DecodeBCD(DS1307_Doc_Dulieu(DS1307_Thanh_ghi_Thang));
		cen = DS1307_Doc_Dulieu(DS1307_Nghin_Tram_Nam ) * 100;
		ds1307.nam=DS1307_DecodeBCD(DS1307_Doc_Dulieu(DS1307_Thanh_ghi_Nam)) + cen;
}

//Ham lien quan den LCD
#define LCD_RS P2_0
#define LCD_RW P2_1
#define LCD_EN P2_2
#define LCD_D0 P0_0
#define LCD_D1 P0_1
#define LCD_D2 P0_2
#define LCD_D3 P0_3
#define LCD_D4 P0_4
#define LCD_D5 P0_5
#define LCD_D6 P0_6
#define LCD_D7 P0_7

#define Dia_chi_LCD 0x4E // Ðia chi mac dinh cua modul

void LCD_DATA(unsigned int datas){
		LCD_D0 = (datas&0x01);
		LCD_D1 = ((datas>>1)&0x01);
		LCD_D2 = ((datas>>2)&0x01);
		LCD_D3 = ((datas>>3)&0x01);
		LCD_D4 = ((datas>>4)&0x01);
		LCD_D5 = ((datas>>5)&0x01);
		LCD_D6 = ((datas>>6)&0x01);
		LCD_D7 = ((datas>>7)&0x01);
}
void Lcd_Ghi_Lenh (unsigned int command){
		LCD_RS = 0;	//RS=0
		LCD_RW = 0; //LCD_RW=0;
		LCD_EN = 1; //LCD_EN = 1;
		LCD_DATA(command);
		LCD_EN = 0; //LCD_EN=0;
		delay_ms(10); //10ms
}
void Lcd_Ghi_Dulieu (unsigned char datas){
		LCD_RS = 1;	//RS=1
		LCD_RW = 0; //LCD_RW=0;
		LCD_EN = 1; //LCD_EN = 1;
		LCD_DATA(datas);
		LCD_EN = 0; //LCD_EN=0;
		delay_ms(10); //10ms
}
void lcd_init (void){
Lcd_Ghi_Lenh(0x03); //Bat dau
Lcd_Ghi_Lenh(0x02); // tro ve dau dong
Lcd_Ghi_Lenh(0x38);// Giao ti?p v?i VÐK b?ng 4 chân
Lcd_Ghi_Lenh(0x06);// T? d?ng dua con tr? d?n v? trí ti?p theo
Lcd_Ghi_Lenh(0x0c);// B?t hi?n th?, t?t con tr?
Lcd_Ghi_Lenh(0x01); // Xóa màn hình
}
void Lcd_Ghi_Chuoi (char *str){
		while (*str) Lcd_Ghi_Dulieu (*str++);
}

void Lcd_xoa_manhinh (void){
		Lcd_Ghi_Lenh (0x01); //xoa man hinh
}
void LCD_PutNumber(int number){
	if(number <= 9)
	{
		Lcd_Ghi_Dulieu(number + 48);
	}
	else if(number > 9 && number < 100)
	{
		Lcd_Ghi_Dulieu(number/10 + 48);
		Lcd_Ghi_Dulieu(number%10 + 48);
	}
	else if(number > 99 && number < 1000)
	{
		Lcd_Ghi_Dulieu(number/100 + 48);
		Lcd_Ghi_Dulieu(number/10%10 + 48);
		Lcd_Ghi_Dulieu(number%10 + 48);
	}
	else if(number > 999 && number < 10000)
	{
		Lcd_Ghi_Dulieu(number/1000 + 48);
		Lcd_Ghi_Dulieu(number/100%10 + 48);
		Lcd_Ghi_Dulieu(number/10%10 + 48);
		Lcd_Ghi_Dulieu(number%10 + 48);
	}
	
}
void day(int d) {
    switch (d) {
        case 0:
						Lcd_Ghi_Lenh(0x8c);
            Lcd_Ghi_Chuoi("DAY");
            break;
        case 1:
            Lcd_Ghi_Lenh(0x8c);
            Lcd_Ghi_Chuoi("SUN");
            break;
        case 2:
            Lcd_Ghi_Lenh(0x8c);
            Lcd_Ghi_Chuoi("MON");
            break;
        case 3:
            Lcd_Ghi_Lenh(0x8c);
            Lcd_Ghi_Chuoi("TUE");
            break;
        case 4:
            Lcd_Ghi_Lenh(0x8c);
            Lcd_Ghi_Chuoi("WED");
            break;
        case 5:
           Lcd_Ghi_Lenh(0x8c);
            Lcd_Ghi_Chuoi("THU");
            break;
        case 6:
            Lcd_Ghi_Lenh(0x8c);
            Lcd_Ghi_Chuoi("FRI");
            break;
        case 7:
            Lcd_Ghi_Lenh(0x8c);
            Lcd_Ghi_Chuoi("SAT");
            break;
    }
}
void hien_thi(){
	Lcd_Ghi_Lenh(0x80);//Dong 1
		LCD_PutNumber(ds1307.gio);
		Lcd_Ghi_Chuoi(" ");
		Lcd_Ghi_Dulieu (':');
		Lcd_Ghi_Lenh(0x84);
		LCD_PutNumber(ds1307.phut);
		Lcd_Ghi_Chuoi(" ");
		Lcd_Ghi_Dulieu (':');
		Lcd_Ghi_Lenh(0x88);
		LCD_PutNumber(ds1307.giay);
		Lcd_Ghi_Chuoi(" ");
		day(ds1307.thu);
		Lcd_Ghi_Lenh(0xC0);//Dong 2
		LCD_PutNumber(ds1307.ngay);
		Lcd_Ghi_Chuoi(" ");
		Lcd_Ghi_Dulieu ('/');
		Lcd_Ghi_Lenh(0xC4);
		LCD_PutNumber(ds1307.thang);
		Lcd_Ghi_Chuoi(" ");
		Lcd_Ghi_Dulieu ('/');
		Lcd_Ghi_Lenh(0xC8);
		LCD_PutNumber(ds1307.nam);
	delay_ms(200);
}
void hienthi(){
		Lcd_Ghi_Lenh(0x80);//Dong 1
		LCD_PutNumber(ds1307.gio);
		Lcd_Ghi_Chuoi(" ");
		Lcd_Ghi_Dulieu (':');
		Lcd_Ghi_Lenh(0x84);
		LCD_PutNumber(ds1307.phut);
		Lcd_Ghi_Chuoi(" ");
		Lcd_Ghi_Dulieu (':');
		Lcd_Ghi_Lenh(0x88);
		LCD_PutNumber(ds1307.giay);
		Lcd_Ghi_Chuoi(" ");
		day(ds1307.thu);
		Lcd_Ghi_Lenh(0xC0);//Dong 2
		LCD_PutNumber(ds1307.ngay);
		Lcd_Ghi_Chuoi(" ");
		Lcd_Ghi_Dulieu ('/');
		Lcd_Ghi_Lenh(0xC4);
		LCD_PutNumber(ds1307.thang);
		Lcd_Ghi_Chuoi(" ");
		Lcd_Ghi_Dulieu ('/');
		Lcd_Ghi_Lenh(0xC8);
		LCD_PutNumber(ds1307.nam);
	delay_ms(200);
}
unsigned int nam =0;
unsigned int thang =0;
unsigned int ngay = 0;
unsigned int thu = 0;
unsigned int gio =0;
unsigned int phut =0;
unsigned int giay =0;
 int set_count =0;
 int mode_count = 0;
void cai_dat(){
	while(1){
		if(mode == 0){
		while(mode == 0);
			mode_count++;
		}
		if(mode_count == 0){//set nam
			
			Lcd_Ghi_Lenh(0x80);//Dong 1
			Lcd_Ghi_Chuoi("Set nam       ");
			Lcd_Ghi_Lenh(0xC0);//Dong 2
			LCD_PutNumber(nam);
			Lcd_Ghi_Chuoi("      ");
			if(tang == 0){
				while(tang == 0);
				nam++;
				if(nam > 2099) nam =2000;
			}
			if(giam == 0){
				while(giam == 0);
				nam--;
				if(nam < 2000) nam =2099;
			}
	
		}else if(mode_count ==1 ){//set thang
			Lcd_Ghi_Lenh(0x80);//Dong 1
			Lcd_Ghi_Chuoi("Set thang       ");
			Lcd_Ghi_Lenh(0xC0);//Dong 2
			LCD_PutNumber(thang);
			Lcd_Ghi_Chuoi("      ");
			if(tang == 0){
				while(tang == 0);
				thang++;
				if(thang > 12) thang =0;
			}
			if(giam == 0){
				while(giam == 0);
				thang--;
				if(thang < 0) thang =12;
			}
		}else if(mode_count == 2){//set ngay

			Lcd_Ghi_Lenh(0x80);//Dong 1
			Lcd_Ghi_Chuoi("Set ngay         ");
			Lcd_Ghi_Lenh(0xC0);//Dong 2
			LCD_PutNumber(ngay);
			Lcd_Ghi_Chuoi("      ");
			if(tang == 0){
				while(tang == 0);
				ngay++;
				if(ngay > 31) ngay =0;
			}
			if(giam == 0){
				while(giam == 0);
				ngay--;
				if(ngay < 0) ngay =31;
			}
		}else if(mode_count ==3 ){//set thu
			
			Lcd_Ghi_Lenh(0x80);//Dong 1
			Lcd_Ghi_Chuoi("Set thu     ");
			Lcd_Ghi_Lenh(0xC0);//Dong 2
			Lcd_Ghi_Chuoi("                ");
			day(thu);
			if(tang == 0){
				while(tang == 0);
				thu++;
				if(thu > 7) thu =0;
			}
			if(giam == 0){
				while(giam == 0);
				thu--;
				if(thu < 0) thu = 7;
			}
		}else if(mode_count == 4){//set gio
			
			Lcd_Ghi_Lenh(0x80);//Dong 1
			Lcd_Ghi_Chuoi("Set gio        ");
			Lcd_Ghi_Lenh(0xC0);//Dong 2
			LCD_PutNumber(gio);
			Lcd_Ghi_Chuoi("      ");
			if(tang == 0){
				while(tang == 0);
				gio++;
				if(gio > 24) gio =0;
			}
			if(giam == 0){
				while(giam == 0);
				gio--;
				if(gio < 0) nam =24;
			}
		}else if(mode_count ==5 ){//set phut
			Lcd_Ghi_Lenh(0x80);//Dong 1
			Lcd_Ghi_Chuoi("Set phut        ");
			Lcd_Ghi_Lenh(0xC0);//Dong 2
			LCD_PutNumber(phut);
			Lcd_Ghi_Chuoi("      ");
			if(tang == 0){
				while(tang == 0);
				phut++;
				if(phut > 60) phut =0;
			}
			if(giam == 0){
				while(giam == 0);
				phut--;
				if(phut < 0) phut =60;
			}
		}else if(mode_count == 6){//set giay
			Lcd_Ghi_Lenh(0x80);//Dong 1
			Lcd_Ghi_Chuoi("Set giay        ");
			Lcd_Ghi_Lenh(0xC0);//Dong 2
			LCD_PutNumber(giay);
			Lcd_Ghi_Chuoi("      ");
			if(tang == 0){
				while(tang == 0);
				giay++;
				if(giay > 60) giay =0;
			}
			if(giam == 0){
				while(giam == 0);
				giay--;
				if(giay < 0) giay =60;
			}
		}else {//save and exit
			DS1307_Cai_dat_Thoi_gian(gio, phut, giay, thu, ngay, thang, nam);
			set_count=0;
			mode_count=0;
			Lcd_xoa_manhinh();
			break;
		}
}}
void gan_giatri(){
	nam  = ds1307.nam;
	thang= ds1307.thang;
	ngay = ds1307.ngay;
	thu  = ds1307.thu;
	gio  = ds1307.gio;
	phut = ds1307.phut;
	giay = ds1307.giay;
}
void main(){
	//cai dat thoi gian
	///DS1307_Cai_dat_Thoi_gian(02, 02,02, 2,01,2,2022);
	lcd_init();
	while(1){
	if(P3_3 == 0){
		while(P3_3 == 0);
		set_count++;
	}
	if(set_count == 0){
		DS1307_Doc_Thoi_gian(); 
		gan_giatri();
		delay_ms(100);
		hienthi();
	}else{
		Lcd_xoa_manhinh();
		cai_dat();
	}
}
}