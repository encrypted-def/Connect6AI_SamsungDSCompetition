// Samsung Go Tournament Form C Connect6Algo (g++-4.8.3)

// <--------------- 이 Code를 수정하면  작동하지 않을 수 있습니다 ------------------>

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Connect6Algo.h"

unsigned s_time;
int terminateAI;
int width = 19, height = 19;
int cnt = 2;
int myColor;

static char cmd[256];
static HANDLE event1, event2;
#define BOARD_SIZE 20
int board[BOARD_SIZE][BOARD_SIZE];

static void getLine() {
	int c, bytes;

	bytes = 0;
	do {
		c = getchar();
		if (c == EOF) exit(0);
		if (bytes < sizeof(cmd)) cmd[bytes++] = (char)c;
	} while (c != '\n');
	cmd[bytes - 1] = 0;
	if (cmd[bytes - 2] == '\r') cmd[bytes - 2] = 0;
}

int setLine(char *fmt, ...) {
	int i;
	va_list va;
	va_start(va, fmt);
	i = vprintf(fmt, va);
	putchar('\n');
	fflush(stdout);
	va_end(va);
	return i;
}

static const char *getParam(const char *command, const char *input) {
	int n1, n2;
	n1 = (int)strlen(command);
	n2 = (int)strlen(input);
	if (n1 > n2 || _strnicmp(command, input, n1)) return NULL;
	input += strlen(command);
	while (isspace(input[0])) input++;	
	return input;
}

static void stop() {
	terminateAI = 1;
	WaitForSingleObject(event2, INFINITE);
}

static void start() {
	s_time = GetTickCount();
	stop();
}

static void turn() {
	terminateAI = 0;
	ResetEvent(event2);
	SetEvent(event1);
}

void domymove(int x[], int y[], int cnt) {
	mymove(x, y, cnt);
	if (cnt == 1)	
		setLine("%d,%d", x[0], y[0]);
	else
		setLine("%d,%d %d,%d", x[0], y[0], x[1], y[1]);
}

int showBoard(int x, int y) {
	return board[x][y];
}


static void doCommand() {
	const char *param;

	if ((param = getParam("START", cmd)) != 0) {
		start();
		init();
	}
	else if ((param = getParam("BEGIN", cmd)) != 0) {
		myColor = 1;
		cnt = 1;
		start();
		turn();
	}
	else if ((param = getParam("TURN", cmd)) != 0) {
		int x[2], y[2], r;
		if (((r = sscanf_s(param, "%d,%d %d,%d", &x[0], &y[0], &x[1], &y[1])) != 4 && r != 2)) {
			setLine("ERROR 형식에 맞지 않는 좌표가 입력되었습니다");
			return;
		}
		else {
			for (int i = 0; i < (r / 2); i++) {
				if (x[i] < 0 || x[i] >= width || y[i] < 0 || y[i] >= height) {
					setLine("ERROR 형식에 맞지 않는 좌표가 입력되었습니다");
					return;
				}
			}
			cnt = 2;
			opmove(x, y, r / 2);	
			turn();
		}
	}
	else if ((param = getParam("INFO", cmd)) != 0) {
		setLine("%s", info);
	}
	else if ((param = getParam("BLOCK", cmd)) != 0) {	
		int x, y;
		if (((sscanf_s(param, "%d,%d", &x, &y)) == 2)) {
			block(x, y);
			setLine("OK");
		}
	}
	else if ((param = getParam("QUIT", cmd)) != 0) {
		exit(0);
	}
}

static DWORD WINAPI threadLoop(LPVOID) {	
	while (1) {
		WaitForSingleObject(event1, INFINITE);
		myturn(cnt);
		if (cnt == 1) cnt = 2;	
		SetEvent(event2);
	}
}

int main() {
	DWORD mode;
	if (GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode))
		puts("직접 실행 불가능한 파일입니다. 육목 알고리즘 대회 툴을 이용해 실행하세요.");

	DWORD tid;
	event1 = CreateEvent(0, FALSE, FALSE, 0);
	CreateThread(0, 0, threadLoop, 0, 0, &tid);
	event2 = CreateEvent(0, TRUE, TRUE, 0);

	while (1) {
		getLine();
		doCommand();
	}

	return 0;
}

int isFree(int x, int y)
{
	return x >= 0 && y >= 0 && x < width && y < height && board[x][y] == 0;
}

void init() {	
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			board[i][j] = 0;
		}
	}
	setLine("OK");
}

void mymove(int x[], int y[], int cnt) {
	for (int i = 0; i < cnt; i++) {
		if (isFree(x[i], y[i])) {
			board[x[i]][y[i]] = 1;
		}
		else {
			setLine("ERROR 이미 돌이 있는 위치입니다. MY[%d, %d]", x[i], y[i]);
		}
	}
}

void opmove(int x[], int y[], int cnt) {
	for (int i = 0; i < cnt; i++) {
		if (isFree(x[i], y[i])) {
			board[x[i]][y[i]] = 2;
		}
		else {
			setLine("ERROR 이미 돌이 있는 위치입니다. OP[%d, %d]", x[i], y[i]);
		}
	}
	if(cnt == 2) // 상대가 두 수를 착수했을 경우에만
		RenewalOpponentMoves(x[0], x[1], y[0], y[1]); // 상대방의 착수를 갱신
}

void block(int x, int y) {
	if (isFree(x, y)) {
		board[x][y] = 3;
	}
}

