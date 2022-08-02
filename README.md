스마트 자판기
=========

개요
----------
RFID 모듈로 고객카드를 인식, 카드번호와 상품정보를 서버로 전송하고 잔액과 나이 조건을 판별 상품을 판매하고 LCD로 출력   
실시간으로 모바일에 고객정보와 재고상태 확인 가능
   
디렉터리 설명
---------
* SERVER : 고객, 상품 관리 및 구매 처리
* Client : 고객카드 태그 및 상품 구입
      
      
구성도
---------
![mechanism](/Readme_src/mechanism.png)   
    
    
회로도
---------
![circuit](/Readme_src/sized_circuit.png)   

    
STM32F429ZI 핀 정보
--------
* PORT_A0 : GPIO = 온습도센서
* PORT_A3 : ADC1.IN3 = 조도센서
* PORT_A4 : ADC1.IN4 = 플레임센서
* PORT_B1 : GPIO = LED   
     
데이터베이스 테이블 구조
--------
![table](/Readme_src/table_structure.png)   
   
   * DATE : 현재 날짜
   * TIME : 현재 시간
   * HUMI : 습도 값
   * TEMP : 온도 값
   * FLAME : 불꽃 감지 여부 값 ( 1 or 0)
   * CDS : 조도 값

     
작동화면
----
* 좌측 화면에서는 실시간으로 데이터가 저장된 테이블을 출력, 우측에서는 google 차트를 이용하여 온습도 및 조도값 차트를 출력
* 불꽃 감지 시 페이지 배경색 붉은색으로 출력

![sized_running](/Readme_src/sized_running2.png)
