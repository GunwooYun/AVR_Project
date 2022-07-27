/*
 * Server.c
 *
 * Created: 2022-07-27 오후 4:46:39
 * Author : GunwooYun
 */ 

#define F_CPU 14745600L // 14.7456MHz
#define BAUDRATE_9600 95 // baud rate : 9600
#define NUM_OF_CUSTOMER 5 // number of customers
#define NUM_OF_PRODUCT 5 // number of products

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


struct customer {
	char cID[30];
	char cName[20];
	int cAge;
	int cBal;
};

struct product{
	char pName[20];
	int price;
	int age;
	int stock;
	};

struct customer csArray[NUM_OF_CUSTOMER];
struct product pArray[NUM_OF_PRODUCT];

/*
int price[PRODUCT] = {0, 1500, 4000, 1700, 2500};
char* productArr[PRODUCT] = {"N/A", "drink", "cigar", "snack", "coffee"};
int stockArr[PRODUCT] = {10, 4, 21, 4, 9};
char* ageArr[PRODUCT] = {"0", "0", "19", "0", "15"};
	*/

void UART0_transmit(uint8_t token);
uint8_t UART0_receive(void);

//void calBalance(int index, int pIndex); //잔액계산
char checkCard(char* cn); // 카드번호 탐색
/*
char* getCusInfo(int index); // 고객정보 리턴
char* getProInfo(int index); // 상품정보 리턴
char* getBleInfo(int index); // 블루투스 전송용 고객정보 리턴
char* getBleStock(int index); // 블루투스 전송용 재고 리턴
void sendInfo(char* info, int a); // 시리얼 전송
*/
void dataInit(); // 고객정보 초기화


int main(void)
{
	DDRE = 0x02; // Set TX output
	UCSR0B |= (1 << RXEN0 | 1 << TXEN0); // RXEN0 : receiver enable, TXEN0 : Transmitter enable
	UCSR0C |= 0x06; // UCSZn1 : 1 UCSZn0 : 1 -> 8-bit
	UBRR0L = BAUDRATE_9600; // speed : 9600
	
	uint8_t cardToken; // 수신받은 문자
	char cardNum[30] = {0}; // 수신받은 카드 번호
	char* chkChar;
	uint8_t cardIdx;
	char csIndex;
	
	dataInit(); // 고객정보 초기화
    /* Replace with your application code */
    while (1) 
    {
		cardIdx = 0;
		while(1){
			cardToken = UART0_receive();
			//UART0_transmit(cardToken);
			
			cardNum[cardIdx++] = cardToken;
			if(cardToken == 'E'){
				cardNum[cardIdx] = '\0';
				break;
				}
		}
		
		if(((strlen(cardNum) > 13) && (strlen(cardNum) < 20)) && (cardNum[0] == 'a')){
			
			chkChar = cardNum;
			while(*chkChar){
				UART0_transmit(*chkChar);
				chkChar++;
			}
			
			
			csIndex = checkCard(cardNum);
			UART0_transmit(csIndex);
			_delay_ms(1);
			#if 0
			if(csIndex < 0){
				printf("No Found customer");
				sendInfo("1", 1);
			}
			else{
				printf("Found customer --> index : %d\n", csIndex);
				String cusInfoStr = getCusInfo(csIndex);
				String cusInfoBle = getBleInfo(csIndex);
				sendInfo(cusInfoStr,1);
				sendInfo(cusInfoBle,2);
			}
			cardNum="";
			#endif
		}
		memset(cardNum, 0, sizeof(cardNum));
		
		#if 0
		else if(cardNum[0] == 'c'){
			int pIdx = cardNum[1]-'0';
			String proInfoStr = getProInfo(pIdx);
			String stoInfoStr = getBleStock(pIdx);
			calBalance(csIndex, pIdx);
			sendInfo(proInfoStr, 1);
			sendInfo(stoInfoStr, 2);
			cardNum="";
		
		
			
		}
		#endif
		
    }
}

void UART0_transmit(uint8_t token)
{
	
	while(1)
	{
		// empty : 1 & 1 -> 1
		if((UCSR0A & (1 << UDRE0))) break;
	}
	UDR0 = token;
}

uint8_t UART0_receive(void)
{
	while(1)
	{
		if((UCSR0A & (1 << RXC1))) break;
	}
	return UDR0;
}

#if 0
char* getCusInfo(uint8_t index)
{
	return "b "+csArray[index].cName+" "+csArray[index].cAge+" "+csArray[index].cBal;
}

char* getProInfo(uint8_t index)
{
	return "d " + (String)productArr[index] + " " + (String)ageArr[index] + " " + (String)price[index];
}

char* getBleInfo(uint8_t index)
{
	return "Name: " + (String)csArray[index].cName + " Age : " + (String)csArray[index].cAge+"\n";
}
char* getBleStock(uint8_t index)
{
	return "ID: " + (String)productArr[index] + " Stock : " + (String)stockArr[index]+"\n";
}

#endif
char checkCard(char* cn)
{
	for(int i = 0; i < NUM_OF_CUSTOMER; i++)
	{
		if(!strcmp(csArray[i].cID, cn))
		return 'O';
	}
	return 'x';
}

void dataInit()
{
	/* 고객정보 데이터 저장 */
	strcpy(csArray[0].cID, "a179 15 232 17E");
	strcpy(csArray[0].cName, "Tom");
	//csArray[0].cID = "a179 15 232 17 \r\n";
	//csArray[0].cName = "Tom";
	csArray[0].cAge = 21;
	csArray[0].cBal = 10000;

	strcpy(csArray[1].cID, "a211 153 231 17 \r\n");
	strcpy(csArray[1].cName, "Jack");
	//csArray[1].cID = "a211 153 231 17 \r\n";
	//csArray[1].cName = "Jack";
	csArray[1].cAge = 17;
	csArray[1].cBal = 30000;

	strcpy(csArray[2].cID, "a213 153 231 17 \r\n");
	strcpy(csArray[2].cName, "John");
	//csArray[2].cID = "a213 153 231 17 \r\n";
	//csArray[2].cName = "John";
	csArray[2].cAge = 32;
	csArray[2].cBal = 40000;

	strcpy(csArray[3].cID, "a211 153 561 17 \r\n");
	strcpy(csArray[3].cName, "Jane");
	//csArray[3].cID = "a211 153 561 17 \r\n";
	//csArray[3].cName = "Jane";
	csArray[3].cAge = 28;
	csArray[3].cBal = 20000;

	strcpy(csArray[4].cID, "a204 153 231 17 \r\n");
	strcpy(csArray[4].cName, "Mary");
	//csArray[4].cID = "a204 153 231 17 \r\n";
	//csArray[4].cName = "Mary";
	csArray[4].cAge = 56;
	csArray[4].cBal = 64000;

	/* 상품정보 데이터 저장 */
	strcpy(pArray[0].pName, "N/A");
	pArray[0].price = 0;
	pArray[0].age = 0;
	pArray[0].stock = 10;
	
	strcpy(pArray[1].pName, "drink");
	pArray[1].price = 1500;
	pArray[1].age = 0;
	pArray[1].stock = 4;
	
	strcpy(pArray[2].pName, "cigar");
	pArray[2].price = 4000;
	pArray[2].age = 19;
	pArray[2].stock = 21;
	
	strcpy(pArray[3].pName, "snack");
	pArray[3].price = 1700;
	pArray[3].age = 0;
	pArray[3].stock = 4;
	
	strcpy(pArray[4].pName, "coffee");
	pArray[4].price = 2500;
	pArray[4].age = 15;
	pArray[4].stock = 9;
		
	
}