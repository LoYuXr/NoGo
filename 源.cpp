#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <ctime>
#include <climits>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cassert>
#include <windows.h>
#include<fstream>
#define BLACK -1
#define WHITE 1   //word上黑在这里是白，没有改所以现在下出来是黑子
#define do 523 //用来控制落子音色
#define re 578
#define mi 659
#define fa 698
#define so 784
#define la 880
#define si 988
#define do1 1046
#define sso1 1661
int best1, best2;
int cnt = 0;
bool choosetype=false;
int MYSIDE = -1;
int OPPSIDE = 1; //我方下（当sidetomove返回0）还是对方下（sidetomove返回1）
int MY_COLOR = -1; //机器方子颜色
int OPP_COLOR = 1; //人方子颜色
int bw;  //记录执黑执白的情况
int hard;//记录选择的难度
using namespace std;   //起始颜色我是-1,对方是1

int board[9][9] = { 0 };
string str;//用来处理用户输入

bool dfs_air_visit[9][9];  //全局变量没有初始化就是false 表征没有访问过
const int delta_x[] = { -1,0,1,0 };
const int delta_y[] = { 0,-1,0,1 };
int timer;
double checkvalue(int k);
bool judgeAvailable(int fx, int fy, int COLOR);
bool judge( int x, int y, int color);
bool inBorder(int x, int y);
bool dfs_air(int fx, int fy);
int MinMax(int depth, int best1, int best2, int timer, int k);
void minimax_algo();
void random();
void player();
bool saveboardB();
bool saveboardW();
bool loadboardB();
bool loadboardW();

void help();
void frntpage();
void printboard();
void playminmax();
void playrandom();

//操作系统

int input() {       
	while (1) {
		cout << "                             请您输入横坐标和纵坐标(空格隔开)，或者菜单栏里的字母以执行操作" << endl;
		getline(cin, str);
		int a = str.length();
		if (a == 1) {         //如果要想执行操作，长度只为1
			switch (str[0]) {
			case 'A': help(); break;
			case 'B': {
				if (bw == BLACK)
					saveboardB();
				else if (bw == WHITE)
					saveboardW();
				cout << "                                                    保存当前局面成功！" << endl; 
				break; }
			case 'C': {
				if (bw == BLACK)
					loadboardB();
				else if (bw == WHITE)
					loadboardW();
				system("cls");
				printboard();
				cout << "                                                    读取之前局面成功！" << endl; break; }
			case 'D': {
				int flag = 0;
				for(int i=0;i<9;++i)
					for (int j = 0; j < 9; ++j) {
						if (board[i][j] != 0) {
							flag++;//可以悔棋
						}
					}
				if (flag != 0 && flag != 1) {
					if (bw == BLACK)
						loadboardB();
					else if (bw == WHITE)
						loadboardW();
					system("cls");
					printboard();
					break;
				}
				else {
					Beep(so, 200); Beep(do, 200);
					cout << "                                                 悔棋不合法！请先下再悔棋！" << endl;
					continue;
				}
			}
			case 'E': {
				Beep(so, 200); Beep(do, 200);
				cout << "                                 您确定要中途退出游戏吗？请输入“Y”退出，“N”取消退出" << endl;
				cout << "                                       退出后游戏自动保存当前局面和可用悔棋次数" << endl;
				string str1;
				cin >> str1; cin.get();
				if (str1 == "Y") {
					if(bw==BLACK)
						saveboardB(); 
					else if(bw==WHITE)
						saveboardW();
					cnt = 0; choosetype = false;
					memset(board, 0, sizeof(board));
					system("cls");
					frntpage(); 
				}
				else if (str1 == "N") {
					system("cls");
					printboard();
					break;
				}
			}
			default: {
				Beep(so, 200); Beep(do, 200);
				cout << "                                                 输入不合法，请从新尝试!!" << endl; break; }
			}
		}
		else {   //如果是输入数字
		int meme[100] = { 0 }, cnt = 0;
		for (int i0 = 0; i0 < a; ++i0) {
			if (str[i0] == ' ' || str[i0] == ',' || str[i0] == '，')
				continue;
			if (str[i0] == '-' && str[i0 + 1] - '0' >= 0 && str[i0 + 1] - '0' <= 8) {   //输入是负数，不合法
				break;
			}
			if (str[i0] - '0' >= 0 && str[i0] - '0' <= 8 &&cnt<100) {
				meme[cnt] = str[i0] - '0'; cnt++;
			}
		}
		if (cnt == 2) {
			if(bw==BLACK)
			saveboardB();
			else
				saveboardW();
			return meme[0] * 9 + meme[1];
		}
		else {        //只输入了一个数字或者没输数字输入乱码
			Beep(so, 200); Beep(do, 200);
			cout << "                                                 输入不合法，请重新尝试!!" << endl;
		}
	}
	}
}
void printboard() {
	cout << "                                            ╔═══════════════════════════╗" << endl;    //20个宽度
	cout << "                                            ║    --------菜单--------   ║" << endl;
	cout << "                                            ║ 输入以下序号，转到相应功能║" << endl;
	cout << "                                            ║          A.帮 助          ║" << endl;
	cout << "                                            ║          B.存 档          ║" << endl;
	cout << "                                            ║          C.读 档          ║" << endl;
	cout << "                                            ║          D.悔 棋          ║" << endl;
	cout << "                                            ║          E.退 出          ║" << endl;
	cout << "                                            ╚═══════════════════════════╝" << endl;
	cout << "                                              当前难度："; 
	if (hard == 0) { //简单
		cout << "简单    您执";
	}
	else {
		cout << "困难    您执";
	}
	if (OPP_COLOR == WHITE) {
		cout << "白" << endl;
	}
	if (OPP_COLOR == BLACK) {
		cout << "黑" << endl;
	}
	cout << "                                                   0 1 2 3 4 5 6 7 8" << endl;
	for (int i = 0; i < 9; ++i) {
		cout << "                                                  " << i;
		for (int j = 0; j < 9; ++j) {
			if (board[i][j] == BLACK) {
				if (board[i][j - 1] != 0 || j == 0)
					cout << "○";
				else
					cout << " ○";
			}
			else if (board[i][j] == WHITE) {
				if (board[i][j - 1] != 0 || j == 0)
					cout << "●";
				else
					cout << " ●";
			}
			else {
				if (i == 0) {
					if (j == 0)
						cout << "┏";
					else if (j == 8) {
						if (board[i][j - 1] != 0)
							cout << "┓";
						else {
							cout << "━┓";
						}
					}
					else
						if (board[i][j - 1] != 0) {
							cout << "┳";
						}
						else
							cout << "━┳";
				}
				else if (i == 8) {
					if (j == 0)
						cout << "┗";
					else if (j == 8) {
						if (board[i][j - 1] != 0) {
							cout << "┛";
						}
						else
							cout << "━┛";
					}
					else {
						if (board[i][j - 1] != 0) {
							cout << "┻";
						}
						else
						{
							cout << "━┻";
						}
					}
				}
				else {
					if (j == 0)
						cout << "┣";
					else if (j == 8) {
						if (board[i][j - 1] != 0) {
							cout << "┫";
						}
						else {
							cout << "━┫";
						}
					}
					else {
						if (board[i][j - 1] == BLACK || board[i][j - 1] == WHITE) {
							cout << "╋";
						}
						else {
							cout << "━╋";
						}
					}
				}
			}
		}
		/*if (i != 8 )
			cout << "┃" << endl;*/
		cout << endl;
	}
}
void help() {
	string r;
	system("cls");
	cout << "                    ╔════════════════════════════════════════════════════════════════════════════╗" << endl;
	cout << "                    ║                              不围棋规则介绍                                ║" << endl;
	cout << "                    ║                                                                            ║" << endl;
	cout << "                    ║    不围棋由围棋衍生而来，属于两人零和博弈，规则与围棋相反。具体规则如下：  ║" << endl;
	cout << "                    ║                                                                            ║" << endl;
	cout << "                    ║   ------------------------- 一、基本概念 -------------------------------   ║" << endl;
	cout << "                    ║  1. “气”：一个棋子在棋盘上与棋子上下左右紧邻的空点叫棋子的“气”。不围棋 ║" << endl;
	cout << "                    ║  的决策就是让自己不使对方和己方棋子处于无气状态（就是围棋里的“吃子”）。  ║" << endl;
	cout << "                    ║  同时尽可能让对方无子子可下。                                              ║" << endl;
	cout << "                    ║                                                                            ║" << endl;
	cout << "                    ║  2. “眼”：本方几个连接在一起的棋子围住一个或两个空交叉点称为“眼”。眼分 ║" << endl;
	cout << "                    ║  “死眼”与“活眼” ：“活眼”是指围出的空位己方能落子（保持己方子有气）， ║" << endl;
	cout << "                    ║ 而对方不能落子（自杀）；“死眼”是指围出的空位己方和对方都不能落子。玩家在 ║" << endl;
	cout << "                    ║ 进行游戏时应尽力多造活眼，并且阻止对方造出活眼，同时避免己方的眼成为死眼。 ║" << endl;
	cout << "                    ║                                                                            ║" << endl;
	cout << "                    ║   ------------------------- 二、对弈规则 -------------------------------   ║" << endl;
	cout << "                    ║  1.  不围棋对弈的目标不是吃掉对方的棋子。恰恰相反，如果一方落子后 吃掉了对 ║" << endl;
	cout << "                    ║ 方的棋子,则落子一方判负；                                                  ║" << endl;
	cout << "                    ║  2. 对弈禁止自杀，即令己方落子处于无气状态，落子自杀一方判负；             ║" << endl;
	cout << "                    ║  3. 对弈过程中不允许空手(pass)。                                           ║" << endl;
	cout << "                    ║                                                                            ║" << endl;
	cout << "                    ║   --------------------------- 三、棋盘 ---------------------------------   ║" << endl;
	cout << "                    ║ 不围棋棋盘与9*9围棋棋盘相同，坐标从0开始，纵轴为x,横轴为y，输入时按照先输x ║" << endl;
	cout << "                    ║ ，后输y的方式进行。玩家可选择执黑执白，黑子先手，双方轮流落子。            ║" << endl;
	cout << "                    ║                                                                            ║" << endl;
	cout << "                    ╚════════════════════════════════════════════════════════════════════════════╝" << endl;
	//之后再做一个返回输入R返回
	if (choosetype==true) {
		cout << "                                                  请输入小写r返回棋盘" << endl;
		while (cin >> r) {
			if (r == "r") {
				system("cls");
				printboard();
				return;
			}
			else {
				Beep(so, 200); Beep(do, 200);
				cout << "                                                输入不合法，请重新输入" << endl;
			}
		}
	}
	else {
		cout << "                                       请输入大写R返回主菜单，并选择游戏难度" << endl;
		while (cin >> r) {
			if (r == "R") {
				system("cls");
				frntpage();
				return;
			}
			else {
				Beep(so, 200); Beep(do, 200);
				cout << "                                                输入不合法，请重新输入" << endl;
			}
		}
	}
	
}
bool judge(int dx, int dy, int color) {   
	if (!judgeAvailable(dx, dy, color))//这个点我不能下
		return false;
	return true;
}  //有余力应该再写一个输入前判断能否下子的

bool saveboardB()//存档
{
	ofstream fout("cundangB.txt", ios::out);
	if (!fout.is_open())
		return false;

	for (int i = 0; i < 9; ++i)//把board写入文件
	{
		for (int j = 0; j < 9; ++j)
			fout << board[i][j] << ' ';
		fout << endl;
	}
	fout.close();
	return true;
}
bool saveboardW()//存档
{
	ofstream fout("cundangW.txt", ios::out);
	if (!fout.is_open())
		return false;

	for (int i = 0; i < 9; ++i)//把board写入文件
	{
		for (int j = 0; j < 9; ++j)
			fout << board[i][j] << ' ';
		fout << endl;
	}
	fout.close();
	return true;
}
bool loadboardB() //读取上次游戏
{
	ifstream fin("cundangB.txt", ios::in);
	if (!fin.is_open())
		return false;
	for (int i = 0; i < 9; ++i) //把文件里的数据写入board
		for (int j = 0; j < 9; ++j)
			fin >> board[i][j];
	fin.close();
	return true;
}
bool loadboardW() //读取上次游戏
{
	ifstream fin("cundangW.txt", ios::in);
	if (!fin.is_open())
		return false;
	for (int i = 0; i < 9; ++i) //把文件里的数据写入board
		for (int j = 0; j < 9; ++j)
			fin >> board[i][j];
	fin.close();
	return true;
}




void blackwhite() {
	while (1) {
		cout << "                               请选择执黑执白，黑子先手，选择黑子输入“B”，白子输入“W”" << endl;
		string s;
		cin >> s;
		if (s == "B") {    //创建，黑子是1，记住先手
			ofstream fout("color.txt", ios::out);
			OPP_COLOR = BLACK; MY_COLOR = WHITE;
			fout << 1 << endl;
			fout.close();
			return;
		}
		else if (s == "W") {    //创建，白子是-1，记住后手
			ofstream fout("color.txt", ios::out);
			OPP_COLOR = WHITE; MY_COLOR = BLACK;
			fout << -1 << endl;
			fout.close();
			return;
		}
		else {
			Beep(so, 200); Beep(do, 200);
			cout << "                                                输入不合法，请重新输入" << endl;
		}
	}
	cin.get();
}

void frntpage() {
	string r;
	system("cls");
	cout << "                               ╔══════════════════════════════════════════════════════╗" << endl;
	cout << "                               ║                        不围棋                        ║" << endl;
	cout << "                               ║                                                      ║" << endl;
	cout << "                               ║                   欢迎您尝试不围棋!                  ║" << endl;
	cout << "                               ║                                                      ║" << endl;
	cout << "                               ║                    查看帮助请按“A”                 ║" << endl;
	cout << "                               ║                   开始新游戏请按“B”                ║" << endl;
	cout << "                               ║                    退出游戏请按“C”                 ║" << endl;
	cout << "                               ║                                                      ║" << endl;
	cout << "                               ║               制作人：罗宇轩 2000017426              ║" << endl;
	cout << "                               ╚══════════════════════════════════════════════════════╝" << endl;
	cout << "                                               请输入您想执行的操作符号" << endl;
	while (cin >> r) {
		if (r == "A") {
			system("cls");
			help();
			return;
		}
		else if (r == "B") {
			string input1;
			cout << "                                       请选择游戏难度，简单请输入E，困难请输入H" << endl;
			while (cin >> input1) {
				if (input1 == "E") {
					choosetype = true; hard = 0;
					cin.get();    //吃掉回车
					system("cls"); 
					blackwhite();//判断黑白
					system("cls");
					playrandom(); break;
				}
				else if (input1 == "H") {
					choosetype = true; hard = 1;
					cin.get();    //吃掉回车
					system("cls"); 
					blackwhite();//判断黑白
					system("cls"); 
					playminmax(); break;
				}
				else {
					Beep(so, 200); Beep(do, 200);
					cout << "                                                输入不合法，请重新输入" << endl;
				}
			}
		}
		else if (r == "C") {
			Beep(so, 200); Beep(la, 200); Beep(si, 200); Beep(so, 200); Beep(do1, 200);
			cout << "                                                          再见！" << endl;
			exit(1);
		}
		else {
			Beep(so, 200); Beep(do, 200);
			cout << "                                                输入不合法，请重新输入" << endl;
		}
	}
}
bool loadcolor() {
	ifstream fin("color.txt", ios::in);
	if (!fin.is_open())
		return false;

		fin >> bw;
	fin.close();
	return true;
}
void playminmax() {
	loadcolor();
	if(bw==-1){//白子后手
		while (1) {
			system("cls");
			printboard();
			srand((unsigned)time(0));
			//以下处理输入（注意系统稳定性）
			minimax_algo();
			if (cnt == 0) { cin.get(); cnt++; }
			player();
		}
	}
	else if (bw == 1) {//黑子先手
		while (1) {
			system("cls");
			printboard();
			srand((unsigned)time(0));
			//以下处理输入（注意系统稳定性）
			if (cnt == 0) { cin.get(); cnt++; }
			player();
			minimax_algo();
			
		}
	}
}
void playrandom() {
	loadcolor();
	if (bw == -1) {//白子后手
		memset(board, 0, sizeof(board));
		while (1) {
			system("cls");
			printboard();
			srand((unsigned)time(0));
			random();
			if (cnt == 0) { cin.get(); cnt++; }
			player();
		}
	}
	else {
		memset(board, 0, sizeof(board));
		while (1) {
			system("cls");
			printboard();
			srand((unsigned)time(0));
			if (cnt == 0) { cin.get(); cnt++; }
			player();
			random();
		}
	}
}


//核心代码

double checkvalue(int k)
{
	double myvalue = 0;
	for (int x = 0; x < 9; ++x)
		for (int y = 0; y < 9; ++y)
		{
			if (!board[x][y]) {
				if (judgeAvailable(x, y, MY_COLOR))
				{
					myvalue += 7;
				}
				if (!judgeAvailable(x, y, OPP_COLOR))
				{
					myvalue += 7;
				}
				double my = 0, opp = 0;
				for (int i = 0; i < 4; ++i)
				{
					int sur_x = x + delta_x[i], sur_y = y + delta_y[i];
					if (!inBorder(sur_x, sur_y))
					{
						++my, ++opp;
						continue;
					}
					if (board[sur_x][sur_y] == MY_COLOR)
						++my;
					else if (board[sur_x][sur_y] == OPP_COLOR)
						++opp;
				}
				if (my == 4)
					myvalue += 11;
				else if (opp == 4)
					myvalue -= 15;  //这样无论出不出界都可以查到眼
			}
			if (k > 28) {
				if (x >= 1 && x < 8 && y >= 1 && y < 8) {
					if (board[x][y] == OPP_COLOR && board[x + 1][y + 1] == OPP_COLOR) {
						if (((board[x + 1][y] == MY_COLOR && (board[x + 2][y] != MY_COLOR ||
							board[x + 1][y - 1] != MY_COLOR)) || (board[x][y + 1] == MY_COLOR && (board[x][y + 2] != MY_COLOR ||
								board[x - 1][y + 1] != MY_COLOR))))
							myvalue += pow(1.3, (k * 1.0 / 10));//对方划线挨着堵
					}
					if (board[x][y] == OPP_COLOR && board[x + 1][y - 1] == OPP_COLOR &&((board[x + 1][y] == MY_COLOR && (board[x + 1][y + 1] != MY_COLOR || 
						board[x + 2][y] != MY_COLOR)) ||(board[x][y - 1] == MY_COLOR && (board[x - 1][y - 1] != MY_COLOR || board[x][y - 2] != MY_COLOR))))
						myvalue += pow(1.3, (k * 1.0 / 10));   //加了查对方画线（斜）   //阶段估值 改成浮动
				}
				if (x >= 2 && x < 7 && y >= 2 && y < 7) {
					if (board[x][y] == OPP_COLOR && board[x][y + 2] == OPP_COLOR && board[x][y + 1] == MY_COLOR)
						myvalue += pow(1.3, (k * 1.0 / 9));
					if (board[x][y] == OPP_COLOR && board[x + 2][y] == OPP_COLOR && board[x + 1][y] == MY_COLOR)
						myvalue += pow(1.3, (k * 1.0 / 9));//加了查对方画线（横纵）
				}
			}

		}
	return myvalue;
}

inline bool inBorder(int x, int y) { return x >= 0 && y >= 0 && x < 9 && y < 9; }

//true: has air
bool dfs_air(int fx, int fy)//深搜判断有没有气
{
	dfs_air_visit[fx][fy] = true;//不希望一个点重复搜
	bool flag = false;
	for (int dir = 0; dir < 4; ++dir)
	{
		int dx = fx + delta_x[dir], dy = fy + delta_y[dir];
		if (inBorder(dx, dy))  //inborder 限制在界面里头
		{
			if (board[dx][dy] == 0)  //空位置（有气）
				flag = true;
			if (board[dx][dy] == board[fx][fy]/*（fx,fy这个点和周围四个点同色棋子）*/ && !dfs_air_visit[dx][dy]) //判断四个点没有被访问过
				if (dfs_air(dx, dy))//递归（好传统！）如果他有气
					flag = true;
		}
	}
	return flag;
}

//true: available
bool judgeAvailable(int fx, int fy, int COLOR)  //合法下棋 (外头2重循环)
{
	if (board[fx][fy]) return false;  //只要不等于零，有子不能下
	board[fx][fy] = COLOR; //黑方白方下棋 side 就是-1/-1 先放上去试一下
	memset(dfs_air_visit, 0, sizeof(dfs_air_visit));//对每一个点都要判断
	if (!dfs_air(fx, fy))//此处没气   （保证不能自杀）
	{
		board[fx][fy] = 0;//把刚才放上去的恢复
		return false;//不合法
	}
	for (int i = 0; i < 4; ++i) //判断旁边的4点 巧妙：判断旁边4点有无气 （现在已经放上去，如果旁边点没气就说明围死别人，不合法）
	{
		int dx = fx + delta_x[i], dy = fy + delta_y[i];
		if (inBorder(dx, dy))//保证在棋盘上
		{
			if (board[dx][dy]/*有子*/ && !dfs_air_visit[dx][dy])//判断没搜过0的时候才
				if (!dfs_air(dx, dy))  //判断旁边点没有气
				{
					board[fx][fy] = 0;
					return false;
				}
		}
	}
	board[fx][fy] = 0;//恢复棋盘
	return true;
}

int MinMax(int depth, int best1, int best2, int timer, int k) {  //best1是最大下界α int-min(对于我方取大) ; best2是最小上界β int-max(对于对方取小)
	if (depth <= 0) {
		return checkvalue(k);
	}
	if (timer == MYSIDE) {  // 我方是“最大”者
		int w = 0; int rec[81][2]; int val;
		for (int x = 0; x < 9; x++)
			for (int y = 0; y < 9; y++) {
				if (judgeAvailable(x, y, MY_COLOR)) {
					rec[w][0] = x; rec[w][1] = y; ++w;//记录
				}
			}
		for (int i = 0; i < w; i++) {
			board[rec[i][0]][rec[i][1]] = MY_COLOR;
			val = MinMax(depth - 1, best1, best2, -timer, k);
			if (val > best1) {
				best1 = val;
			}
			board[rec[i][0]][rec[i][1]] = 0;//回溯
			if (best1 >= best2)
				return best2;
		}
		return best1;
	}
	else { //对手走棋
		int w = 0; int rec[81][2]; int val;
		for (int x = 0; x < 9; x++)
			for (int y = 0; y < 9; y++) {
				if (judgeAvailable(x, y, OPP_COLOR)) {
					rec[w][0] = x; rec[w][1] = y; w++;//记录
				}
			}
		for (int i = 0; i < w; i++) {
			board[rec[i][0]][rec[i][1]] = OPP_COLOR;
			val = MinMax(depth - 1, best1, best2, -timer, k);
			if (val < best2) {
				best2 = val;
			}
			board[rec[i][0]][rec[i][1]] = 0;//回溯
			if (best1 >= best2)
				return best1;
		}
		return best2;
	}
}

void random() {         //随机策略

		//以下为机器读入

		//以下为随机策略 
		vector<int> available_list; //合法位置表 
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
				if (judgeAvailable(i, j, MY_COLOR))    //未调试
					available_list.push_back(i * 9 + j);
		int result = available_list[rand() % available_list.size()];
		if (!judge(result / 9, result % 9, MY_COLOR)) {
			choosetype = false; cnt = 0;
			memset(board, 0, sizeof(board));
			cout << "                                               程序落子不合法，用户胜利！" << endl;
			Beep(so, 200); Beep(la, 200); Beep(si, 200); Beep(so, 200); Beep(do1, 200);
			cout << "                                                 五秒后系统自动返回菜单" << endl;
			Sleep(5000);
			return frntpage();
		}
		board[result / 9][result % 9] = MY_COLOR;
		system("cls");
		printboard();
		Beep(sso1, 100);
}

void minimax_algo() {    //困难版本

		//以下为机器读入
		double bestchoice = -INFINITY;
		vector<int> bestpos;
		int legalm[81] = { 0 };
		int k = 0;
		for (int i = 0; i < 9; ++i)
			for (int j = 0; j < 9; ++j) {
				if (judgeAvailable(i, j, MY_COLOR)) {//在最外层搜一遍，之后遍历能下的//第二：可下的点越来越少：把第一层max数组拿来用，扫一遍
					legalm[k] = i * 9 + j;
					k++;
				}
			}
		if (k > 73) {
			if (board[0][0] == OPP_COLOR && judgeAvailable(1, 0, MY_COLOR) == 1) {
				bestpos.push_back(9);
			}
			else if (board[0][0] == OPP_COLOR && judgeAvailable(0, 1, MY_COLOR) == 1) {
				bestpos.push_back(1);
			}
			else if (board[0][8] == OPP_COLOR && judgeAvailable(0, 7, MY_COLOR) == 1) {
				bestpos.push_back(7);
			}
			else if (board[0][8] == OPP_COLOR && judgeAvailable(1, 8, MY_COLOR) == 1) {
				bestpos.push_back(17);
			}
			else if (board[8][8] == OPP_COLOR && judgeAvailable(7, 8, MY_COLOR) == 1) {
				bestpos.push_back(71);
			}
			else if (board[8][8] == OPP_COLOR && judgeAvailable(8, 7, MY_COLOR) == 1) {
				bestpos.push_back(79);
			}
			else if (board[8][0] == OPP_COLOR && judgeAvailable(7, 0, MY_COLOR) == 1) {
				bestpos.push_back(63);
			}
			else if (board[8][0] == OPP_COLOR && judgeAvailable(8, 1, MY_COLOR) == 1) {
				bestpos.push_back(73);
			}
			else if (board[0][0] == OPP_COLOR || board[0][8] == OPP_COLOR || board[8][0] == OPP_COLOR || board[8][8] == OPP_COLOR
				|| (board[0][1] == OPP_COLOR && board[1][0] == 0) || (board[1][0] == OPP_COLOR && board[0][1] == 0)
				|| (board[0][7] == OPP_COLOR && board[1][8] == 0) || (board[1][8] == OPP_COLOR && board[0][7] == 0)
				|| (board[8][1] == OPP_COLOR && board[7][0] == 0) || (board[7][0] == OPP_COLOR && board[8][1] == 0)
				|| (board[7][8] == OPP_COLOR && board[8][7] == 0) || (board[8][7] == OPP_COLOR && board[7][8] == 0)
				|| (board[0][0] == OPP_COLOR && board[1][1] == OPP_COLOR) || (board[7][7] == OPP_COLOR && board[8][8] == OPP_COLOR)
				|| (board[0][8] == OPP_COLOR && board[1][7] == OPP_COLOR) || (board[8][0] == OPP_COLOR && board[7][1] == OPP_COLOR))
			{  //如果对方上来准备做眼或者按对角线划线
				for (int i = 0; i < k; i++)
				{   //遍历求可下位置的估值并同时取最大的值与位置
					board[legalm[i] / 9][legalm[i] % 9] = MY_COLOR;  //对每一个位置先下上
					double rec = MinMax(1, INT_MIN, INT_MAX, 1, k);
					if (bestchoice < rec) {
						bestchoice = rec;
						bestpos.clear();
						bestpos.push_back(legalm[i]);
					}
					if (bestchoice == rec) {
						bestpos.push_back(legalm[i]);
					}
					board[legalm[i] / 9][legalm[i] % 9] = 0;//回溯
				}
			}
			else {
				int flagsss = 0;

				if (board[8][0] == 0 && judgeAvailable(8, 0, MY_COLOR) == 1) {
					bestpos.push_back(72);
				}
				else if (board[0][8] == 0 && judgeAvailable(0, 8, MY_COLOR) == 1) {
					bestpos.push_back(8);
				}
				else if (board[8][8] == 0 && judgeAvailable(8, 8, MY_COLOR) == 1) {
					bestpos.push_back(80);
				}
				else if (board[0][0] == 0 && judgeAvailable(0, 0, MY_COLOR) == 1) {
					bestpos.push_back(0);
				}

				else {
					for (int j = 8; j >= -8; --j) {
						for (int i = 0; i <= 8; i++) {
							if (inBorder(i + j, i - j)) {
								if (board[i + j][i - j] == 0 && judgeAvailable(i + j, i - j, MY_COLOR) == 1) {
									bestpos.push_back(9 * (i + j) + i - j);
									flagsss = 1;
									break;
								}
							}
							if (flagsss == 1)
								break;
						}
					}
				}
			}

		}
		if (k > 27 && k <= 73) {
			for (int i = 0; i < k; i++) {//遍历求可下位置的估值并同时取最大的值与位置
				board[legalm[i] / 9][legalm[i] % 9] = MY_COLOR;  //对每一个位置先下上
				double rec = MinMax(1, INT_MIN, INT_MAX, 1, k);
				if (bestchoice < rec) {
					bestchoice = rec;
					bestpos.clear();
					bestpos.push_back(legalm[i]);
				}
				if (bestchoice == rec) {
					bestpos.push_back(legalm[i]);
				}
				board[legalm[i] / 9][legalm[i] % 9] = 0;//回溯
			}
		}
		if (k <= 27) {
			for (int i = 0; i < k; i++) {//遍历求可下位置的估值并同时取最大的值与位置
				board[legalm[i] / 9][legalm[i] % 9] = MY_COLOR;  //对每一个位置先下上
				double rec = MinMax(3, INT_MIN, INT_MAX, 1, k);
				if (bestchoice < rec) {
					bestchoice = rec;
					bestpos.clear();
					bestpos.push_back(legalm[i]);
				}
				if (bestchoice == rec) {
					bestpos.push_back(legalm[i]);
				}
				board[legalm[i] / 9][legalm[i] % 9] = 0;//回溯
			}
		}
		int bpos = bestpos[rand() % bestpos.size()];//此处加judge 代码
		if (!judge(bpos / 9, bpos % 9, MY_COLOR)) {
			choosetype = false; cnt = 0;
			memset(board, 0, sizeof(board));
			cout << "                                               程序落子不合法，用户胜利！" << endl;
			Beep(so, 200); Beep(la, 200); Beep(si, 200); Beep(so, 200); Beep(do1, 200);
			cout << "                                                 五秒后系统自动返回菜单" << endl;
			Sleep(5000);
			return frntpage();
		}
		board[bpos / 9][bpos % 9] = MY_COLOR;
		system("cls");
		printboard();
		Beep(sso1, 100);

}

void player() {
	int cur = input();
	int dx = cur / 9, dy = cur % 9;
	if (!judge(dx, dy, OPP_COLOR)) {
		choosetype = false; cnt = 0;
		memset(board, 0, sizeof(board));
		cout << "                                               用户落子不合法，程序胜利！" << endl;
		Beep(so, 200); Beep(fa, 200); Beep(mi, 200); Beep(re, 200); Beep(do, 200);
		cout << "                                                 五秒后系统自动返回菜单" << endl;
		Sleep(5000);
		return frntpage();
	}
	board[dx][dy] = OPP_COLOR;
	Beep(do, 100);
}
int main()    //记住while1 !！裁判代码判断胜负
{
	frntpage();
	return 0;
}