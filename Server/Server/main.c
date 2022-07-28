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

void UART0_transmit(uint8_t token);
uint8_t UART0_receive(void);

uint8_t checkCard(char* cn); // 카드번호 탐색

void sendInfo(char* info, uint8_t mode); // UART 또는 블루투스로 정보 전송

char* getCusInfo(uint8_t index); // 고객정보 리턴
char* getBleInfo(uint8_t index); // 블루투스 전송용 고객정보 리턴
char* getProInfo(uint8_t index); // 상품정보 리턴
char* getBleStock(uint8_t index); // 블루투스 전송용 재고 리턴

void calBalance(uint8_t index, uint8_t pIndex); //잔액계산

void dataInit(); // 고객정보 초기화

int main(void)
{
	DDRE = 0x02; // Set TX output
	UCSR0B |= (1 << RXEN0 | 1 << TXEN0); // RXEN0 : receiver enable, TXEN0 : Transmitter enable
	UCSR0C |= 0x06; // UCSZn1 : 1 UCSZn0 : 1 -> 8-bit
	UBRR0L = BAUDRATE_9600; // speed : 9600
	
	uint8_t cardToken; // 수신받은 문자
	char cardNum[30] = {0}; // 수신받은 카드 번호
	uint8_t cardIdx; // 카드조회 인덱스
	uint8_t csIndex; // 고객정보 배열 인덱스
	
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
		/* 고객 정보 조회 및 전송 */
		if(((strlen(cardNum) > 13) && (strlen(cardNum) < 20)) && (cardNum[0] == 'a')){
			csIndex = checkCard(cardNum); // 카드번호로 고객 조회
			
			/* 카드번호 문자열 확인 */
			/*
			chkChar = cardNum;
			while(*chkChar){
				UART0_transmit(*chkChar);
				chkChar++;
			}
			*/
			
			/*
			UART0_transmit(csIndex + '0'); // csIndex 확인
			_delay_ms(1);
			chkCus = getCusInfo(csIndex);
			while(*chkCus){
				UART0_transmit(*chkCus);
				chkCus++;
			}
			_delay_ms(1);
			*/
			#if 1
			if(csIndex < 0){
				/* 고객정보 조회 실패 */
				sendInfo("1", 1);
			}
			else{
				/* 고객 정보 조회 성공 */
				char* cusInfoStr = getCusInfo(csIndex);
				if(cusInfoStr == NULL){
					
				}
				char* cusInfoBle = getBleInfo(csIndex);
				if(cusInfoBle == NULL){
					
				}
				sendInfo(cusInfoStr,UART);
				//sendInfo(cusInfoBle,BLUETOOTH);
				
				free(cusInfoStr);
				free(cusInfoBle);
			}			
			#endif
		}
		//memset(cardNum, 0, sizeof(cardNum));
		
		#if 1
		/* 상품 정보 조회 및 전송 */
		else if(cardNum[0] == 'c'){
			int pIdx = cardNum[1]-'0';
			char* proInfoStr = getProInfo(pIdx);
			if(proInfoStr == NULL){
				// malloc 실패
			}
			char* stoInfoStr = getBleStock(pIdx);
			if(stoInfoStr == NULL){
				// malloc 실패
			}
			calBalance(csIndex, pIdx);
			sendInfo(proInfoStr, UART);
			sendInfo(stoInfoStr, UART);
			
		}
		memset(cardNum, 0, sizeof(cardNum));
		#endif
		
    }
}

void Init_BLUETOOTH(void)
{
	DDRD = 0x08; // PORTD3 output
	UCSR1A = 0x00;
	UCSR1B = 0x98; // TXE, RXE Enable
	UCSR1C = 0x06; // 비동기, Non Parity, 1 Stop Bit
	UBRR1H = 0x00;
	UBRR1L = 0x07; //115200 bps
	DDRB = 0xff;
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


char* getCusInfo(uint8_t index)
{
	char* pCusInfo;
	pCusInfo = (char *)malloc(sizeof(char) * 30);
	if(pCusInfo == NULL){
		return NULL;
	}
	sprintf(pCusInfo, "b %s %d %d\n", csArray[index].cName, csArray[index].cAge, csArray[index].cBal);
	return pCusInfo;
}
char* getBleInfo(uint8_t index)
{
	char* pBleInfo;
	pBleInfo = (char *)malloc(sizeof(char) * 30);
	if(pBleInfo == NULL){
		return NULL;
	}
	sprintf(pBleInfo, "Name: %s Age : %d\n", csArray[index].cName, csArray[index].cAge);
	return pBleInfo;
	//return "Name: " + (String)csArray[index].cName + " Age : " + (String)csArray[index].cAge+"\n";
}
#if 1
char* getProInfo(uint8_t index)
{
	char* pProInfo;
	pProInfo = (char *)malloc(sizeof(char) * 30);
	if(pProInfo == NULL){
		return NULL;
	}
	sprintf(pProInfo, "d %s %d %d", pArray[index].pName, pArray[index].age, pArray[index].price);
	return pProInfo;
	//return "d " + (String)productArr[index] + " " + (String)ageArr[index] + " " + (String)price[index];
}


char* getBleStock(uint8_t index)
{
	char* pBleStock;
	pBleStock = (char *)malloc(sizeof(char) * 30);
	if(pBleStock == NULL){
		return NULL;
	}
	sprintf(pBleStock, "ID: %s Stock : %d\n", pArray[index].pName, pArray[index].stock);
	return pBleStock;
	//return "ID: " + (String)productArr[index] + " Stock : " + (String)stockArr[index]+"\n";
}

#endif

uint8_t checkCard(char* cn)
{
	for(int i = 0; i < NUM_OF_CUSTOMER; i++)
	{
		if(!strcmp(csArray[i].cID, cn))
		return 0;
	}
	return 1;
}

void sendInfo(char* info, uint8_t mode)
{
	char* sendToken;
	if(mode == UART){
		sendToken = info;
		while(*sendToken){
			UART0_transmit(*sendToken);
			sendToken++;
		}
	}
	else if(mode == BLUETOOTH)
	{
		
	}
}

void calBalance(uint8_t index, uint8_t pIndex)
{
	if(csArray[index].cAge < pArray[pIndex].age)
		return;
	if(pArray[pIndex].stock <= 0) return;
	int bal = csArray[index].cBal - pArray[pIndex].price;
	if(bal > 0){
		csArray[index].cBal = bal;
		pArray[pIndex].stock--;
	}
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

	strcpy(csArray[1].cID, "a211 153 231 17E");
	strcpy(csArray[1].cName, "Jack");
	//csArray[1].cID = "a211 153 231 17 \r\n";
	//csArray[1].cName = "Jack";
	csArray[1].cAge = 17;
	csArray[1].cBal = 30000;

	strcpy(csArray[2].cID, "a213 153 231 17E");
	strcpy(csArray[2].cName, "John");
	//csArray[2].cID = "a213 153 231 17 \r\n";
	//csArray[2].cName = "John";
	csArray[2].cAge = 32;
	csArray[2].cBal = 40000;

	strcpy(csArray[3].cID, "a211 153 561 17E");
	strcpy(csArray[3].cName, "Jane");
	//csArray[3].cID = "a211 153 561 17 \r\n";
	//csArray[3].cName = "Jane";
	csArray[3].cAge = 28;
	csArray[3].cBal = 20000;

	strcpy(csArray[4].cID, "a204 153 231 17E");
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