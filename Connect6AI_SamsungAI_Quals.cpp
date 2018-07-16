// Samsung Go Tournament Form C (g++-4.8.3)
 
/*
[AI 코드 작성 방법]
1. char info[]의 배열 안에                    "TeamName:자신의 팀명,Department:자신의 소속"                    순서로 작성합니다.
( 주의 ) Teamname:과 Department:는 꼭 들어가야 합니다.
"자신의 팀명", "자신의 소속"을 수정해야 합니다.
2. 아래의 myturn() 함수 안에 자신만의 AI 코드를 작성합니다.
3. AI 파일을 테스트 하실 때는 "육목 알고리즘대회 툴"을 사용합니다.
4. 육목 알고리즘 대회 툴의 연습하기에서 바둑돌을 누른 후, 자신의 "팀명" 이 들어간 알고리즘을 추가하여 테스트 합니다.
[변수 및 함수]
myturn(int cnt) : 자신의 AI 코드를 작성하는 메인 함수 입니다.
int cnt (myturn()함수의 파라미터) : 돌을 몇 수 둬야하는지 정하는 변수, cnt가 1이면 육목 시작 시  한 번만  두는 상황(한 번), cnt가 2이면 그 이후 돌을 두는 상황(두 번)
int  x[0], y[0] : 자신이 둘 첫 번 째 돌의 x좌표 , y좌표가 저장되어야 합니다.
int  x[1], y[1] : 자신이 둘 두 번 째 돌의 x좌표 , y좌표가 저장되어야 합니다.
void domymove(int x[], int y[], cnt) : 둘 돌들의 좌표를 저장해서 출력
//int board[BOARD_SIZE][BOARD_SIZE]; 바둑판 현재상황 담고 있어 바로사용 가능함. 단, 원본데이터로 수정 절대금지
// 놓을수 없는 위치에 바둑돌을 놓으면 실격패 처리.
boolean ifFree(int x, int y) : 현재 [x,y]좌표에 바둑돌이 있는지 확인하는 함수 (없으면 true, 있으면 false)
int showBoard(int x, int y) : [x, y] 좌표에 무슨 돌이 존재하는지 보여주는 함수 (1 = 자신의 돌, 2 = 상대의 돌, 3 = 블럭킹)
<-------AI를 작성하실 때, 같은 이름의 함수 및 변수 사용을 권장하지 않습니다----->
*/
 
#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <tuple>
#include <time.h>
#include "Connect6Algo.h"
using namespace std;
typedef pair<int, int> POSITION;
typedef pair<POSITION, POSITION> MOVES;
typedef pair<MOVES, double> MOVES_SCORE;
#define X first
#define Y second
#define BOARD_ROW 19
#define BOARD_COL 19
#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define BLOCK 3
#define MARK 4
 
// "샘플코드[C]"  -> 자신의 팀명 (수정)
// "AI부서[C]"  -> 자신의 소속 (수정)
// 제출시 실행파일은 반드시 팀명으로 제출!
char info[] = { "TeamName:자본주의가낳은AI,Department:고려대학교" };
 
//(x, y)가 보드 내에 위치하는지 아닌지 확인하는 함수
bool IsOutOfBounds(int x, int y) {
    if (0 <= x && x < BOARD_ROW && 0 <= y && y < BOARD_COL)
        return false;
    return true;
}
 
void print_POSITION(POSITION tmp) {
    printf("(%d, %d)\n", tmp.X, tmp.Y);
}
void print_MOVES(MOVES tmp2) {
    printf("(%d, %d), (%d, %d)\n", tmp2.first.X, tmp2.first.Y, tmp2.second.X, tmp2.second.Y);
}
void print_BOARD(int myBoard[][BOARD_COL]) {
    for (int x = 0; x < BOARD_ROW; x++) {
        for (int y = 0; y < BOARD_COL; y++)
            printf("%d ", myBoard[x][y]);
        printf("\n");
    }
}
// player(=BLACK or WHITE)가 board에 myMove를 착수했을 때 6목이 있는지 없는지 판단하는 함수. board는 갱신된 상태로 넘어옴
bool Is_Conn6(int myBoard[][BOARD_COL], MOVES myMove, int player) {
    POSITION tmp[2] = { myMove.first, myMove.second };
    for (int iter = 0; iter < 2; iter++) {
        int x = tmp[iter].X;
        int y = tmp[iter].Y; // 착수한 두 점에 대해 4방향으로 살펴보며 6목이 생성되었는지 확인. 확인하는 방법은 착수한 점을 기준으로 양 방향으로 player의 말이 몇개가 연속하는지 확인
        int dx[4] = { 1, 1, 0, 1 };
        int dy[4] = { 1, 0, 1, -1 };
        for (int dir = 0; dir < 4; dir++) {
            int cnt = 1;
            int curx = x + dx[dir];
            int cury = y + dy[dir];
            while (!IsOutOfBounds(curx, cury) && myBoard[curx][cury] == player) {
                cnt++;
                curx += dx[dir];
                cury += dy[dir];
            } // dx, dy 방향대로 진행
            curx = x - dx[dir];
            cury = y - dy[dir];
            while (!IsOutOfBounds(curx, cury) && myBoard[curx][cury] == player) {
                cnt++;
                curx -= dx[dir];
                cury -= dy[dir];
            } // dx, dy 반대 방향으로 진행
            if (cnt == 6) // 정확히 6개의 돌이 연속했을 경우
                return true;
        }
    }
    return false;
}
 
// board_source를 board_dest에 deep copy함
void DeepCopy_Board(int board_source[][BOARD_COL], int board_dest[][BOARD_COL]) {
    for (int x = 0; x < BOARD_ROW; x++) {
        for (int y = 0; y < BOARD_COL; y++) {
            board_dest[x][y] = board_source[x][y];
        }
    }
}
 
// player가 상대의 승리를 막기 위해 반드시 둬야 할 돌의 최소 갯수(=Threat의 갯수)를 구함. 적으면 적을수록 player에게 유리한 것임
int Count_Threat(int myBoard[][BOARD_COL], int player) {
    int opponent = 3 - player;
    int threat_cnt = 0;
    int board_copy[BOARD_ROW][BOARD_COL];
    DeepCopy_Board(myBoard, board_copy); // board_copy에 myBoard를 copy
    int dx[4] = { -1, 0, 1, 1 };
    int dy[4] = { 1, 1, 1, 0 };
    for (int x = 0; x < BOARD_ROW; x++) {
        for (int y = 0; y < BOARD_COL; y++) { // 각 (x, y)에 대해 dir 방향으로 진행했을 때 threat window 안에 4개 이상의 opponent 돌이 존재하고, 인접한 돌이 opponent 돌이 아니고, player 돌이나 mark가 존재하지 않을 경우를 찾음
            for (int dir = 0; dir < 4; dir++) {
                if (IsOutOfBounds(x + 5 * dx[dir], y + 5 * dy[dir]))
                    continue;
                int opponentStone = 0;
                for (int i = 0; i < 6; i++) { // threat window 안의 6개 돌을 살펴보는 중
                    if (board_copy[x + i*dx[dir]][y + i*dy[dir]] == EMPTY) // 빈 칸일 경우
                        continue;
                    else if (board_copy[x + i*dx[dir]][y + i*dy[dir]] == opponent) // opponent 돌이 놓여있을 경우
                        opponentStone++;
                    else { // player 돌이 놓여있거나 mark가 놓여있을 경우
                        opponentStone = 0;
                        break;
                    }
                }
                if (opponentStone >= 4 && (IsOutOfBounds(x - dx[dir], y - dy[dir]) || board_copy[x - dx[dir]][y - dy[dir]] != opponent) && (IsOutOfBounds(x + 6 * dx[dir], y + 6 * dy[dir]) || board_copy[x + 6 * dx[dir]][y + 6 * dy[dir]] != opponent)) { // threat window 안에 4개 이상의 player 돌이 존재하고, 상대방 혹은 mark 돌이 없으며 threat window와 인접한 2개의 돌이 opponent의 돌이 아닐때(7목 8목 방지용)
                    threat_cnt++;
                    for (int i = 0; i < 6; i++) {
                        if (board_copy[x + i*dx[dir]][y + i*dy[dir]] == EMPTY)
                            board_copy[x + i*dx[dir]][y + i*dy[dir]] = MARK; // 빈 칸을 MARK로 변경
                    }
                }
            }
        }
    }
    return threat_cnt;
}
 
// 상대의 승리를 막기 위해 반드시 두어야하는 자리들의 목록을 반환.
// (만약 Threat의 수가 1일 경우 {{x, y}, {-1, -1}}을 반환. threat_cnt에는 threat의 수를 기록할 것임
// board에 opponent의 6목은 없다고 가정.(있을 경우 밑의 tmp 부분에서 오류 발생함)
vector<MOVES> Get_ForcedMove(int myBoard[][BOARD_COL], int player, int* threat_cnt) {
    int opponent = 3 - player;
    *threat_cnt = 0;
    int board_copy[BOARD_ROW][BOARD_COL];
    DeepCopy_Board(myBoard, board_copy);
    int dx[4] = { -1, 0, 1, 1 };
    int dy[4] = { 1, 1, 1, 0 };
    vector<MOVES> ForcedMove;
    vector<MOVES> ThreatList;
    for (int x = 0; x < BOARD_ROW; x++) {
        for (int y = 0; y < BOARD_COL; y++) { // 각 (x, y)에 대해 dir 방향으로 진행했을 때 threat window 안에 4개 이상의 opponent 돌이 존재하고, 인접한 돌이 opponent 돌이 아니고, player 돌이나 mark가 존재하지 않을 경우를 찾음
            for (int dir = 0; dir < 4; dir++) {
                if (IsOutOfBounds(x + 5 * dx[dir], y + 5 * dy[dir]))
                    continue;
                int opponentStone = 0;
                for (int i = 0; i < 6; i++) { // threat window 안의 6개 돌을 살펴보는 중
                    if (board_copy[x + i*dx[dir]][y + i*dy[dir]] == EMPTY) // 빈 칸일 경우
                        continue;
                    else if (board_copy[x + i*dx[dir]][y + i*dy[dir]] == opponent) // opponent 돌이 놓여있을 경우
                        opponentStone++;
                    else { // player 돌이 놓여있거나 mark가 놓여있을 경우
                        opponentStone = 0;
                        break;
                    }
                }
                if (opponentStone >= 4 && (IsOutOfBounds(x - dx[dir], y - dy[dir]) || board_copy[x - dx[dir]][y - dy[dir]] != opponent) && (IsOutOfBounds(x + 6 * dx[dir], y + 6 * dy[dir]) || board_copy[x + 6 * dx[dir]][y + 6 * dy[dir]] != opponent)) { // threat window 안에 4개 이상의 player 돌이 존재하고, 상대방 혹은 mark 돌이 없으며 threat window와 인접한 2개의 돌이 opponent의 돌이 아닐때(7목 8목 방지용)
                    (*threat_cnt)++;
                    if ((*threat_cnt) > 2) { // 내가 막아야할 자리가 3곳 이상인 상태니 어차피 졌음. 시간 아까우니 그냥 바로 return
                        ForcedMove.push_back({ ThreatList[0].first, ThreatList[1].first });
                        return ForcedMove;
                    }
                    int tmp[4] = { -1, -1, -1, -1 }; // ThreatList에 파싱할 것임
                    for (int i = 0; i < 6; i++) {
                        if (board_copy[x + i*dx[dir]][y + i*dy[dir]] == EMPTY) {
                            board_copy[x + i*dx[dir]][y + i*dy[dir]] = MARK; // 빈 칸을 MARK로 변경
                            if (tmp[0] == -1) {
                                tmp[0] = x + i*dx[dir];
                                tmp[1] = y + i*dy[dir];
                            }
                            else {
                                tmp[2] = x + i*dx[dir];
                                tmp[3] = y + i*dy[dir];
                            }
                        } // tmp에 현재 threat window의 좌표를 기록해둠.(threat window 내 빈칸이 1칸일 경우 끝에는 -1, -1이 들어감)
                    }
                    ThreatList.push_back({ { tmp[0], tmp[1] },{ tmp[2], tmp[3] } }); // ThreatList에 현재 threat window에 대한 threat을 삽입
                }
            }
        }
    }
    if ((*threat_cnt) == 0) // threat이 없으면
        return ForcedMove; // 아무것도 안채워넣은걸 그냥 반환
    if ((*threat_cnt) == 1) { // threat이 1개이면
        myBoard[ThreatList[0].first.X][ThreatList[0].first.Y] = player;
        if (Count_Threat(myBoard, player) == 0) // threat 후보 중에서 하나를 메꾸니 Threat이 없어졌다면 이는 올바른 착수
            ForcedMove.push_back({ ThreatList[0].first,{ -1, -1 } });
        myBoard[ThreatList[0].first.X][ThreatList[0].first.Y] = EMPTY;
        if (ThreatList[0].second.X == -1) // 해당 threat window에 threat이 1개 있었을 경우
            return ForcedMove;
        myBoard[ThreatList[0].second.X][ThreatList[0].second.Y] = player;
        if (Count_Threat(myBoard, player) == 0) // threat 후보 중에서 하나를 메꾸니 Threat이 없어졌다면 이는 올바른 착수
            ForcedMove.push_back({ ThreatList[0].second,{ -1, -1 } });
        myBoard[ThreatList[0].second.X][ThreatList[0].second.Y] = EMPTY;
        return ForcedMove;
    }
    if ((*threat_cnt) == 2) { // threat이 2개이면
        POSITION tmp1[2] = { ThreatList[0].first, ThreatList[0].second };
        POSITION tmp2[2] = { ThreatList[1].first, ThreatList[1].second };
        for (int i = 0; i < 2; i++) {
            if (tmp1[i].X == -1)
                continue;
            for (int j = 0; j < 2; j++) {
                if (tmp2[j].X == -1)
                    continue;
                myBoard[tmp1[i].X][tmp1[i].Y] = player;
                myBoard[tmp2[j].X][tmp2[j].Y] = player;
                if (Count_Threat(myBoard, player) == 0) // threat 후보 중에서 하나를 메꾸니 Threat이 없어졌다면 이는 올바른 착수
                    ForcedMove.push_back({ tmp1[i], tmp2[j] });
                myBoard[tmp1[i].X][tmp1[i].Y] = EMPTY;
                myBoard[tmp2[j].X][tmp2[j].Y] = EMPTY; // 원상복귀
            }
        }
        return ForcedMove;
    }
    return ForcedMove; // 사실 Unreachable 코드
}
 
// 놓으면 6목을 만들 수 있는 수가 있는지 확인. 있으면 보드를 갱신하고 6목을 만드는 수를 반환, 없으면 {{-1,-1},{-1,-1}}을 반환
MOVES Find_Conn6Move(int myBoard[][BOARD_COL], int player) {
    int dx[4] = { -1, 0, 1, 1 };
    int dy[4] = { 1, 1, 1, 0 };
    for (int x = 0; x < BOARD_ROW; x++) {
        for (int y = 0; y < BOARD_COL; y++) { // 각 (x, y)에 대해 dir 방향으로 진행했을 때 threat window 안에 4개 이상의 player 돌이 존재하고, 인접한 돌이 player 돌이 아니고, 상대 돌이나 mark가 존재하지 않을 경우를 찾음
            for (int dir = 0; dir < 4; dir++) {
                if (IsOutOfBounds(x + 5 * dx[dir], y + 5 * dy[dir]))
                    continue;
                int playerStone = 0;
                for (int i = 0; i < 6; i++) { // threat window 안의 6개 돌을 살펴보는 중
                    if (myBoard[x + i*dx[dir]][y + i*dy[dir]] == EMPTY) // 빈 칸일 경우
                        continue;
                    else if (myBoard[x + i*dx[dir]][y + i*dy[dir]] == player) // player 돌이 놓여있을 경우
                        playerStone++;
                    else { // 상대방의 돌이 놓여있을 경우
                        playerStone = 0;
                        break;
                    }
                }
                if (playerStone >= 4 && (IsOutOfBounds(x - dx[dir], y - dy[dir]) || myBoard[x - dx[dir]][y - dy[dir]] != player) && (IsOutOfBounds(x + 6 * dx[dir], y + 6 * dy[dir]) || myBoard[x + 6 * dx[dir]][y + 6 * dy[dir]] != player)) { // threat window 안에 4개 이상의 player 돌이 존재하고, 상대방 돌이 없으며 threat window와 인접한 2개의 돌이 player의 돌이 아닐때(7목 8목 방지용)
                    POSITION tmp[2] = { { 0, 0 },{ 0, 0 } };
                    int idx = 0;
                    for (int i = 0; i < 6; i++) {
                        if (myBoard[x + i*dx[dir]][y + i*dy[dir]] == EMPTY) {
                            myBoard[x + i*dx[dir]][y + i*dy[dir]] = player;
                            tmp[idx++] = { x + i*dx[dir], y + i*dy[dir] };
                        }
                    }
                    if (idx == 1) { // 5개의 player 돌로 채워진 곳이라 한 개의 돌은 다른 곳에 착수해야하는 상황일 때
                        for (int i = 0; i < BOARD_ROW; i++) {
                            for (int j = 0; j < BOARD_COL; j++) {
                                if (myBoard[i][j] != EMPTY) // 빈칸이 아닐 경우
                                    continue;
                                if (i == x - dx[dir] && j == y - dy[dir]) // 6목을 깨버릴 경우
                                    continue;
                                if (i == x + 6 * dx[dir] && j == y + dx[dir]) // 6목을 꺠버릴 경우
                                    continue;
                                tmp[1] = { i, j };
                                return { tmp[0], tmp[1] };
                            }
                        }
                        // 여기에 도달했다는 것은 한 개의 돌을 어디에 두더라도 승리조건이 위배된다는 의미.
                        return{ { -1,-1 },{ -1,-1 } };
                    }
                    return { tmp[0], tmp[1] }; // 빈칸을 반환. 여기에 착수하면 됨
                }
            }
        }
    }
    return{ { -1, -1 },{ -1, -1 } };
}
 
// 지금 두는 수(두 수)의 Score를 반환
double Get_ScoreOfSingleMove(int myBoard[][BOARD_COL], POSITION myMove, int player) {
    double PlayerFactor[6] = { 0.0, 1.0, 2.0, 6.0, 0.0, 0.0 }; // PlayerFactor[i] : 나의 착수로 window 안에 나의 돌 i개를 만들었을 때 score. 검은돌 4/5개는 어차피 Threat에서 알아서 카운트될테니 계산할 필요 없음
    double OpponentFactor[6] = { 0.0, 1.05, 5.0, 11.05, 0.0, 0.0 }; // OpponentFacotr[i] : 나의 착수로 window 안에 상대 돌 i개를 저지했을 때 score. 하얀돌 4/5개를 막는건 어차피 Threat에서 걸러지므로 따로 score를 부여할 필요 없음
    int opponent = 3 - player;
    double score = 0.0;
    int board_copy[BOARD_ROW][BOARD_COL];
    DeepCopy_Board(myBoard, board_copy);
    board_copy[myMove.X][myMove.Y] = player; // Threat의 수를 쉽게 구하기 위해 일단 착수시켜둠
    int dx[4] = { 1, 0, 1, 1 };
    int dy[4] = { 0, 1, 1, -1 };
    for (int dir = 0; dir < 4; dir++) {
        for (int i = 0; i < 6; i++) {
            int x = myMove.X - i * dx[dir];
            int y = myMove.Y - i * dy[dir];
            if (IsOutOfBounds(x, y) || IsOutOfBounds(x + 5 * dx[dir], y + 5 * dy[dir]))
                continue; // 6칸이 다 보드 안에 있지 않는 threat window는 그냥 버림
            bool isThreat = true;
            int PlayerStoneCnt = 0; // 6칸 내에 player 돌의 갯수(최소 1이어야 함, 0일 경우 6칸 내에 상대 돌이나 Block이 있는 경우
            int OpponentStoneCnt = 0;
            for (int i = 0; i < 6; i++) {
                if (board_copy[x + i*dx[dir]][y + i*dy[dir]] == BLOCK) { // Block이 있는 경우
                    PlayerStoneCnt = 0;
                    OpponentStoneCnt = 0; // Player, Opponent 둘 다 가능성이 없음
                    isThreat = false;
                    break;
                }
                else if (board_copy[x + i*dx[dir]][y + i*dy[dir]] == opponent) { // 상대 돌인 경우
                    OpponentStoneCnt++;
                    isThreat = false;
                }
                else if (board_copy[x + i*dx[dir]][y + i*dy[dir]] == player) // 내 돌인 경우
                    PlayerStoneCnt++;
                else if (board_copy[x + i*dx[dir]][y + i*dy[dir]] == MARK) // MARK가 놓여있는 경우(계속 진행은 하지만 Threat은 아님)
                    isThreat = false;
                // EMPTY가 놓여있는 경우 따로 처리해줄게 없음
            }
            if (isThreat && PlayerStoneCnt == 4 && (IsOutOfBounds(x - dx[dir], y - dy[dir]) || myBoard[x - dx[dir]][y - dy[dir]] != player) && (IsOutOfBounds(x + 6 * dx[dir], y + 6 * dy[dir]) || myBoard[x + 6 * dx[dir]][y + 6 * dy[dir]] != player)) { // 상대에게 Threat을 생성했으면서 window의 양 끝이 player의 돌이 아닌경우(7,8목 방지용). 갯수가 4일때만 취급하는 이유는, 5일때는 그 수를 두지 않아도 이미 Threat이라는 소리니 굳이 쳐줄 필요가 없음                    
                score += 10000.0;
                for (int i = 0; i < 6; i++) {
                    if (board_copy[x + i*dx[dir]][y + i*dy[dir]] == EMPTY)
                        board_copy[x + i*dx[dir]][y + i*dy[dir]] = MARK; // 비어있는 칸에 mark를 둠
                }
            }
            if (OpponentStoneCnt == 0 && (IsOutOfBounds(x - dx[dir], y - dy[dir]) || board_copy[x - dx[dir]][y - dy[dir]] != player) && (IsOutOfBounds(x + 6 * dx[dir], y + 6 * dy[dir]) || board_copy[x + 6 * dx[dir]][y + 6 * dy[dir]] != player)) // 상대돌이 없을 때
                score += PlayerFactor[PlayerStoneCnt];
            if (PlayerStoneCnt == 1 && (IsOutOfBounds(x - dx[dir], y - dy[dir]) || board_copy[x - dx[dir]][y - dy[dir]] != opponent) && (IsOutOfBounds(x + 6 * dx[dir], y + 6 * dy[dir]) || board_copy[x + 6 * dx[dir]][y + 6 * dy[dir]] != opponent)) // 내 돌이 없는(자기자신때문에 값은 1) window일 경우.(즉 상대의 6목을 저지했음)
                score += OpponentFactor[OpponentStoneCnt];
 
        }
    }
    return score;
}
 
double Get_ScoreOfDoubleMoves(int myBoard[][BOARD_COL], MOVES myMoves, int player) {
    double score = 0.0;
    score += Get_ScoreOfSingleMove(myBoard, myMoves.first, player);
    myBoard[myMoves.first.X][myMoves.first.Y] = player; // 첫번째 착점하는 곳을 player로 채워넣고 두번째 착수에 대한 score를 합산
    score += Get_ScoreOfSingleMove(myBoard, myMoves.second, player);
    myBoard[myMoves.first.X][myMoves.first.Y] = EMPTY; // 다시 되돌림
    return score;
}
 
// 최선의 수를 반환(한개짜리)
POSITION Find_BestSingleMove(int myBoard[][BOARD_COL], int player) {
    double maxscore = -1.0;
    POSITION bestmove = { -1, -1 };
    for (int x = 0; x < BOARD_ROW; x++) {
        for (int y = 0; y < BOARD_COL; y++) {
            if (myBoard[x][y] != EMPTY)
                continue;
            double tmpScore = Get_ScoreOfSingleMove(myBoard, { x, y }, player);
            if (tmpScore >= maxscore && abs(BOARD_ROW / 2 - x) + abs(BOARD_COL / 2 - y) < abs(BOARD_ROW / 2 - bestmove.X) + abs(BOARD_COL / 2 - bestmove.Y)) {
                bestmove = { x, y };
                maxscore = tmpScore;
            }
        }
    }
    return bestmove;
}
 
//최선의 MOVES 후보를 반환(최대 N개), N은 5이상
vector<MOVES_SCORE> Find_CandidateOfBestDoubleMoves(int myBoard[][BOARD_COL], int player, int N) {
    vector<MOVES_SCORE> candidate(N, { { { -1, -1 },{ -1, -1 } }, -1000000.0 });
    int opponent = 3 - player;
    MOVES bestmove = Find_Conn6Move(myBoard, player);
    if (bestmove.first.X >= 0) { // 6목을 만드는 적절한 착수가 존재할 경우
        candidate[0] = { bestmove, 100000.0 };
        return candidate; // 더 살펴볼 필요 없이 최선의 수를 바로 반환
    }
    vector<MOVES> forcedmove;
    int player_threat;
    forcedmove = Get_ForcedMove(myBoard, player, &player_threat);
    if (player_threat >= 3) { // 막아야할 곳이 세 곳 이상이면(나의 Threat이 3이상이면)
        candidate[0] = make_pair(forcedmove[0], -1000000.0);
        return candidate; // 어차피 승산이 없으므로 아무값이나 반환함    
    }
    if (player_threat == 2) { // 둘 곳이 굉장히 한정됨. 그 한정된 자리들을 대상으로 Score를 확인해 가장 점수가 높은 곳을 찾음
        for (auto const& candidateMoves : forcedmove) {
            double tmpScore = Get_ScoreOfDoubleMoves(myBoard, candidateMoves, player);
            int idx = N - 1;
            while (idx >= 0 && tmpScore > candidate[idx].second)
                idx--;
            if (idx == N - 1) // N번째보다 score가 작을 경우
                continue;
            for (int i = N - 2; i >= idx + 1; i--)
                candidate[i + 1] = candidate[i]; // (idx+1)번째에 삽입할 수 있도록 공간 확보
            candidate[idx + 1] = { candidateMoves, tmpScore };
        }
    }
    if (player_threat == 1) { // 고정적으로 둬야할 자리가 1개 있고 그것의 종류가 1개 혹은 2개이므로 고정적으로 둬야할 자리를 고정해두고 나머지 빈칸을 전수조사 해서 찾을 것임
        for (auto const& candidateMoves : forcedmove) {
            for (int x = 0; x < BOARD_ROW; x++) {
                for (int y = 0; y < BOARD_COL; y++) {
                    if (myBoard[x][y] != EMPTY || (x == candidateMoves.first.X && y == candidateMoves.first.Y)) // 비어있지 않은 칸이거나 x, y가 이미 1개 둔 자리에 겹친 경우
                        continue;
                    MOVES tmpMoves = { candidateMoves.first,{ x, y } };
                    double tmpScore = Get_ScoreOfDoubleMoves(myBoard, tmpMoves, player);
 
                    int idx = N - 1;
                    while (idx >= 0 && tmpScore > candidate[idx].second)
                        idx--;
                    if (idx == N - 1) // N번째보다 score가 작을 경우
                        continue;
                    for (int i = N - 2; i >= idx + 1; i--)
                        candidate[i + 1] = candidate[i]; // (idx+1)번째에 삽입할 수 있도록 공간 확보
                    candidate[idx + 1] = { tmpMoves, tmpScore };
                }
            }
        }
    }
    if (player_threat == 0) { // 모든 빈칸(대략 300개)에 대해, 300C2 가지를 전부 따져보면 가장 좋지만 너무 시간을 많이 잡아먹으므로 우선 한 개의 돌에 대해 착점해서 점수가 높은 상위 High_N개에 대해서 2개씩 매칭해 조사를 할 것임
        int High_N = 20;
        vector<POSITION> highScoredPosition(High_N, { -1, -1 });
        vector<double> highScore(High_N, -1.0);
        for (int x = 0; x < BOARD_ROW; x++) {
            for (int y = 0; y < BOARD_COL; y++) {
                if (myBoard[x][y] != EMPTY) // 빈칸이 아닐 경우 스킵
                    continue;
                double currentScore = Get_ScoreOfSingleMove(myBoard, { x, y }, player);
                int idx = High_N - 1;
                while (idx >= 0 && currentScore > highScore[idx])
                    idx--;
                if (idx == High_N - 1) // High_N번째보다 작을 경우
                    continue;
                for (int i = High_N - 2; i >= idx + 1; i--) {
                    highScore[i + 1] = highScore[i];
                    highScoredPosition[i + 1] = highScoredPosition[i]; // (idx+1)번째에 {x, y}를 삽입할 수 있도록 공간확보
                }
                highScore[idx + 1] = currentScore;
                highScoredPosition[idx + 1] = { x, y };
            }
        }
 
 
        for (int i = 0; i < High_N / 2; i++) {
            for (int j = i + 1; j < High_N; j++) {
                double tmpScore = Get_ScoreOfDoubleMoves(myBoard, { highScoredPosition[i], highScoredPosition[j] }, player);
                int idx = N - 1;
                while (idx >= 0 && tmpScore > candidate[idx].second)
                    idx--;
                if (idx == N - 1) // N번째보다 score가 작을 경우
                    continue;
                for (int i = N - 2; i >= idx + 1; i--)
                    candidate[i + 1] = candidate[i]; // (idx+1)번째에 삽입할 수 있도록 공간 확보
                candidate[idx + 1] = { { highScoredPosition[i], highScoredPosition[j] }, tmpScore };
            }
        }
    }
    return candidate;
}
 
MOVES_SCORE Find_BestDoubleMovesByDepthSearch(int myboard[][BOARD_COL], int player, int currentDepth, int maxDepth, long st_time) {
    MOVES_SCORE tmpMax = { { { -1,-1 },{ -1,-1 } }, -10000000.0 };
    int Breadth = 4;
    vector<MOVES_SCORE> candidate = Find_CandidateOfBestDoubleMoves(myboard, player, Breadth);
    if (candidate[0].second < -50000.0) // 가장 최선인 수 조차 필패일 경우
        return candidate[0];
    if (candidate[0].second >= 50000.0) // 가장 최선의 수를 두면 바로 6목이 되는 경우
        return candidate[0];
    if (currentDepth == maxDepth) // 최대 깊이까지 내려갔으면
        return candidate[0]; // 더 깊게 들어갈 필요가 없음
    if (1.0 * (clock() - st_time) / (CLOCKS_PER_SEC) > 6.2) // 6.2초이상 지났으면
        return candidate[0]; // 더 파고들 시간이 없음
    int cnt = 0;
    for (auto const& candidateMoves_Score : candidate) { // 이제 필승/필패는 없음. 후보들을 대상으로 depth를 한 칸 더 깊게 가볼 예정
        if (candidateMoves_Score.first.first.X == -1) // 잘못된 값일경우
            break; // 이 이후로는 전부 (-1, -1), (-1, -1)이 기록되어있으므로 넘김
        MOVES tmp = candidateMoves_Score.first;
        myboard[tmp.first.X][tmp.first.Y] = player;
        myboard[tmp.second.X][tmp.second.Y] = player;
        MOVES_SCORE currentBest = Find_BestDoubleMovesByDepthSearch(myboard, 3 - player, currentDepth + 1, maxDepth, st_time);
        if (tmpMax.second < candidateMoves_Score.second - currentBest.second)
            tmpMax = { tmp, candidateMoves_Score.second - currentBest.second };
        myboard[tmp.first.X][tmp.first.Y] = EMPTY;
        myboard[tmp.second.X][tmp.second.Y] = EMPTY;
    }
    return tmpMax;
}
 
void myturn(int cnt) {
    long st_time = clock();
    int myBoard[BOARD_ROW][BOARD_COL];
    for (int i = 0; i < BOARD_ROW; i++) {
        for (int j = 0; j < BOARD_COL; j++) {
            myBoard[i][j] = showBoard(i, j);
        }
    } // myBoard에 board 상태를 저장
 
    int x[2], y[2];
 
    // 이 부분에서 알고리즘 프로그램(AI)을 작성하십시오. 기본 제공된 코드를 수정 또는 삭제하고 본인이 코드를 사용하시면 됩니다.
    // 현재 Sample code의 AI는 Random으로 돌을 놓는 Algorithm이 작성되어 있습니다.
    if (cnt == 1) {
        POSITION myMove = Find_BestSingleMove(myBoard, 1);
        x[0] = myMove.X;
        x[1] = 0;
        y[0] = myMove.Y;
        y[1] = 0;
    }
    else {
        MOVES_SCORE myMove = Find_BestDoubleMovesByDepthSearch(myBoard, 1, 0, 5, st_time);
        x[0] = myMove.first.first.X;
        x[1] = myMove.first.second.X;
        y[0] = myMove.first.first.Y;
        y[1] = myMove.first.second.Y;
    }
 
    // 이 부분에서 자신이 놓을 돌을 출력하십시오.
    // 필수 함수 : domymove(x배열,y배열,배열크기)
    // 여기서 배열크기(cnt)는 myturn()의 파라미터 cnt를 그대로 넣어야합니다.
    domymove(x, y, cnt);
}
