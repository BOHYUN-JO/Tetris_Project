﻿#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);

	move(8,WIDTH+10);
	DrawBox(9,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
	for(i=0; i<4; i++){
		move(10+i, WIDTH+13);
		for(j=0; j<4; j++){
			if( block[nextBlock[2]][0][i][j] == 1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}

}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i,j;
	for(i=0; i<BLOCK_HEIGHT; i++)
	{
		for(j=0; j<BLOCK_WIDTH; j++)
		{
			if(block[currentBlock][blockRotate][i][j]==1){
				if(i+blockY >= HEIGHT || j+blockX >= WIDTH || i+blockY<0 || j+blockX < 0)
					return 0;
				else if(f[i+blockY][j+blockX] == 1)
					return 0;
			}
		}
	}

	return 1;

}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){

	int i,j;
	int tmpRotate = blockRotate;
	int tmpY = blockY;
	int tmpX = blockX;
	int shadowY;
	// 1. 바뀌기 전 현재 블록의 정보 얻기
	switch(command){
	case KEY_UP:
		if(blockRotate == 0)
			tmpRotate = 3;
		else tmpRotate = blockRotate -1;
		break;
	case KEY_DOWN:
		tmpY = blockY-1;
		break;
	case KEY_RIGHT:
		tmpX = blockX-1;
		break;
	case KEY_LEFT:
		tmpX = blockX+1;
		break;
	defalut:
		break;
	}
	// 2. 이전에 그려진 현재 블록 지우기
	shadowY = tmpY;
	for(i=0; i<BLOCK_HEIGHT; i++)
	{
		for(j=0; j<BLOCK_WIDTH; j++)
		{
			if(block[currentBlock][tmpRotate][i][j] == 1)
			{
				while(1)
				{
					if(CheckToMove(field, currentBlock, tmpRotate,shadowY+1, tmpX) == 0) break;
					else
						shadowY++;
				}
				move(shadowY+i+1, tmpX+j+1);
				printw(".");
				move(tmpY+i+1, tmpX+j+1);
				printw(".");
			}
		}
	}
	// 3. 현재 블록을 화면에 그려주기
	DrawBlockWithFeatures(blockY,blockX,currentBlock, blockRotate);
	
	
}

void BlockDown(int sig){
	
	
	if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX  ))	//이동가능
	{
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}
	else	//이동 불가능
	{
		if(blockY == -1)	// gameOver
			gameOver = 1;
		else	// score 획득
		{
			AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
			score += DeleteLine(field);
			nextBlock[0] = nextBlock[1];
			nextBlock[1] = nextBlock[2];
			nextBlock[2] = rand() %7;
			blockRotate = 0;
			blockY = -1;
			blockX = WIDTH/2-2;
			DrawNextBlock(nextBlock);
			PrintScore(score);
			DrawField();
		}
	}
	timed_out =0;	// 0으로 초기화

}

void AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i,j,touched =0;
	for(i=0; i< BLOCK_HEIGHT; i++)
	{
		for(j=0; j<BLOCK_WIDTH; j++)
		{
			if(block[currentBlock][blockRotate][i][j] == 1){
				f[blockY+i][blockX+j] = 1;
				if(f[blockY+i+1][blockX+j]==1|| blockY+i == HEIGHT-1)
					touched++;
			}
		}
	}
	score += touched*10;

	//Block이 추가된 영역의 필드값을 바꾼다.
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	
	int i,j,flag,cnt = 0;

	for(i=0; i<HEIGHT; i++)
	{
		flag = 1;
		for(j=0; j<WIDTH; j++)
		{
			if(f[i][j] != 1)	// i번째 line이 꽉차지 않은 경우
			{
				flag = 0;
				break;
			}
		}
		if(flag == 1)	// i번째 line이 꽉 찬 경우
		{
			cnt++;	// 지운 line count;
			for(int k=i; k>0; k--)
			{
				for(j=0; j<WIDTH; j++)
					f[k][j] = f[k-1][j];
			}
		}
	}
	
	return cnt*cnt*100;	// score 반환
			

}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	int i,j;
	int tmpY = y;
	while(1)
	{
		if(CheckToMove(field,blockID,blockRotate, tmpY+1, x) == 0) break;
		else tmpY++;
	}
	DrawBlock(tmpY,x,blockID,blockRotate,'/');

}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	DrawShadow(y,x,blockID, blockRotate);
	DrawBlock(y,x,blockID,blockRotate, ' ');
}

void createRankList(){
	// user code
}

void rank(){
	// user code
}

void writeRankFile(){
	// user code
}

void newRank(int score){
	// user code
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}
