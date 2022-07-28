/*
 * Server.c
 *
 * Created: 2022-07-27 오후 4:46:39
 * Author : GunwooYun
 */ 

#define F_CPU 14745600L // 14.7456MHz
#define BAUDRATE_9600 95 // baud rate : 9600
#define BAUDRATE_115200 7 // baud rate : 115200
#define NUM_OF_CUSTOMER 5 // number of customers
#define NUM_OF_PRODUCT 5 // number of products
#define UART 1 // Way to send
#define BLUETOOTH 2 // Way to send

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


struct customer {
	char cID[30];
	char cName[20];
	int cAge;
	unsigned int cBal;
};

struct product{
	char pName[20];
	int price;
	int age;
	int stock;
	};

struct customer csArray[NUM_OF_CUSTOMER];
struct product pArray[NUM_OF_PRODUCT];

/* 레지스터 설정 초기화 */
void Init_UART(void);
void Init_BLUETOOTH(void);

/* UART 송수신 */
void UART0_transmit(uint8_t token);
uint8_t UART0_receive(void);

/* 블루투스 송수신 */
void UART1_transmit(uint8_t token);
uint8_t UART1_receive(void);

int checkCard(char* cn); // 카드번호 탐색

void sendInfo(char* info, uint8_t mode); // UART 또는 블루투스로 정보 전송

char* getCusInfo(uint8_t index); // 고객정보 리턴
char* getBleInfo(uint8_t index); // 블루투스 전송용 고객정보 리턴
char* getProInfo(uint8_t index); // 상품정보 리턴
char* getBleStock(uint8_t index); // 블루투스 전송용 재고 리턴

void calBalance(uint8_t index, uint8_t pIndex); //잔액계산

void dataInit(); // 고객정보 초기화

int main(void)
{
	Init_UART();
	Init_BLUETOOTH();
	dataInit();
	
	uint8_t cardToken; // 수신받은 문자
	char cardNum[30] = {0}; // 수신받은 카드 번호
	uint8_t cardIdx; // 카드조회 인덱스
	int csIndex = -1; // 고객정보 배열 인덱스
	
	
    /* Replace with your application code */
    while (1) 
    {
		cardIdx = 0;
		while(1){
			cardToken = UART0_receive();			
			cardNum[cardIdx++] = cardToken;
			if(cardToken == 'E'){
				cardNum[cardIdx] = '\0';
				break;
				}
		}
		/* 고객 정보 조회 및 전송 */
		if(((strlen(cardNum) > 13) && (strlen(cardNum) < 20)) && (cardNum[0] == 'a')){
			csIndex = checkCard(cardNum); // 카드번호로 고객 조회
			
			if(csIndex < 0){
				/* 고객정보 조회 실패 */
				sendInfo("1", 1);
			}
			else{
				/* 고객 정보 조회 성공 */
				char* cusInfoStr = getCusInfo(csIndex);
				if(cusInfoStr == NULL){
					continue;
				}
				char* cusInfoBle = getBleInfo(csIndex);
				if(cusInfoBle == NULL){
					free(cusInfoStr);
					continue;
				}
				sendInfo(cusInfoStr,UART); // 고객정보 UART 전송
				sendInfo(cusInfoBle,BLUETOOTH); // 고객정보 블루투스 전송
				
				free(cusInfoStr);
				free(cusInfoBle);
			}
		}		

		/* 상품 정보 조회 및 전송 */
		else if(cardNum[0] == 'c'){
			if(csIndex < 0){
				UART0_transmit('X');
				continue;
			}
			int pIdx = cardNum[1]-'0';
			char* proInfoStr = getProInfo(pIdx);
			if(proInfoStr == NULL){
				continue;
			}
			char* stoInfoStr = getBleStock(pIdx);
			if(stoInfoStr == NULL){
				free(proInfoStr);
				continue;
			}
			calBalance(csIndex, pIdx); // 잔액 계산
			sendInfo(proInfoStr, UART); // 상품 정보 UART 전송
			sendInfo(stoInfoStr, BLUETOOTH); // 구매 고객 정보 블루투스 전송
			
			free(proInfoStr);
			free(stoInfoStr);
		}
		//memset(cardNum, 0, sizeof(cardNum));
    }
}

void Init_UART(void)
{
	DDRE = 0x02; // Set TX output
	UCSR0B |= (1 << RXEN0 | 1 << TXEN0); // RXEN0 : receiver enable, TXEN0 : Transmitter enable
	UCSR0C |= 0x06; // UCSZn1 : 1 UCSZn0 : 1 -> 8-bit
	UBRR0L = BAUDRATE_9600; // speed : 9600
}

void Init_BLUETOOTH(void)
{
	DDRD = 0x08; // PORTD3 output
	UCSR1A = 0x00; // USART 제어및상태 레지스터, 
	/*
		UCSR1B = 0x98
		7 : RX Complete Interrupt Enable
		4 : Receiver Enable
		3 : Transmitter Enable
	*/
	UCSR1B = 0x18; // TXE, RXE Enable
	/*
		UCSR1C = 0x06
		6 : Asynchronous Operation
		4, 5 : 00 Non parity
		3 : 1 stop bit
		2 : Character size
		1 : Character size
		-> 8bit
	*/
	UCSR1C = 0x06; // 비동기, Non Parity, 1 Stop Bit
	UBRR1H = 0x00;
	UBRR1L = BAUDRATE_115200;
}

void UART1_transmit(uint8_t token)
{
	while(1)
	{
		
		// transmit buffer is ready to receive new data
		if((UCSR1A & (1 << UDRE1))) break;
	}
	UDR1 = token; // I/O Data Register
}

uint8_t UART1_receive(void)
{
	while(1)
	{
		if((UCSR1A & (1 << RXC1))) break;
	}
	return UDR1;
}

void UART0_transmit(uint8_t token)
{
	while(1)
	{
		// transmit buffer is ready to receive new data
		if((UCSR0A & (1 << UDRE0))) break;
	}
	UDR0 = token; // I/O Data Register
}

uint8_t UART0_receive(void)
{
	while(1)
	{
		if((UCSR0A & (1 << RXC1))) break;
	}
	return UDR0;
}

char* getCusInfo(uint8_t index)
{
	char* pCusInfo;
	pCusInfo = (char *)malloc(sizeof(char) * 30);
	if(pCusInfo == NULL){
		// failed malloc
		return NULL;
	}
	sprintf(pCusInfo, "b %s %d %u\n", csArray[index].cName, csArray[index].cAge, csArray[index].cBal);
	return pCusInfo;
}
char* getBleInfo(uint8_t index)
{
	char* pBleInfo;
	pBleInfo = (char *)malloc(sizeof(char) * 30);
	if(pBleInfo == NULL){
		// failed malloc
		return NULL;
	}
	sprintf(pBleInfo, "Name: %s Age : %d\n", csArray[index].cName, csArray[index].cAge);
	return pBleInfo;
}

char* getProInfo(uint8_t index)
{
	char* pProInfo;
	pProInfo = (char *)malloc(sizeof(char) * 30);
	if(pProInfo == NULL){
		// failed malloc
		return NULL;
	}
	sprintf(pProInfo, "d %s %d %d", pArray[index].pName, pArray[index].age, pArray[index].price);
	return pProInfo;
}

char* getBleStock(uint8_t index)
{
	char* pBleStock;
	pBleStock = (char *)malloc(sizeof(char) * 30);
	if(pBleStock == NULL){
		// failed malloc
		return NULL;
	}
	sprintf(pBleStock, "ID: %s Stock : %d\n", pArray[index].pName, pArray[index].stock);
	return pBleStock;
}

int checkCard(char* cn)
{
	for(int i = 0; i < NUM_OF_CUSTOMER; i++)
	{
		if(!strcmp(csArray[i].cID, cn))
			return i;
	}
	return -1;
}

void sendInfo(char* info, uint8_t mode)
{
	char* sendToken;
	
	if(!strcmp(info, "1")){
		UART0_transmit('1');
		return;
	}
	
	if(mode == UART){
		sendToken = info;
		while(*sendToken){
			UART0_transmit(*sendToken);
			sendToken++;
		}
	}
	else if(mode == BLUETOOTH)
	{
		sendToken = info;
		while(*sendToken){
			UART1_transmit(*sendToken);
			sendToken++;
		}
	}
}

void calBalance(uint8_t cIndex, uint8_t pIndex)
{
	if(csArray[cIndex].cAge < pArray[pIndex].age)
		return;
	if(pArray[pIndex].stock <= 0) return;
	int bal = csArray[cIndex].cBal - pArray[pIndex].price;
	if(bal > 0){
		csArray[cIndex].cBal = bal;
		pArray[pIndex].stock--;
	}
}

void dataInit()
{
	/* 고객정보 데이터 저장 */
	strcpy(csArray[0].cID, "a179 15 232 17E");
	strcpy(csArray[0].cName, "Tom");
	csArray[0].cAge = 21;
	csArray[0].cBal = 10000;

	strcpy(csArray[1].cID, "a211 153 231 17E");
	strcpy(csArray[1].cName, "Jack");
	csArray[1].cAge = 17;
	csArray[1].cBal = 30000;

	strcpy(csArray[2].cID, "a213 153 231 17E");
	strcpy(csArray[2].cName, "John");
	csArray[2].cAge = 32;
	csArray[2].cBal = 40000;

	strcpy(csArray[3].cID, "a211 153 561 17E");
	strcpy(csArray[3].cName, "Jane");
	csArray[3].cAge = 28;
	csArray[3].cBal = 20000;

	strcpy(csArray[4].cID, "a204 153 231 17E");
	strcpy(csArray[4].cName, "Mary");
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