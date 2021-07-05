# Tetris_Project 소개

ncurses 라이브러리가 제공하는 리눅스 터미널 상에서의 GUI를 이용하여 테트리스 게임 프로그램을 제작하고 여기에 랭킹 시스템과 
블록 배치 추천 기능을 추가 구현한다. 

테트리스 게임은, 블록을 90도씩 반시계 방향으로 회전하거나 세 방향(좌, 우, 하)으로 움직여 필드에 쌓으면, 빈틈없이 채워진 
줄은 지워지고 그에 따른 스코어를 얻어 결과적으로 가장 높은 최종 스코어를 얻는 것이 목적이다. 

프로그램을 실행하면 사용자는 메뉴를 선택할 수 있다. 1번을 선택하면 테트리스 게임을 플레이할 수 있고, 2번을 선택하면 랭킹 
정보를 확인할 수 있고, 3번을 선택하면 블록 배치 추천 기능을 따라 플레이되는 모드로 테트리스 게임이 실행되고, 마지막으로
4번을 선택하면 프로그램이 종료된다. 

# 실행환경
1. OS : Ubuntu 16.04.2 LTS (GNU/Linux 4.4.0-193-generic x86_64)
2. ncurses 라이브러리 필요

# 컴파일 및 실행 방법
```
> make
> ./tetris
```

# 실행예시 

첫화면 - 메뉴 선택

![image](https://user-images.githubusercontent.com/70252973/124463090-5809bf00-ddcd-11eb-98e8-4261aeeb6b5f.png)

1. play 모드 

![image](https://user-images.githubusercontent.com/70252973/124463172-74a5f700-ddcd-11eb-8250-5ae55877d0d5.png)

기본적인 테트리스 게임과 같다.

![image](https://user-images.githubusercontent.com/70252973/124463186-7bcd0500-ddcd-11eb-954e-611a74b14d4f.png)

Game Over

2. rank 모드

![image](https://user-images.githubusercontent.com/70252973/124463270-9c955a80-ddcd-11eb-9f36-8a8a6725b3be.png)

랭크 기능 선택 화면

![image](https://user-images.githubusercontent.com/70252973/124463339-b767cf00-ddcd-11eb-8be1-61d27b830464.png)

랭크 출력 화면

3. recommend 모드

![image](https://user-images.githubusercontent.com/70252973/124463538-01e94b80-ddce-11eb-9b04-969512803e5b.png)

프로그램이 스스로 최적의 자리를 찾아 테트리스를 진행하는 모드


