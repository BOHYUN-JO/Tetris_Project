#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));
	createRankList();
	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_REC_PLAY: recommendPlay(); break;
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

	for(i=0; i<BLOCK_NUM; i++)
		nextBlock[i] = rand() %7;

	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	recRoot = (RecNode*)malloc(sizeof(RecNode));
	recRoot->score = 0;
	recRoot->lv = -1;
	recRoot->recBlockX = -3;
	recRoot->recBlockY = -1;
	recRoot->recBlockRotate = 0;
	recRoot->curBlockID = nextBlock[0];
	
	modified_recommend(recRoot);

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	DrawRecommend(recRoot->recBlockY, recRoot->recBlockX, recRoot->curBlockID, recRoot->recBlockRotate);
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
			score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
			score += DeleteLine(field);
			for(int i=0; i< BLOCK_NUM; i++)
			{
				if(i == BLOCK_NUM-1)
					nextBlock[i] = rand() % 7;
				else
					nextBlock[i]= nextBlock[i+1];
			}

			blockRotate = 0;
			blockY = -1;
			blockX = WIDTH/2-2;

			recRoot->curBlockID = nextBlock[0];
			recRoot->recBlockX = blockX;
			recRoot->recBlockY = blockY;
			recRoot->recBlockRotate = blockRotate;
			recRoot->score = 0;
			recRoot->lv = -1;
			for(int i=0; i<HEIGHT; i++)
			{
				for(int j=0; j<WIDTH; j++)
					recRoot->f[i][j] = field[i][j];
			}
			modified_recommend(recRoot);
			DrawNextBlock(nextBlock);
			PrintScore(score);
			DrawField();
		}
	}
	timed_out =0;	// 0으로 초기화

}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
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
	return touched*10;

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
	DrawRecommend(recRoot->recBlockY, recRoot->recBlockX, recRoot->curBlockID, recRoot->recBlockRotate);
	DrawShadow(y,x,blockID, blockRotate);
	DrawBlock(y,x,blockID,blockRotate, ' ');
}

void createRankList(){
	int newScore;
	char newName[NAMELEN];
	FILE* fp = fopen("rank.txt", "r");
	if(fp == NULL)
		return;
	fscanf(fp, "%d", &N);
	for(int i=0; i < N ; i++)
	{
		fscanf(fp, "%s %d", newName, &newScore);
		Node* newNode = (Node*)malloc(sizeof(Node));
		strcpy(newNode->name, newName);
		newNode->score = newScore;
		newNode->next = NULL;
		if(head == NULL)
			head = tail =  newNode;
		else
		{
			tail->next = newNode;
			tail = newNode;
		}
	}
	fclose(fp);
}

void rank(){
	int X=1,Y=N, count, order,flag =0;
	char targetName[NAMELEN];
	int targetNum;
	Node* cur = head;
	clear();
	printw("1. List ranks from X to Y\n");
	printw("2. List ranks by a specific name\n");
	printw("3. Delete a specific rank X\n");
	order = wgetch(stdscr);
	switch(order)
	{
		case '1':
			printw("X: ");
			echo();
			scanw("%d", &X);
			printw("Y: ");
			scanw("%d", &Y);
			noecho();
			printw("      name      |\tscore\n");
			printw("-----------------------------\n");
			if(X<0 || Y< 0|| X>Y || X>N)
				printw("\nsearch failure: no rank in the list\n");
			else
			{
				count = 0;
				while(1)
				{
					count++;
					if(count >= X & count <=Y)
					{
						printw("%-16s| %d\n", cur->name, cur->score);
						cur = cur->next;
						if(cur == NULL) 
							break;
					}
					else
					{
						if(count > Y || cur->next == NULL)
							break;
						else
							cur = cur->next;
					}
				}
			}
			break;
		case '2':
			echo();
			printw("Input the name: ");
			scanw("%s", targetName);
			noecho();
			printw("      name      |\tscore\n");
			printw("-----------------------------\n");
			for(; cur; cur = cur->next)
			{
				if(!strcmp(cur->name, targetName))
				{
					flag =1;
					printw("%-16s| %d\n", cur->name, cur->score);
				}
			}
			if(flag == 0)
				printw("serach failure: no name in the list\n");

			break;
		case '3':
			echo();
			printw("Input the rank: ");
			scanw("%d", &targetNum);
			noecho();
			if(targetNum > N || targetNum<= 0 || N<0 )
				printw("\nsearch failure: the rank not in the list\n");
			else
			{
				Node* prev;
				count = 0;
				count++;
				for(; cur; cur = cur->next)
				{
					if(count == targetNum)
					{
						if(count == 1)
						{
							if(cur->next == NULL)
							{
								head = NULL;
								tail = NULL;
								free(cur);
								break;
							}
							else
							{
								head = cur->next;
								cur->next = NULL;
								free(cur);
								break;
							}
						}
						else if(count < N)
						{
							prev->next = cur->next;
							cur->next = NULL;
							free(cur);
							break;
						}
						else	// count == N
						{
							tail = prev;
							prev->next = NULL;
							free(cur);
							break;
						}
					}
					prev = cur;
					count++;
				}
				printw("\nresult: the rank deleted\n");
			}
			writeRankFile();
			break;
	}
	getch();

}
	



void writeRankFile(){
	Node* cur;
	FILE* fp = fopen("rank.txt", "w");
	fprintf(fp, "%d\n", N);
	for(cur = head; cur; cur = cur->next)
		fprintf(fp, "%s %d\n", cur->name, cur->score);

}

void newRank(int score){
	char playerName[NAMELEN];
	Node* newNode = (Node*)malloc(sizeof(Node));
	clear();
	printw("Your name: ");
	echo();
	scanw("%s", playerName);
	strcpy(newNode->name, playerName);
	noecho();
	newNode->score = score;
	newNode->next = NULL;
	if(head == NULL)
	{
		head = newNode;
		N=1;
	}
	else if(newNode->score > head->score)
	{
		newNode->next = head;
		head = newNode;
		N++;
	}
	else
	{
		Node* cur = head;
		while(1)
		{
			if(cur->next == NULL)
			{
				cur->next = newNode;
				N++;
				break;
			}
			else if(newNode->score <= cur->score && newNode->score > cur->next->score)
			{
				newNode->next = cur->next;
				cur->next = newNode;
				N++;
				break;
			}
			else
				cur = cur->next;
		}
	}
	writeRankFile();
					

}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	DrawBlock(y,x,blockID, blockRotate, 'R');
}
/*
int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int X,Y,i,j,k;
	int tempMax =0;
	int idx =0;
	RecNode** child = root->c;

	if(root->lv+1 == VISIBLE_BLOCK+1)
		return 0;
	for(i=0; i< RecRotate[nextBlock[root->lv+1]] ; i++)
	{
		for(X = -3; X<WIDTH; X++)
		{
			child[idx] = (RecNode*)malloc(sizeof(RecNode));
			child[idx]->lv = root->lv+1;
			child[idx]->curBlockID = nextBlock[root->lv+1];
			child[idx]->recBlockRotate = i;
			Y= -1;
			if(CheckToMove(root->f, nextBlock[root->lv+1],i,Y,X))
			{
				while(1)
				{
					if(!CheckToMove(root->f, nextBlock[root->lv+1],i,Y+1, X))
					break;
					Y++;
				}
				for(j=0; j< HEIGHT; j++)
				{
					for(k =0; k < WIDTH; k++)
						child[idx]->f[j][k] = root->f[j][k];
				}

				child[idx]->recBlockX = X;
				child[idx]->recBlockY = Y;
				child[idx]->score = root->score;
				child[idx]->score += AddBlockToField(child[idx]->f,nextBlock[root->lv+1],i,Y,X);
				child[idx]->score += DeleteLine(child[idx]->f);

				if(root->lv+1 == VISIBLE_BLOCK)
					max = child[idx]->score;
				else
				{
					tempMax = recommend(child[idx]);
					if( tempMax >= max )
					{
						if(tempMax==max)
						{
							if(recRoot->recBlockY > Y)
							{
								idx++;
								continue;
							}
						}
						max = tempMax;
						if(root->lv+1 == 0)
						{
							recRoot->curBlockID = nextBlock[root->lv+1];
							recRoot->recBlockX = X;
							recRoot->recBlockY = Y;
							recRoot->recBlockRotate = i;
						}
					}

				}
				idx++;
			}
		}
	}
	return max;
}
*/

int modified_recommend(RecNode* root)
{
	int max =0;
	int X,Y,i,j,k;
	int tempMax = 0;
	int prunMax = 0;
	int idx =0;
	RecNode** child = root->c;
	if(root->lv+1 == VISIBLE_BLOCK+1)
		return 0;
	for(i=0; i< RecRotate[nextBlock[root->lv+1]] ; i++)
	{
		for(X = -3; X< WIDTH; X++)
		{
			child[idx] = (RecNode*)malloc(sizeof(RecNode));
			child[idx]->lv = root->lv+1;
			child[idx]->curBlockID = nextBlock[root->lv+1];
			child[idx]->recBlockRotate = i;
			Y = -1;
			if(CheckToMove(root->f, nextBlock[root->lv+1], i, Y, X))
			{
				while(1)
				{
					if(!CheckToMove(root->f, nextBlock[root->lv+1], i, Y+1, X))
					break;
					Y++;
				}
				for(j=0; j<HEIGHT; j++)
				{
					for(k=0; k <WIDTH; k++)
						child[idx]->f[j][k] = root->f[j][k];
				}
				child[idx]->recBlockX = X;
				child[idx]->recBlockY = Y;
				child[idx]->score = root->score;
				child[idx]->score += AddBlockToField(child[idx]->f, nextBlock[root->lv+1],i,Y,X);
				child[idx]->score += DeleteLine(child[idx]->f);
				
				if(root->lv+1 == VISIBLE_BLOCK)
					max = child[idx]->score;
			
				if(child[idx]->score >= prunMax)
				{
					prunMax = child[idx]->score;
					if(root->lv+1 == VISIBLE_BLOCK)
						max = child[idx]->score;
					else
					{
						tempMax = modified_recommend(child[idx]);
						if( tempMax >= max)
						{
							if(tempMax ==max)
							{
								if(recRoot->recBlockY > Y)
								{
									idx++;
									continue;
								}
							}
							max = tempMax;
							if(root->lv+1 == 0)
							{
								recRoot->curBlockID=nextBlock[root->lv+1];
								recRoot->recBlockX =X;
								recRoot->recBlockY = Y;
								recRoot->recBlockRotate = i;
							}
						}
					}
				}
				idx++;
			}
		}
	}
	return max;
					
}
void recommendPlay(){
	int command;
	clear();
	act.sa_handler = recBlockDown;
	sigaction(SIGALRM, &act, &oact);
	InitTetris();
	do{
		if(timed_out == 0)
		{
			alarm(1);
			timed_out = 1;
		}

		command = GetCommand();
		if(command == QUIT)
		{
			alarm(0);
			DrawBox(HEIGHT/2-1, WIDTH/2-5, 1,10);
			move(HEIGHT/2, WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1, WIDTH/2-5, 1,10);
	move(HEIGHT/2, WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
}

void recBlockDown(int sig)
{
	int i,j;
	if(recRoot->recBlockY == -1) gameOver = 1;
	else
	{
		score += AddBlockToField(field, nextBlock[0], recRoot->recBlockRotate, recRoot->recBlockY, recRoot->recBlockX);
		score += DeleteLine(field);

		for(i=0; i<BLOCK_NUM; i++)
		{
			if(i == BLOCK_NUM-1)
				nextBlock[i] = rand() %7;
			else
				nextBlock[i] = nextBlock[i+1];
		}
		blockRotate = 0;
		blockY = -1;
		blockX = WIDTH/2-2;

		recRoot->curBlockID = nextBlock[0];
		recRoot->recBlockX = blockX;
		recRoot->recBlockY = blockY;
		recRoot->score = 0;
		recRoot->recBlockRotate = blockRotate;

		for(i=0; i<HEIGHT ; i++)
		{
			for(j=0; j<WIDTH; j++)
				recRoot->f[i][j] = field[i][j];
		}

		modified_recommend(recRoot);

		DrawNextBlock(nextBlock);
		PrintScore(score);
		DrawField();
		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	}
	timed_out =0;
}



