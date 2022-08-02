스마트 룸 관리
=========

개요
----------
실내 온도와 습도, 조도를 센싱하여 그 값을 데이터베이스에 저장 및 웹 서버에서 관제하는 시스템 구성
   
디렉터리 설명
---------
* SERVER : 소켓 통신 및 DB SQL 실행 C 파일
* html : 웹페이지 파일
* arduino_RP_Project : 아두이노 테스트 프로젝트
* rp_projectFinal : STM32F429ZI 프로젝트   
      
      
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
