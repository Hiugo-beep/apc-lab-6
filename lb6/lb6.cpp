#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

void get_reg();//функция вывода регистра 60р
void interrupt (*intOld09)(...); //IRQ1 прерывание от клаватуры
void interrupt intNew09(...) { system("cls"); get_reg(); intOld09(); }//новый обработчик прерываний для клавиатуры



void newMask(int mask) { //записать новую маску
	long TimeWait = 100000;
	while(--TimeWait>0) {
		if((inp(0x64) & 0x02) == 0x00) break;//если регистр свободен
	}
	if(TimeWait==0) { 
		printf("No turned out to access the keyboard\n");
		return;
	}	
	outp(0x60, 0xED);
	if(inp(0x60) == 0xFE) {//произошла ошибка при обработке байта
		 printf("Failed data transfer. Please, try again\n");
		return;
	}
	if(inp(0x60) == 0xFA) outp(0x60, mask); // байт успешно обработан + задана маска
	delay(1000); // пауза
}
void play() {
	printf("Start flashing!\n");
	int oldMask = inp(0x64);//старая маска
	oldMask = oldMask & 0x00; // биты 0-2 в 0

	int maskScroll = oldMask | 0x01; //маска для индикатора scroll
	int maskNum = oldMask | 0x02; //маска для индикатора num
	int maskCaps = oldMask | 0x04; //маска для индикатора caps

	newMask(maskCaps | maskNum | maskScroll);
	newMask(maskCaps | maskNum);
	newMask(maskCaps | maskScroll);
	newMask(maskCaps);
	newMask(maskNum | maskScroll);
	newMask(maskNum);
	newMask(maskScroll);
	newMask(oldMask);
	printf("Successful end of flashing indicators!\n");	
}

void printReturnCode() {
		intOld09 = getvect(0x09);
	_disable();//запретить прерывания 
   setvect(0x09 , intNew09);//новый обработчик прерываний
	_enable();//разрешить прерывания 
}

void get_reg()//получить состояние регистров
{
	char temp;
	int i;
	char number[4];
	number[0]='0';
	number[1]='x';
	int x = inp(0x60);
	for (i = 3; i>1; i--) //поделить по битам
	{
		temp = x % 16;
		x /= 16;
		if (temp >= 0 && temp <= 9) {
			number[i] = temp + '0';
		}
		else {
			number[i] = temp + 55;
		}
	}
	printf("Print \'q\' for exit.\nReturn code:\n");
	for(i=0;i<4;i++) printf("%c", number[i]);
}


int main() {
	char selection;
	while (1) {
		printf("1: Flashing keyboard indicatoprs\n");
		printf("2: Display return codes on hexidecimal form\n");
		printf("3: Exit(e)\n\n");
		selection = getch();

		switch (selection) {
			case '1':
				play();
				break;
			case '2':
				system("cls");
				printReturnCode();
				char k;
				do {
					k = getch();
				} while(k!='q');
				system("cls");
        	_disable();//запретить прерывания (cli
          setvect(0x09 , intOld09);
        	_enable();//разрешить прерывания (sti)
				break;
			case '3':
				printf("Exiting...");
				return 0;
			case 'e':
				printf("Exiting...");
				return 0;
			default:
				printf("Error key! Please try again\n");
				break;
		}
	}
}
