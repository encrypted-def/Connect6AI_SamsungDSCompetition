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
#include <random>
#include <functional>
#include <time.h>
using namespace std;
typedef pair<int, int> POSITION;
typedef pair<POSITION, POSITION> MOVES;
typedef pair<MOVES, double> MOVES_SCORE;
typedef tuple<double, double, double, double, double, double> FACTOR;
typedef pair<int, FACTOR> FACTOR_SCORE;
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
char info[] = { "TeamName:white,Department:고려대학교" };
MOVES CurrentOpponentMoves = { { -1, -1 },{ -1, -1 } }; // opmove가 실행된 후 상대방의 착수를 저장하는 변수
MOVES CurrentMyMoves = { { -1, -1 },{ -1, -1 } }; // opmove가 실행된 후 상대방의 착수를 저장하는 변수
 
void RenewalOpponentMoves(int x0, int x1, int y0, int y1) {
    CurrentOpponentMoves = { { x0, y0 },{ x1, y1 } };
}
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
    for (int x = 0; x < BOARD_ROW; x++)
        for (int y = 0; y < BOARD_COL; y++)
            board_dest[x][y] = board_source[x][y];
}
 
// player의 착수로 Threat을 전부 없앴는지 확인하는 함수
// Get_ForcedMove에서 사용하는 함수
// 만약 남아있는 Threat이 있다면 반드시 check를 포함하고 있으므로 check 주변만 확인하면 됨
bool IsNoThreat(int myBoard[][BOARD_COL], MOVES check, int player) {
    int opponent = 3 - player;
    int threat_cnt = 0;
    int board_copy[BOARD_ROW][BOARD_COL];
    DeepCopy_Board(myBoard, board_copy); // board_copy에 myBoard를 copy
    int dx[4] = { -1, 0, 1, 1 };
    int dy[4] = { 1, 1, 1, 0 };
    POSITION tmp[2] = { check.first, check.second };
    for (int iter = 0; iter < 2; iter++) {
        if (tmp[iter].X == -1) // (-1, -1)이라면
            continue;
        int st_x = tmp[iter].X;
        int st_y = tmp[iter].Y;
        for (int dir = 0; dir < 4; dir++) {
            for (int i = 0; i < 6; i++) {
                int x = st_x - dx[dir] * i;
                int y = st_y - dy[dir] * i;
                if (IsOutOfBounds(x, y) || IsOutOfBounds(x + 5 * dx[dir], y + 5 * dy[dir]))
                    continue;
                int opponentStone = 0;
                for (int j = 0; j < 6; j++) { // threat window 안의 6개 돌을 살펴보는 중
                    if (board_copy[x + j*dx[dir]][y + j*dy[dir]] == EMPTY) // 빈 칸일 경우
                        continue;
                    else if (board_copy[x + j*dx[dir]][y + j*dy[dir]] == opponent) // opponent 돌이 놓여있을 경우
                        opponentStone++;
                    else { // player 돌이 놓여있거나 mark가 놓여있을 경우
                        opponentStone = 0;
                        break;
                    }
                }
                if (opponentStone >= 4 && (IsOutOfBounds(x - dx[dir], y - dy[dir]) || board_copy[x - dx[dir]][y - dy[dir]] != opponent) && (IsOutOfBounds(x + 6 * dx[dir], y + 6 * dy[dir]) || board_copy[x + 6 * dx[dir]][y + 6 * dy[dir]] != opponent)) // threat window 안에 4개 이상의 player 돌이 존재하고, 상대방 혹은 mark 돌이 없으며 threat window와 인접한 2개의 돌이 opponent의 돌이 아닐때(7목 8목 방지용)
                    return false;
            }
        }
    }
    return true;
}
 
// 상대의 승리를 막기 위해 반드시 두어야하는 자리들의 목록을 반환.
// (만약 Threat의 수가 1일 경우 {{x, y}, {-1, -1}}을 반환. threat_cnt에는 threat의 수를 기록할 것임
// board에 opponent의 6목은 없다고 가정.(있을 경우 밑의 tmp 부분에서 오류 발생함)
vector<MOVES> Get_ForcedMove(int myBoard[][BOARD_COL], MOVES OpMoves, int player, int* threat_cnt) {
    if (OpMoves.first.X == -1) { // 이전의 상대 착수가 없다면(상대가 아직 두지 않았다면)
        *threat_cnt = 0;
        vector<MOVES> tmp;
        return tmp; // 상대의 승리를 막기 위해 반드시 두어야하는 자리가 존재할 수 없음. 빈 리스트를 반환.
    }
    int opponent = 3 - player;
    *threat_cnt = 0;
    int board_copy[BOARD_ROW][BOARD_COL];
    DeepCopy_Board(myBoard, board_copy);
    int dx[4] = { -1, 0, 1, 1 };
    int dy[4] = { 1, 1, 1, 0 };
    vector<MOVES> ForcedMove;
    vector<MOVES> ThreatList;
    POSITION tmp[2] = { OpMoves.first, OpMoves.second };
    for (int iter = 0; iter < 2; iter++) {
        if (tmp[iter].X == -1) // (-1, -1)일 경우
            continue;
        int st_x = tmp[iter].X;
        int st_y = tmp[iter].Y;
        for (int dir = 0; dir < 4; dir++) {
            for (int i = 0; i < 6; i++) {
                int x = st_x - i * dx[dir];
                int y = st_y - i * dy[dir];
                if (IsOutOfBounds(x, y) || IsOutOfBounds(x + 5 * dx[dir], y + 5 * dy[dir]))
                    continue;
                int opponentStone = 0;
                for (int j = 0; j < 6; j++) { // threat window 안의 6개 돌을 살펴보는 중
                    if (board_copy[x + j*dx[dir]][y + j*dy[dir]] == EMPTY) // 빈 칸일 경우
                        continue;
                    else if (board_copy[x + j*dx[dir]][y + j*dy[dir]] == opponent) // opponent 돌이 놓여있을 경우
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
                    for (int k = 0; k < 6; k++) {
                        if (board_copy[x + k*dx[dir]][y + k*dy[dir]] == EMPTY) {
                            board_copy[x + k*dx[dir]][y + k*dy[dir]] = MARK; // 빈 칸을 MARK로 변경
                            if (tmp[0] == -1) {
                                tmp[0] = x + k*dx[dir];
                                tmp[1] = y + k*dy[dir];
                            }
                            else {
                                tmp[2] = x + k*dx[dir];
                                tmp[3] = y + k*dy[dir];
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
        if (IsNoThreat(myBoard, { ThreatList[0].second,{ -1, -1 } }, player)) // threat 후보 중에서 하나를 메꾸니 Threat이 없어졌다면 이는 올바른 착수
            ForcedMove.push_back({ ThreatList[0].first,{ -1, -1 } });
        myBoard[ThreatList[0].first.X][ThreatList[0].first.Y] = EMPTY;
        if (ThreatList[0].second.X == -1) // 해당 threat window에 threat이 1개 있었을 경우
            return ForcedMove;
        myBoard[ThreatList[0].second.X][ThreatList[0].second.Y] = player;
        if (IsNoThreat(myBoard, { ThreatList[0].first,{ -1,-1 } }, player)) // threat 후보 중에서 하나를 메꾸니 Threat이 없어졌다면 이는 올바른 착수
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
                if (IsNoThreat(myBoard, { tmp1[1 - i], tmp2[1 - j] }, player)) // threat 후보 중에서 하나를 메꾸니 Threat이 없어졌다면 이는 올바른 착수
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
// 내가 가장 최근에 놓은 수에 대해서만 확인하면 끝임
// 보드를 꼭 갱신해야 하는건가? 일단 갱신안하도록 만들었음
MOVES Find_Conn6Move(int myBoard[][BOARD_COL], MOVES myMoves, int player) {
    if (myMoves.first.X == -1) // 이전에 착수한 수가 없다면
        return { { -1,-1 },{ -1,-1 } }; // 절대 6목을 만들 수 있는 수가 존재할 수 없음
    int board_copy[BOARD_ROW][BOARD_COL];
    DeepCopy_Board(myBoard, board_copy); // board_copy에 myBoard를 copy
    int dx[4] = { -1, 0, 1, 1 };
    int dy[4] = { 1, 1, 1, 0 };
    POSITION tmp1[2] = { myMoves.first, myMoves.second };
    for (int iter = 0; iter < 2; iter++) {
        int st_x = tmp1[iter].X;
        int st_y = tmp1[iter].Y;
        for (int dir = 0; dir < 4; dir++) {
            for (int i = 0; i < 6; i++) {
                int x = st_x - dx[dir] * i;
                int y = st_y - dy[dir] * i;
                if (IsOutOfBounds(x, y) || IsOutOfBounds(x + 5 * dx[dir], y + 5 * dy[dir]))
                    continue;
                int playerStone = 0;
                for (int j = 0; j < 6; j++) { // threat window 안의 6개 돌을 살펴보는 중
                    if (board_copy[x + j*dx[dir]][y + j*dy[dir]] == EMPTY) // 빈 칸일 경우
                        continue;
                    else if (board_copy[x + j*dx[dir]][y + j*dy[dir]] == player) // player 돌이 놓여있을 경우
                        playerStone++;
                    else { // 상대방의 돌이 놓여있을 경우
                        playerStone = 0;
                        break;
                    }
                }
                if (playerStone >= 4 && (IsOutOfBounds(x - dx[dir], y - dy[dir]) || board_copy[x - dx[dir]][y - dy[dir]] != player) && (IsOutOfBounds(x + 6 * dx[dir], y + 6 * dy[dir]) || board_copy[x + 6 * dx[dir]][y + 6 * dy[dir]] != player)) { // threat window 안에 4개 이상의 player 돌이 존재하고, 상대방 돌이 없으며 threat window와 인접한 2개의 돌이 player의 돌이 아닐때(7목 8목 방지용)
                    POSITION tmp[2] = { { 0, 0 },{ 0, 0 } };
                    int idx = 0;
                    for (int i = 0; i < 6; i++) {
                        if (board_copy[x + i*dx[dir]][y + i*dy[dir]] == EMPTY) {
                            board_copy[x + i*dx[dir]][y + i*dy[dir]] = player;
                            tmp[idx++] = { x + i*dx[dir], y + i*dy[dir] };
                        }
                    }
                    if (idx == 1) { // 5개의 player 돌로 채워진 곳이라 한 개의 돌은 다른 곳에 착수해야하는 상황일 때
                        for (int i = 0; i < BOARD_ROW; i++) {
                            for (int j = 0; j < BOARD_COL; j++) {
                                if (board_copy[i][j] != EMPTY) // 빈칸이 아닐 경우
                                    continue;
                                if (i == x - dx[dir] && j == y - dy[dir]) // 6목을 깨버릴 경우
                                    continue;
                                if (i == x + 6 * dx[dir] && j == y + 6 * dx[dir]) // 6목을 꺠버릴 경우
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
 
// 지금 두는 수의 Score를 반환
double Get_ScoreOfSingleMove(int myBoard[][BOARD_COL], POSITION myMove, FACTOR f, int player) {
    double PlayerFactor[6] = { 0.0, get<0>(f), get<1>(f), get<2>(f), 0.0, 0.0 }; // PlayerFactor[i] : 나의 착수로 window 안에 나의 돌 i개를 만들었을 때 score. 검은돌 4/5개는 어차피 Threat에서 알아서 카운트될테니 계산할 필요 없음
    double OpponentFactor[6] = { 0.0, get<3>(f), get<4>(f), get<5>(f), 0.0, 0.0 }; // OpponentFacotr[i] : 나의 착수로 window 안에 상대 돌 i개를 저지했을 때 score. 하얀돌 4/5개를 막는건 어차피 Threat에서 걸러지므로 따로 score를 부여할 필요 없음
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
 
double Get_ScoreOfDoubleMoves(int myBoard[][BOARD_COL], MOVES myMoves, FACTOR f, int player) {
    double score = 0.0;
    score += Get_ScoreOfSingleMove(myBoard, myMoves.first, f, player);
    myBoard[myMoves.first.X][myMoves.first.Y] = player; // 첫번째 착점하는 곳을 player로 채워넣고 두번째 착수에 대한 score를 합산
    score += Get_ScoreOfSingleMove(myBoard, myMoves.second, f, player);
    myBoard[myMoves.first.X][myMoves.first.Y] = EMPTY; // 다시 되돌림
    return score;
}
 
// 최선의 수를 반환(한개짜리)
POSITION Find_BestSingleMove(int myBoard[][BOARD_COL], FACTOR f, int player) {
    double maxscore = -1.0;
    POSITION bestmove = { -1, -1 };
    for (int x = 0; x < BOARD_ROW; x++) {
        for (int y = 0; y < BOARD_COL; y++) {
            if (myBoard[x][y] != EMPTY)
                continue;
            double tmpScore = Get_ScoreOfSingleMove(myBoard, { x, y }, f, player);
            if (tmpScore >= maxscore && abs(BOARD_ROW / 2 - x) + abs(BOARD_COL / 2 - y) < abs(BOARD_ROW / 2 - bestmove.X) + abs(BOARD_COL / 2 - bestmove.Y)) {
                bestmove = { x, y };
                maxscore = tmpScore;
            }
        }
    }
    return bestmove;
}
 
//최선의 MOVES 후보를 반환(최대 N개), N은 5이상
vector<MOVES_SCORE> Find_CandidateOfBestDoubleMoves(int myBoard[][BOARD_COL], MOVES myMoves, MOVES OpMoves, FACTOR f, int player, int N) {
    vector<MOVES_SCORE> candidate(N, { { { -1, -1 },{ -1, -1 } }, -1000000.0 });
    int opponent = 3 - player;
    MOVES bestmove = Find_Conn6Move(myBoard, myMoves, player);
    if (bestmove.first.X >= 0) { // 6목을 만드는 적절한 착수가 존재할 경우
        candidate[0] = { bestmove, 100000.0 };
        return candidate; // 더 살펴볼 필요 없이 최선의 수를 바로 반환
    }
    vector<MOVES> forcedmove;
    int player_threat;
    forcedmove = Get_ForcedMove(myBoard, OpMoves, player, &player_threat);
    if (player_threat >= 3) { // 막아야할 곳이 세 곳 이상이면(나의 Threat이 3이상이면)
        candidate[0] = make_pair(forcedmove[0], -1000000.0);
        return candidate; // 어차피 승산이 없으므로 아무값이나 반환함    
    }
    if (player_threat == 2) { // 둘 곳이 굉장히 한정됨. 그 한정된 자리들을 대상으로 Score를 확인해 가장 점수가 높은 곳을 찾음
        for (auto const& candidateMoves : forcedmove) {
            double tmpScore = Get_ScoreOfDoubleMoves(myBoard, candidateMoves, f, player);
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
                    double tmpScore = Get_ScoreOfDoubleMoves(myBoard, tmpMoves, f, player);
 
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
                double currentScore = Get_ScoreOfSingleMove(myBoard, { x, y }, f, player);
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
                double tmpScore = Get_ScoreOfDoubleMoves(myBoard, { highScoredPosition[i], highScoredPosition[j] }, f, player);
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
 
MOVES_SCORE Find_BestDoubleMovesByDepthSearch(int myboard[][BOARD_COL], MOVES myMoves, MOVES OpMoves, FACTOR f, int player, int Breadth, int currentDepth, int maxDepth, long st_time) {
    MOVES_SCORE tmpMax = { { { -1,-1 },{ -1,-1 } }, -10000000.0 };
    vector<MOVES_SCORE> candidate = Find_CandidateOfBestDoubleMoves(myboard, myMoves, OpMoves, f, player, Breadth);
    if (candidate[0].second < -50000.0) // 가장 최선인 수 조차 필패일 경우
        return candidate[0];
    if (candidate[0].second >= 50000.0) // 가장 최선의 수를 두면 바로 6목이 되는 경우
        return candidate[0];
    if (currentDepth == maxDepth) // 최대 깊이까지 내려갔으면
        return candidate[0]; // 더 깊게 들어갈 필요가 없음
    if (1.0 * (clock() - st_time) / (CLOCKS_PER_SEC) > 100.2) // 6.2초이상 지났으면
        return candidate[0]; // 더 파고들 시간이 없음
    int i = 0;
    for (auto const& candidateMoves_Score : candidate) { // 이제 필승/필패는 없음. 후보들을 대상으로 depth를 한 칸 더 깊게 가볼 예정
        if (candidateMoves_Score.first.first.X == -1) // 잘못된 값일경우
            break; // 이 이후로는 전부 (-1, -1), (-1, -1)이 기록되어있으므로 넘김
 
        MOVES tmp = candidateMoves_Score.first; // 내가 tmp에 착수했을 때 상대의 최선의 수를 확인할 예정
 
        myboard[tmp.first.X][tmp.first.Y] = player;
        myboard[tmp.second.X][tmp.second.Y] = player;
        MOVES_SCORE currentBest = Find_BestDoubleMovesByDepthSearch(myboard, OpMoves, tmp, f, 3 - player, Breadth, currentDepth + 1, maxDepth, st_time);
        if (tmpMax.second < candidateMoves_Score.second - currentBest.second)
            tmpMax = { tmp, candidateMoves_Score.second - currentBest.second };
        myboard[tmp.first.X][tmp.first.Y] = EMPTY;
        myboard[tmp.second.X][tmp.second.Y] = EMPTY;
    }
    return tmpMax;
}
/*
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
        x[1] = -1;
        y[0] = myMove.Y;
        y[1] = -1;
    }
    else {
        MOVES_SCORE myMove = Find_BestDoubleMovesByDepthSearch(myBoard, CurrentMyMoves, CurrentOpponentMoves, 1, 0, 3, st_time);
        x[0] = myMove.first.first.X;
        x[1] = myMove.first.second.X;
        y[0] = myMove.first.first.Y;
        y[1] = myMove.first.second.Y; // 만약 myMove에 불가능한 쌍이 들어왔다면
        if (IsOutOfBounds(x[0], y[0]) || myBoard[x[0]][y[0]] != EMPTY) {
            bool isFindDone = false;
            for (int i = 0; i < BOARD_ROW; i++) {
                for (int j = 0; j < BOARD_COL; j++) {
                    if (myBoard[i][j] == EMPTY && (i != x[1] || j != y[1])) {
                        x[0] = i;
                        y[0] = j;
                        isFindDone = true;
                        break;
                    }
                }
                if (isFindDone)
                    break;
            }
        }
        if (IsOutOfBounds(x[1], y[1]) || myBoard[x[1]][y[1]] != EMPTY) {
            bool isFindDone = false;
            for (int i = 0; i < BOARD_ROW; i++) {
                for (int j = 0; j < BOARD_COL; j++) {
                    if (myBoard[i][j] == EMPTY && (i != x[0] || j != y[0])) {
                        x[1] = i;
                        y[1] = j;
                        isFindDone = true;
                        break;
                    }
                }
                if (isFindDone)
                    break;
            }
        }
        CurrentMyMoves = { { x[0], y[0] },{ x[1], y[1] } };
    }
    // 이 부분에서 자신이 놓을 돌을 출력하십시오.
    // 필수 함수 : domymove(x배열,y배열,배열크기)
    // 여기서 배열크기(cnt)는 myturn()의 파라미터 cnt를 그대로 넣어야합니다.
    domymove(x, y, cnt);
}
*/
 
void print_board(int myBoard[][BOARD_COL]) {
    for (int x = 0; x < BOARD_ROW; x++) {
        for (int y = 0; y < BOARD_COL; y++) {
            printf("%d ", myBoard[x][y]);
        }
        printf("\n");
    }
    printf("\n");
}
int competitive(FACTOR f1, FACTOR f2, int N) { // f1과 f2이 흑을 잡고 N번 경기했을 때 이긴 횟수
    //00010203040506070809101112131415161718
    int winpt = 0; // 이기면 2, 비기면 1, 지면 0점을 더한 후 2로 나눌 계획
    for (int comp = 0; comp < N; comp++) {
        int myBoard[BOARD_ROW][BOARD_COL] = {
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 0
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 1
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 2
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 3
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 4
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 5
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 6
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 7
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 8
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 9
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 10
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 11
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 12
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 13
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 14
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 15
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 16
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // ROW 17
            { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }  // ROW 18
        };
        random_device rd;  //Will be used to obtain a seed for the random number engine
        mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        uniform_int_distribution<> dis(0, 18);
        int stuckNum = 6; // 장애물은 6개
        while (stuckNum) {
            int stuck_x = dis(gen);
            int stuck_y = dis(gen);
            if (myBoard[stuck_x][stuck_y] == EMPTY) {
                myBoard[stuck_x][stuck_y] = BLOCK;
                stuckNum--;
            }
        }  // 장애물 설치 완료
        POSITION myMove = Find_BestSingleMove(myBoard, f1, BLACK); // 흑이 먼저 착수
        myBoard[myMove.X][myMove.Y] = BLACK;
        int CurrentPlayer = WHITE;
        int winner = 0;
        MOVES CurrentWhiteMoves = { {-1, -1}, {-1, -1} };
        MOVES CurrentBlackMoves = { { -1, -1 },{ -1, -1 } };
        for (int tmp_cnt = 0; tmp_cnt < 170; tmp_cnt++) { // 170수를 둘 때 까지만 진행. 더 넘어가면 그냥 무승부라고 볼 것임
            if (CurrentPlayer == WHITE) { // 백의 차례이면
                int st_time = clock();
                MOVES_SCORE t = Find_BestDoubleMovesByDepthSearch(myBoard, CurrentWhiteMoves, CurrentBlackMoves, f2, WHITE, 2, 0, 2, st_time);
                myBoard[t.first.first.X][t.first.first.Y] = WHITE;
                myBoard[t.first.second.X][t.first.second.Y] = WHITE;
                CurrentWhiteMoves = t.first;
                if (t.second >= 50000) {
                    winner = WHITE;
                    break;
                }
                CurrentPlayer = BLACK;
            }
            else { // 흑의 차례이면
                int st_time = clock();
                MOVES_SCORE t = Find_BestDoubleMovesByDepthSearch(myBoard, CurrentBlackMoves, CurrentWhiteMoves, f1, BLACK, 2, 0, 2, st_time);
                myBoard[t.first.first.X][t.first.first.Y] = BLACK;
                myBoard[t.first.second.X][t.first.second.Y] = BLACK;
                CurrentBlackMoves = t.first;
                if (t.second >= 50000) {
                    winner = BLACK;
                    break;
                }
                CurrentPlayer = WHITE;
            }
        }
        if (winner == BLACK) // f1이 이겼으면
            winpt += 2;
        if (winner == 0) // 무승부이면
            winpt += 1;
    }
 
    
    return winpt;
}
 
FACTOR average3(FACTOR f1, FACTOR f2, FACTOR f3) {
    return { 1.0, (get<1>(f1) + get<1>(f2) + get<1>(f3)) / 3, (get<2>(f1) + get<2>(f2) + get<2>(f3)) / 3, (get<3>(f1) + get<3>(f2) + get<3>(f3)) / 3, (get<4>(f1) + get<4>(f2) + get<4>(f3)) / 3, (get<5>(f1) + get<5>(f2) + get<5>(f3)) / 3 };
}
FACTOR average4(FACTOR f1, FACTOR f2, FACTOR f3, FACTOR f4) {
    return { 1.0, (get<1>(f1) + get<1>(f2) + get<1>(f3) + get<1>(f4)) / 4, (get<2>(f1) + get<2>(f2) + get<2>(f3) + get<2>(f4)) / 4, (get<3>(f1) + get<3>(f2) + get<3>(f3) + get<3>(f4)) / 4, (get<4>(f1) + get<4>(f2) + get<4>(f3) + get<4>(f4)) / 4, (get<5>(f1) + get<5>(f2) + get<5>(f3) + get<5>(f4)) / 4 };
}
 
FACTOR next_generation(FACTOR f1, FACTOR f2) {
    double DeviationFactor = 0.2; // 클 수록 돌연변이가 잘 생김
    random_device rd;
    mt19937 gen(rd());
 
    FACTOR ret;
    get<0>(ret) = 1.0;
    double mean = (get<1>(f1) + get<1>(f2));
    mean /= 2;
    normal_distribution<> d1( mean, mean*DeviationFactor);
    get<1>(ret) = d1(gen);
    mean = get<2>(f1) + get<2>(f2);
    mean /= 2;
    normal_distribution<> d2(mean, mean*DeviationFactor);
    get<2>(ret) = d2(gen);
    mean = get<3>(f1) + get<3>(f2);
    mean /= 2;
    normal_distribution<> d3(mean, mean*DeviationFactor);
    get<3>(ret) = d3(gen);
    mean = get<4>(f1) + get<4>(f2);
    mean /= 2;
    normal_distribution<> d4(mean, mean*DeviationFactor);
    get<4>(ret) = d4(gen);
    mean = get<5>(f1) + get<5>(f2);
    mean /= 2;
    normal_distribution<> d5(mean, mean*DeviationFactor);
    get<5>(ret) = d5(gen);
    return ret;
}
void GeneticAlgorithm(void) {
    const int FACTOR_NUM = 16;
    FACTOR_SCORE table[FACTOR_NUM];
    table[0].second = { 1.0, 2.0, 6.0, 1.05, 5.0, 11.05 };
    table[1].second = { 1.0, 3.62, 5.0, 2.5, 7.235, 14.35 };
    table[2].second = { 1.0, 5.71, 6.2, 3.55, 8.1, 12.05 };
    table[3].second = { 1.0, 5.12, 14.3, 2.75, 4.1234, 8.145 };
    table[4].second = { 1.0, 3.05, 6.6, 7.05, 13.342, 34.01 };
    table[5].second = { 1.0, 7.37, 15.3, 4.05, 9.246, 13.25 };
    table[6].second = { 1.0, 5.732, 7.02, 3.05, 5.0, 11.4505 };
    table[7].second = { 1.0, 4.2, 7.14, 3.65, 5.06, 11.056 };
    table[8].second = { 1.0, 4.7, 4.323, 4.85, 9.20, 11.035 };
    table[9].second = { 1.0, 6.12, 5.62, 6.35, 8.01, 15.025 };
    table[10].second = { 1.0, 1.57, 4.0, 3.05, 5.0, 11.05 };
    table[11].second = { 1.0, 1.62, 7.0, 1.5, 7.235, 14.35 };
    table[12].second = { 1.0, 5.71, 6.2, 1.55, 5.1, 12.05 };
    table[13].second = { 1.0, 5.12, 7.3, 0.55, 14.1234, 18.145 };
    table[14].second = { 1.0, 1.25, 2.6, 3.05, 5.342, 7.01 };
    table[15].second = { 1.0, 2.25, 4.6, 7.05, 15.342, 27.01 };
    for (int Round = 1; ; Round++) {
        FILE* fp = fopen("-", "a");
        fprintf(fp, "============= Round #%d ===========\n", Round);
        printf("============= Round #%d ===========\n", Round);
 
        for (int i = 0; i < FACTOR_NUM; i++) {
            for (int j = i + 1; j < FACTOR_NUM; j++) {
                int N = 5; // 흑으로 5판, 백으로 5판
                int winpt = competitive(table[i].second, table[j].second, N) + (2 * N - competitive(table[j].second, table[i].second, N));
                table[i].first += winpt;
                table[j].first += (4 * N - winpt);
            }
        }
        sort(table, table + FACTOR_NUM, greater<FACTOR_SCORE>()); // 내림차순 정렬
        for (int i = 0; i < FACTOR_NUM; i++) {
            FACTOR f = table[i].second;
            fprintf(fp, "rank #%d : {%.2f, %.2f, %.2f, %.2f, %.2f, %.2f} - %d pts\n", i + 1, get<0>(f), get<1>(f), get<2>(f), get<3>(f), get<4>(f), get<5>(f), table[i].first);
            printf("rank #%d : {%.2f, %.2f, %.2f, %.2f, %.2f, %.2f} - %d pts\n", i + 1, get<0>(f), get<1>(f), get<2>(f), get<3>(f), get<4>(f), get<5>(f), table[i].first);
        }
        // 4위까지는 그대로 계승, 1-2, 1-3, 2-3, 1-4의 자손 4명/3명/2명/1명씩(10개), 1-(5~7위의 평균)의 자손, 2-(8~11위의 평균)의 자손
        for (int i = 0; i < FACTOR_NUM; i++)
            table[i].first = 0; // score는 0으로 초기화
        table[4].second = next_generation(table[0].second, table[1].second);
        table[5].second = next_generation(table[0].second, table[1].second);
        table[6].second = next_generation(table[0].second, table[1].second);
        table[7].second = next_generation(table[0].second, table[1].second);
        table[8].second = next_generation(table[0].second, table[2].second);
        table[9].second = next_generation(table[0].second, table[2].second);
        table[10].second = next_generation(table[0].second, table[2].second);
        table[11].second = next_generation(table[1].second, table[2].second);
        table[12].second = next_generation(table[1].second, table[2].second);
        table[13].second = next_generation(table[0].second, table[3].second);
        table[14].second = next_generation(table[0].second, average3(table[4].second, table[5].second, table[6].second));
        table[15].second = next_generation(table[1].second, average4(table[7].second, table[8].second, table[9].second, table[10].second));
        fprintf(fp, "=================================\n");
        printf("=================================\n");
        fclose(fp);
    }
 
}
int main(void) {    
    //printf("%f %f %f %f %f %f", get<0>(f), get<1>(f), get<2>(f), get<3>(f), get<4>(f), get<5>(f));
    GeneticAlgorithm();    
}
