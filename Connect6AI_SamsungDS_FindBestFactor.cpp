void test(void) {
    FACTOR_SCORE chal[37]; // 도전자
    FACTOR currentBest = { 1.00, 3.83, 14.37, 2.40, 4.40, 26.44 };
    chal[0].second = { 1.00, 3.24, 6.98, 1.39, 5.75, 9.90 };
    chal[1].second = { 1.00, 5.97, 11.26, 1.26, 4.28, 8.13 };
    chal[2].second = { 1.00, 2.82, 9.09, 1.21, 2.95, 9.21 };
    chal[3].second = { 1.00, 3.76, 10.89, 1.54, 2.58, 8.53 };
    chal[4].second = { 1.00, 3.85, 20.33, 1.77, 5.00, 26.28 };
    chal[5].second = { 1.00, 4.69, 9.00, 0.11, 5.28, 20.68 };
    chal[6].second = { 1.00, 11.15, 18.36, 0.13, 2.54, 29.47 };
    chal[7].second = { 1.00, 4.99, 13.68, 2.80, 6.81, 22.62 };
    chal[8].second = { 1.00, 2.13, 7.10, 2.12, 3.44, 12.46 };
    chal[9].second = { 1.00, 5.35, 13.88, 3.07, 6.57, 38.31 };
    chal[10].second = { 1.00, 3.31, 9.48, 1.09, 6.96, 15.24 };
    chal[11].second = { 1.00, 3.73, 16.35, 1.27, 8.20, 14.71 };
    chal[12].second = { 1.00, 3.96, 12.05, 1.33, 6.79, 19.52 };
    chal[13].second = { 1.00, 6.07, 24.75, 1.71, 4.80, 31.47 };
    chal[14].second = { 1.00, 7.73, 31.06, 1.51, 7.02, 52.02 };
    chal[15].second = { 1.00, 9.05, 32.53, 5.02, 6.09, 60.27 };
    chal[16].second = { 1.00, 9.57, 61.24, 1.54, 16.54, 62.75 };
    chal[17].second = { 1.00, 14.02, 41.60, 3.20, 23.97, 88.37 };
    chal[18].second = { 1.00, 11.31, 47.89, 2.19, 15.61, 91.31 };
    chal[19].second = { 1.00, 20.97, 80.93, 3.95, 19.05, 144.88 };
    chal[20].second = { 1.00, 25.53, 89.94, 1.41, 17.67, 172.74 };
    chal[21].second = { 1.00, 55.61, 243.36, 2.57, 189.16, 463.81 };
    chal[22].second = { 1.00, 4.17, 11.39, 2.35, 10.20, 27.04 };
    chal[23].second = { 1.00, 4.32, 13.31, 1.73, 11.16, 25.96 };
    chal[24].second = { 1.00, 5.20, 17.62, 1.84, 8.92, 34.61 };
    chal[25].second = { 1.00, 6.32, 18.06, 1.35, 4.84, 26.61 };
    chal[26].second = { 1.00, 3.20, 18.99, 1.28, 4.33, 18.32 };
    chal[27].second = { 1.00, 5.45, 20.80, 2.02, 7.39, 22.54 };
    chal[28].second = { 1.00, 6.24, 18.58, 2.20, 5.82, 27.10 };
    chal[29].second = { 1.00, 3.78, 18.36, 4.02, 5.14, 34.16 };
    chal[30].second = { 1.00, 3.04, 18.31, 3.21, 7.09, 34.08 };
    chal[31].second = { 1.00, 5.02, 18.31, 4.86, 15.15, 34.75 };
    chal[32].second = { 1.00, 7.37, 22.71, 4.71, 10.81, 55.96 };
    chal[33].second = { 1.00, 8.61, 21.50, 7.92, 11.37, 36.23 };
    chal[34].second = { 1.00, 6.21, 22.03, 6.41, 14.04, 36.74 };
    chal[35].second = { 1.00, 4.25, 23.28, 6.91, 17.52, 58.98 };
    chal[36].second = { 1.00, 6.86, 20.38, 5.28, 9.73, 51.49 };
    chal[37].second = { 1.00, 4.52, 17.27, 5.07, 14.10, 45.21 };
 
    BreadthDepth BD1 = { 3, 3 };
    BreadthDepth BD2 = { 3, 3 };
    FILE* fp = fopen("------------", "a");
    int N = 100; // 흑으로 100판 백으로 100판 붙을 계획
 
    for (int i = 0; i < 3; i++) {
        // challenger가 기준
        printf("challenger #%d : ", i );
        fprintf(fp, "challenger #%d : ", i);
        int winpt_B = competitive(chal[i].second, BD1, currentBest, BD2, N);
        int winpt_W = competitive(currentBest, BD1, chal[i].second, BD2, N);
        printf("BLACK %.2f%% WHITE %.2f%%\n", 100.0 * winpt_B / (2 * N), 100.0 * winpt_W / (2 * N));
        fprintf(fp, "BLACK %.2f%% WHITE %.2f%%\n", 100.0 * winpt_B / (2 * N), 100.0 * winpt_W / (2 * N));
        chal[i].first = winpt_B + winpt_W;
    }
    sort(chal, chal + 37, greater<FACTOR_SCORE>());
    printf("==================\n");
    fprintf(fp, "==================\n");
 
    for (int i = 0; i < 37; i++) {
        FACTOR f = chal[i].second;
        printf("Rank #%d : {%.2f, %.2f, %.2f, %.2f, %.2f, %.2f} - %.2f%%\n", i + 1, get<0>(f), get<1>(f), get<2>(f), get<3>(f), get<4>(f), get<5>(f), 100.0 * chal[i].first / (4 * N));
        fprintf(fp, "Rank #%d : {%.2f, %.2f, %.2f, %.2f, %.2f, %.2f} - %.2f%%\n", i + 1, get<0>(f), get<1>(f), get<2>(f), get<3>(f), get<4>(f), get<5>(f), 100.0 * chal[i].first / (4 * N));
    }
    fclose(fp);    
}
